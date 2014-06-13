#include "explorerFSM.h"

// ----------------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------

State::State(WorldKB& _worldKB) : worldKB(_worldKB)
{
	cout << "   Create state\n ";
}

State::~State()
{
	cout << "   Delete state\n";
}

WorldKB& State::getWorldKB()
{
	return this->worldKB;
}
// --------------------- ---------------- ---------------- ---------------- ---------------- ----------------
State1_Init::State1_Init(WorldKB& _worldKB) : State(_worldKB)
{
	cout << "   fare chiamatra di sisrema morgul muovi cacca\n";
}

State1_Init::~State1_Init() { ; }

State* State1_Init::executeState(void)
{
  //camera_angle=-90;
  //QR_found=0;
  delete this;
  return new State2_Zaghen(this->getWorldKB());
}

// --------------------- ---------------- ---------------- ---------------- ---------------- ----------------

State2_Zaghen::State2_Zaghen(WorldKB& _worldKB) : State(_worldKB)
{
	camera_id = 0;
	int camera_angle = this->getWorldKB().getCameraAngle();
	cout << "   State2_Finding state\n";
	qrStuff.temp_qr_info.message_length = -1;

	const char* filename = "out_camera_data.yml";
	CvFileStorage* fs = cvOpenFileStorage(filename, 0, CV_STORAGE_READ);
	if (fs==NULL) {
		printf("Error during calibration file loading.\n");
		exit(76);
	}
	this->intrinsic_matrix = (CvMat*)cvReadByName(fs, 0, "camera_matrix");
	this->distortion_coeffs = (CvMat*)cvReadByName(fs, 0, "distortion_coefficients");
	this->scale_factor = cvReadIntByName(fs, 0, "scale_factor");
	this->qr_size_mm = cvReadIntByName(fs, 0, "qr_size_mm");

	resetQR();

	capture.open(camera_id);
	if (!capture.isOpened()) {
		printf("Error during capture opening.\n");
		exit(1);
	  }

	// ------------------------------------------------------------------------------------------ currentState = new State1_Init();
	cout << "Zaghen built! \n";
}

State2_Zaghen::~State2_Zaghen() { ; }

State* State2_Zaghen::executeState()
{

	printf("Scanning thread started.\n");
	Mat frame, frame_undistort, frame_BW;
	while(1) {

        if( !this->capture.isOpened() )
			{ perror("Error opening capture.\n"); ThreadOnMutex::closeAllThreads(); }

		Mat frame0;
        capture >> frame0;
        frame0.copyTo(frame);

		if (!frame.data)
			{ perror("Error loading the frame.\n"); ThreadOnMutex::closeAllThreads(); }

		undistort(frame, frame_undistort, intrinsic_matrix, distortion_coeffs);
		cvtColor(frame_undistort, frame_BW, CV_BGR2GRAY);

		cv_to_quirc(this->qrStuff.q, frame_BW);
		processQR();
		resetQR();

		int key = 0xff & waitKey(capture.isOpened() ? 50 : 500);
        if( (key & 255) == 27 )
           exit(0);
	}


  delete this;
  return new State3_StatusChecking(this->getWorldKB());
}

// --------------------- ---------------- ---------------- ---------------- ---------------- ----------------

State3_StatusChecking::State3_StatusChecking(WorldKB& _worldKB) : State(_worldKB)
{
	cout << "   State4_StatusChecking state\n";
}

State3_StatusChecking::~State3_StatusChecking() { ; }

State* State3_StatusChecking::executeState()
{
  delete this;
  return new State4_Localizing(this->getWorldKB());
}

// --------------------- ---------------- ---------------- ---------------- ---------------- ----------------

State4_Localizing::State4_Localizing(WorldKB& _worldKB) : State(_worldKB)
{
	cout << "   State5_Localizing state\n";
}

State4_Localizing::~State4_Localizing() { ; }

State* State4_Localizing::executeState()
{
  delete this;
  return NULL;
}

// --------------------- ---------------- ---------------- ---------------- ---------------- ----------------

State5_Error::State5_Error(WorldKB& _worldKB) : State(_worldKB)
{
	cout << "   State5_Localizing state\n";
}

State5_Error::~State5_Error() { ; }

State* State5_Error::executeState()
{
  delete this;
  return NULL;
}

// --------------------- ---------------- ---------------- ---------------- ---------------- ----------------
// --------------------- ---------------- ---------------- ---------------- ---------------- ----------------
// --------------------- ---------------- ---------------- ---------------- ---------------- ----------------
// --------------------- ---------------- ---------------- ---------------- ---------------- ----------------
// --------------------- ---------------- ---------------- ---------------- ---------------- ----------------

