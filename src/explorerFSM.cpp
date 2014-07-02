#include "explorerFSM.h"
State::State(WorldKB* _worldKB) //: worldKB(_worldKB)
{
	this->worldKB = _worldKB;
}

State::~State(){
	//cout << "   Delete state\n";
}

WorldKB* State::getWorldKB()
{
	return this->worldKB;
}
void State::setWorldKB(WorldKB* kb) 
{
	this->worldKB = kb;
}
// --------------------- ---------------- ---------------- ---------------- ---------------- ----------------
State1_Init::State1_Init(WorldKB* _worldKB) : State(_worldKB) {
	WorldKB* temp = new WorldKB; //controllare che funzioni correttamente; a esempio che sovrascriva le vecchie strutture
	printf("allocated worldkb pointer at %p\n", temp);
	this->setWorldKB(temp);

	cout << "Metto la telecamera a -90\n";
	system("morgulservo -- -90");
	this->getWorldKB()->setCameraAngle(-90);
}
State1_Init::~State1_Init() { ; }

State* State1_Init::executeState(void)
{	
	delete this;
	return new State2_QR(this->getWorldKB());
}

// --------------------- ---------------- ---------------- ---------------- ---------------- ----------------

State2_QR::State2_QR(WorldKB* _worldKB) : State(_worldKB)
{
	camera_id = 0;
	int camera_angle = this->getWorldKB()->getCameraAngle();

	qrStuff.q = (quirc*)malloc(sizeof(quirc));
	QRInfos* temp = (QRInfos*)malloc(sizeof(QRInfos));
	qrStuff.qr_info = *temp;

	//const char* filename = "/home/gio/Scrivania/progettoQR/whereami/out_camera_data.yml"; /**TODO OCCHIO IN RELEASE */
	const char* filename = "out_camera_data.yml"; /**TODO OCCHIO IN RELEASE */
	CvFileStorage* fs = cvOpenFileStorage(filename, 0, CV_STORAGE_READ);
	if (fs==NULL) {
		printf("Error during calibration file loading.\n");
		exit(1);
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
	Mat framet;
    capture >> framet;
    this->frameCols = framet.cols;
}

State2_QR::~State2_QR() { ; }

State* State2_QR::executeState()
{
	while ( this->searching() == false && this->getWorldKB()->getCameraAngle() < CAMERA_END_ANGLE )
	{ // QUA VA LA CHIAMATA DI SISTEMA PER GIRARE LA TELECAMERA DI STEP GRADI; 

		this->getWorldKB()->incrementCameraAngle();
    	//printf("%d\n", this->getWorldKB()->getCameraAngle());
		stringstream comando;
		comando << "morgulservo -- " << this->getWorldKB()->getCameraAngle();
		string support = comando.str();
		cout << "sto chiamando : " << support << endl;
		system(support.c_str());
		sleep(0.5);
	}

	 // cout << "\t Distanza QR dalla camera: " << this->qrStuff.qr_info.distance << endl;

	if(this->qrStuff.q)
		quirc_destroy(this->qrStuff.q);
	delete this;
	return new State3_StatusChecking(this->getWorldKB());
}

bool State2_QR::searching()
{
	this->qrStuff.q = quirc_new();
	if(!this->qrStuff.q) {
		perror("Can't create quirc object");
		exit(1);
}

	Mat frame0, frame, frame_undistort, frame_BW;
	if( !this->capture.isOpened() )
		{ perror("Error opening capture.\n"); exit(1); }

	capture >> frame0;
	frame0.copyTo(frame);

	if (!frame.data)
		{ perror("Error loading the frame.\n"); exit(1); }

	undistort(frame, frame_undistort, intrinsic_matrix, distortion_coeffs);
	cvtColor(frame_undistort, frame_BW, CV_BGR2GRAY);
	cv_to_quirc(this->qrStuff.q, frame_BW);
	if (preProcessing() == true)
		return true;
	return false; //handle
}
bool State2_QR::processing() {
	double side2 = pitagora((double) (this->qrStuff.qr_info.x1 - this->qrStuff.qr_info.x2), (double)(this->qrStuff.qr_info.y1 - this->qrStuff.qr_info.y2));
	double side4 = pitagora((double) (this->qrStuff.qr_info.x3 - this->qrStuff.qr_info.x0), (double)(this->qrStuff.qr_info.y3 - this->qrStuff.qr_info.y0));
	calcPerspective_Distance(side2, side4);
	cout<<"\nPush del QR con messaggio: "<<this->qrStuff.qr_info.qr_message<<" distanza: "<<this->qrStuff.qr_info.distance<<"\n";
	this->getWorldKB()->pushQR(& this->qrStuff.qr_info);
	cout << this->getWorldKB()->get_qr_found();
	return false;
}

void State2_QR::copyCorners() {
	this->qrStuff.qr_info.x0 = this->qrStuff.code.corners[0].x;
	this->qrStuff.qr_info.y0 = this->qrStuff.code.corners[0].y;
	this->qrStuff.qr_info.x1 = this->qrStuff.code.corners[1].x;
	this->qrStuff.qr_info.y1 = this->qrStuff.code.corners[1].y;
	this->qrStuff.qr_info.x2 = this->qrStuff.code.corners[2].x;
	this->qrStuff.qr_info.y2 = this->qrStuff.code.corners[2].y;
	this->qrStuff.qr_info.x3 = this->qrStuff.code.corners[3].x;
	this->qrStuff.qr_info.y3 = this->qrStuff.code.corners[3].y;
}

int State2_QR::scaleQR(double side) {
	static double fact = (double)this->scale_factor * (double)this->qr_size_mm;
	return fact/side;
}

/** Calculate perspective rotation and distance of the QR. ---------------------------------------*/
void State2_QR::calcPerspective_Distance(double side2, double side4) {
	int qr_pixel_size = average(side2, side4);
	int s2_dist = this->scaleQR(side2);
	int s4_dist = this->scaleQR(side4);
	this->qrStuff.qr_info.distance = average(s2_dist, s4_dist);

	static double threshold = qr_pixel_size/double(THRESH); // progressive based on how far the QR code is (near -> increase).
	int s_LR_dist_delta = s2_dist - s4_dist;

	if (abs(s_LR_dist_delta) < threshold)
		s_LR_dist_delta = 0;

	this->qrStuff.qr_info.perspective_rotation = getAngleLR(s_LR_dist_delta, this->qr_size_mm);
}

bool State2_QR::isCentered() {
	centering_tolerance=40;			//LA TOLLERANZA PER LE PROVE IN TESISTI VA BENE A 40 MA PER IL LABO ANDRA' MESSA A 3. :)
	int x_center = average(qrStuff.qr_info.x0, qrStuff.qr_info.x2);
	//printf("x_center %d\tframeCols %d\tcentering_tolerance %d\n", x_center, frameCols, centering_tolerance);
	if (abs( x_center - frameCols/2 ) < centering_tolerance ){
		printf("\nQR rilevato e centrato\n");
		return true;
	}else{
		printf("\nQR rilevato ma non centrato\n");
		return false;
	}
}

void State2_QR::printQRInfo() {
	printf("*********************************************\n");
    printf("Perspective rotation\t\t%d\n", this->qrStuff.qr_info.perspective_rotation);
    printf("Distance from camera\t\t%d\n", this->qrStuff.qr_info.distance);
    printf("Payload\t\t%s\n ", this->qrStuff.qr_info.qr_message);
    printf("*********************************************\n\n");
}

/** Copies payload from data to info structure. --------------------------------------------------*/
int State2_QR::copyPayload() {
	this->qrStuff.qr_info.message_length = MAXLENGTH;
	int payload_len = this->qrStuff.data.payload_len;
    int payloadTruncated = 0;
    if(payload_len > MAXLENGTH-1){
      payload_len = MAXLENGTH-1;  // truncate long payloads
      payloadTruncated = 1;
    }
    this->qrStuff.qr_info.payload_truncated = payloadTruncated;
    memcpy(this->qrStuff.qr_info.qr_message, this->qrStuff.data.payload, payload_len+1 ); // copy the '\0' too.
    this->qrStuff.qr_info.qr_message[MAXLENGTH] = '\0';
    return payload_len;
}

void State2_QR::resetQR() {
	this->qrStuff.qr_info.distance = 0;
	this->qrStuff.qr_info.message_length = -1;
	this->qrStuff.qr_info.payload_truncated = 0;
	this->qrStuff.qr_info.perspective_rotation = 0;
	this->qrStuff.qr_info.vertical_rotation = 0;
	this->qrStuff.qr_info.x0 = this->qrStuff.qr_info.y0 = this->qrStuff.qr_info.x1 = this->qrStuff.qr_info.y1 = this->qrStuff.qr_info.x2 = this->qrStuff.qr_info.y2 = this->qrStuff.qr_info.x3 = this->qrStuff.qr_info.y3 = 0;
}

/** Processes QR code. ---------------------------------------------------------------------------*/
bool State2_QR::preProcessing() {
	int min_payload=2;
	quirc_end(this->qrStuff.q);
  int count = quirc_count(this->qrStuff.q);
  if(count == 0){ // no QR codes found.
    return false;
  }
  if(count > 1) {
	cout << "WARNING: FOUND >1 QR. CONSEGUENZE IMPREVEDIBILI SULL'ORDINAMENTO SPAZIALE" << endl;
	}
  quirc_decode_error_t err;

  quirc_extract(this->qrStuff.q, 0, &(this->qrStuff.code)); // only recognize the first QR code found in the image
  err = quirc_decode(&(this->qrStuff.code),&(this->qrStuff.data));
   if(err==0) {
	   int len_payload=0;
	   len_payload=copyPayload();
	    char* label = this->qrStuff.qr_info.qr_message;
		copyCorners();
		if(len_payload>min_payload &&  isCentered()){
			if(this->getWorldKB()->isQRInStaticKB(label) && !this->getWorldKB()->isQRInDynamicKB(label)){
					cout << "Nuovo QR!\n";
					cout <<"Payload; " << label;
					this->processing();
					return false;
				}else{
						cout << label << "  Non e' nella statica o e' già presente nella dinamica, in particolare: ";
						if(!this->getWorldKB()->isQRInStaticKB(label) )
							cout << "Non è nella statica quindi non può esistere";
						if(this->getWorldKB()->isQRInDynamicKB(label))
							cout << "è nella dinamica quindi è un QR già conosiuto";
						return false; //ignora il qr
			  }
		}
			  //cout << "\t" << label << " is NOT known in Static KB: false" << endl;
		}
  return false;
}

// --------------------- ---------------- ---------------- ---------------- ---------------- ----------------

State3_StatusChecking::State3_StatusChecking(WorldKB* _worldKB) : State(_worldKB){
//cout << "   State4_StatusChecking state\n";
}

State3_StatusChecking::~State3_StatusChecking() { ; }

State* State3_StatusChecking::executeState()
{//cout << "Sono nello stato 3\n";
  delete this;
  return new State4_Localizing(this->getWorldKB());
}

// --------------------- ---------------- ---------------- ---------------- ---------------- ----------------

State4_Localizing::State4_Localizing(WorldKB* _worldKB) : State(_worldKB)
{//cout << "   State5_Localizing state\n";
}

State4_Localizing::~State4_Localizing() { ; }

State* State4_Localizing::executeState()
{
  delete this;
  return NULL;
}

// --------------------- ---------------- ---------------- ---------------- ---------------- ----------------

State5_Error::State5_Error(WorldKB* _worldKB) : State(_worldKB)
{//cout << "   State5_Localizing state\n";
}

State5_Error::~State5_Error() { ; }

State* State5_Error::executeState()
{
  delete this;
  return NULL;
}

// --------------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ----------------
// --------------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ----------------
// --------------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ----------------
// --------------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ----------------

ExplorerFSM::ExplorerFSM(int _camera_id) 
{
	this->camera_id = _camera_id;
	WorldKB* temp = new WorldKB;
	this->worldKB = *temp;
	this->currentState = new State1_Init(&(this->worldKB));
}

ExplorerFSM::~ExplorerFSM() { ; }

void ExplorerFSM::setCurrentState(State *s)
{
	currentState = s;
}

void* ExplorerFSM::runFSM()
{
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






/*

		if(camera_angle==85){
			camera_angle=-85;
		}
		camera_angle =camera_angle+1;




 */