ExplorerFSM::ExplorerFSM(WorldKB& _worldKB, int _camera_id) : ThreadOnMutex(_worldKB) {

	setThreadIndex(EXPLORERTHREADINDEX);
	camera_id = _camera_id;

	/// ci va qualcosa qui ???? boh
}

ExplorerFSM::~ExplorerFSM()
{ ;
}

void ExplorerFSM::setCurrentState(State *s)
{
	currentState = s;
}

void ExplorerFSM::runFSM()
{
	int ret = pthread_create(getOwnThread(), NULL, &ExplorerFSM::call_threadBehaviour, NULL);
	if(ret != 0) {
		printf("Error creating the scanner thread. [%s]\n", strerror(ret));
		ThreadOnMutex::closeAllThreads();
		exit(1);
	}
}

// --HERE STARTS SET OF METHODS OF OLD INSPECT CLASS -----------------------------------------------------------------------------
void State2_Zaghen::copyCorners() {

	this->qrStuff.temp_qr_info.x0 = this->qrStuff.code->corners[0].x;
	this->qrStuff.temp_qr_info.y0 = this->qrStuff.code->corners[0].y;
	this->qrStuff.temp_qr_info.x1 = this->qrStuff.code->corners[1].x;
	this->qrStuff.temp_qr_info.y1 = this->qrStuff.code->corners[1].y;
	this->qrStuff.temp_qr_info.x2 = this->qrStuff.code->corners[2].x;
	this->qrStuff.temp_qr_info.y2 = this->qrStuff.code->corners[2].y;
	this->qrStuff.temp_qr_info.x3 = this->qrStuff.code->corners[3].x;
	this->qrStuff.temp_qr_info.y3 = this->qrStuff.code->corners[3].y;
}

int State2_Zaghen::scaleQR(double side) {
	static double fact = (double)this->scale_factor * (double)this->qr_size_mm;
	return fact/side;
}

/** Calculate perspective rotation and distance of the QR. ---------------------------------------*/
void State2_Zaghen::calcPerspective_Distance(double side2, double side4) {

	int qr_pixel_size = average(side2, side4);
	int s2_dist = this->scaleQR(side2);
	int s4_dist = this->scaleQR(side4);
	this->qrStuff.temp_qr_info.distance = average(s2_dist, s4_dist);

	static double threshold = qr_pixel_size/double(THRESH); // progressive based on how far the QR code is (near -> increase).
	int s_LR_dist_delta = s2_dist - s4_dist;

	if (abs(s_LR_dist_delta) < threshold)
		s_LR_dist_delta = 0;

	this->qrStuff.temp_qr_info.perspective_rotation = getAngleLR(s_LR_dist_delta, this->qr_size_mm);
}

void State2_Zaghen::calcCenter_VerticalRot() {

	//int x_center = average(temp_qr_info.x0, temp_qr_info.x2);
	//int y_center = average(temp_qr_info.y0, temp_qr_info.y2);

	int dx = this->qrStuff.temp_qr_info.x1 - this->qrStuff.temp_qr_info.x0;
	int dy = this->qrStuff.temp_qr_info.y1 - this->qrStuff.temp_qr_info.y0;
	this->qrStuff.temp_qr_info.vertical_rotation = getAngleV(dy, dx) + PIDEG;
}

void State2_Zaghen::printQRInfo() {
	printf("*********************************************\n");
    printf("Perspective rotation\t\t%d\n", this->qrStuff.temp_qr_info.perspective_rotation);
    printf("Distance from camera\t\t%d\n", this->qrStuff.temp_qr_info.distance);
    printf("Payload\t\t%s\n ", this->qrStuff.temp_qr_info.qr_message);
    printf("*********************************************\n\n");
}

/** Extrapolate QR informations. -----------------------------------------------------------------*/
void State2_Zaghen::extrapolate_qr_informations() {

	copyCorners();
	calcCenter_VerticalRot();

	/* p0        side1        p1
       ▄▄▄▄▄▄▄ ▄▄▄▄▄ ▄▄▄▄▄▄▄
       █ ▄▄▄ █  █▄▀  █ ▄▄▄ █
    s  █ ███ █  ▀█▀▄ █ ███ █ s
    i  █▄▄▄▄▄█ ▄ ▄▀█ █▄▄▄▄▄█ i
    d  ▄▄ ▄▄ ▄ ▀██▀█ ▄     ▄ d
    e  ▀▀█▄▀█▄▄▄   ▀▄█ █▄█▀  e
    4  ▀ ███▀▄█▄▄█▀  ▄  ██▀█ 2
       ▄▄▄▄▄▄▄ ▀▀█▀█▄▄█▄▀ ▀█
       █ ▄▄▄ █ ▄ ▄██▀▄█▀█▄ █
       █ ███ █ ▀▄█▀▀ ▄ ▀▀█▄▄
       █▄▄▄▄▄█ █▄▀▄ ▀▀ ▀▄█▄▀
     p3        side3        p2    */

	double side2 = pitagora((double) (this->qrStuff.temp_qr_info.x1 - this->qrStuff.temp_qr_info.x2), (double)(this->qrStuff.temp_qr_info.y1 - this->qrStuff.temp_qr_info.y2));
	double side4 = pitagora((double) (this->qrStuff.temp_qr_info.x3 - this->qrStuff.temp_qr_info.x0), (double)(this->qrStuff.temp_qr_info.y3 - this->qrStuff.temp_qr_info.y0));

	calcPerspective_Distance(side2, side4);

	gettimeofday(&this->qrStuff.temp_qr_info.timestamp_recognition, NULL);
}

/** Copies payload from data to info structure. --------------------------------------------------*/
void State2_Zaghen::copyPayload() {

	int payload_len = this->qrStuff.data->payload_len;
    int payloadTruncated = 0;
    if(payload_len > MAXLENGTH-1){
      payload_len = MAXLENGTH-1;  // truncate long payloads
      payloadTruncated = 1;
    }
    this->qrStuff.temp_qr_info.payload_truncated = payloadTruncated;
    memcpy(this->qrStuff.temp_qr_info.qr_message, this->qrStuff.data->payload, payload_len+1 ); // copy the '\0' too.
    this->qrStuff.temp_qr_info.qr_message[MAXLENGTH] = '\0';
}

void State2_Zaghen::resetQR() {
	if(this->qrStuff.q)
		quirc_destroy(this->qrStuff.q);
	this->qrStuff.q = quirc_new();
	if(!this->qrStuff.q) {
		perror("Can't create quirc object");
		ThreadOnMutex::closeAllThreads();
	}
	this->qrStuff.temp_qr_info.distance = 0;
	this->qrStuff.temp_qr_info.message_length = 0;
	this->qrStuff.temp_qr_info.payload_truncated = 0;
	this->qrStuff.temp_qr_info.perspective_rotation = 0;
	this->qrStuff.temp_qr_info.vertical_rotation = 0;
	this->qrStuff.temp_qr_info.x0 = this->qrStuff.temp_qr_info.y0 = this->qrStuff.temp_qr_info.x1 = this->qrStuff.temp_qr_info.y1 = this->qrStuff.temp_qr_info.x2 = this->qrStuff.temp_qr_info.y2 = this->qrStuff.temp_qr_info.x3 = this->qrStuff.temp_qr_info.y3 = 0;
}


/** Processes QR code. ---------------------------------------------------------------------------*/
int State2_Zaghen::processQR() {

  quirc_end(this->qrStuff.q);

  int count = quirc_count(this->qrStuff.q);
  if(count == 0){ // no QR codes found.
    return 0;
  }

  struct quirc_code code2;
  struct quirc_data data2;
  quirc_decode_error_t err;

  quirc_extract(this->qrStuff.q, 0, &code2); // only recognize the first QR code found in the image
  err = quirc_decode(&code2, &data2);

  if(err == 0){
    while(pthread_mutex_lock(this->getWorldKB().getMutex()) != 0)
	{ ; }

    this->qrStuff.temp_qr_info.message_length = MAXLENGTH;
    extrapolate_qr_informations();
    copyPayload();
    printQRInfo(); // the only excerpt of a QR that should be printed in a release.

    while(pthread_mutex_unlock(this->getWorldKB().getMutex()) != 0)
    { ; }
  }
  return 0;
}
//---------------------------------------------------------------------------------------------------------------------------

/** Thread Behaviour call handler. ---------------------------------------------------------------*/
void* ExplorerFSM::call_threadBehaviour(void * arg){
	ExplorerFSM* temp = (ExplorerFSM*) arg;
	temp->threadBehaviour();
	return NULL;
}

/** Scanning function. ---------------------------------------------------------------------------*/
void* ExplorerFSM::threadBehaviour() {

	State* temp;

	while(1) {
		temp = currentState->executeState();
		if(temp)
			setCurrentState(temp);
		else {
			printf("HO FINITO\n");
			break;
		}
	}
	return NULL;
}

