#include "explorer.h"

Explorer::Explorer(WorldKB& _worldKB, int _camera_id) : ThreadOnMutex(_worldKB) {
	
	setThreadIndex(EXPLORERTHREADINDEX);
	camera_id = _camera_id;
	this->camera_angle = CAMERA_INIT_ANGLE;
	temp_qr_info.message_length = -1;
	
	const char* filename = "out_camera_data.yml";
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
	
	int ret = pthread_create(getOwnThread(), NULL, &Explorer::call_threadBehaviour, NULL);
	if(ret != 0) {
		printf("Error creating the scanner thread. [%s]\n", strerror(ret));
		ThreadOnMutex::closeAllThreads();
		exit(1);
	}
}

Explorer::~Explorer() {
	;
}

void Explorer::copyCorners() {
	
	temp_qr_info.x0 = code->corners[0].x;
	temp_qr_info.y0 = code->corners[0].y;
	temp_qr_info.x1 = code->corners[1].x;
	temp_qr_info.y1 = code->corners[1].y;
	temp_qr_info.x2 = code->corners[2].x;
	temp_qr_info.y2 = code->corners[2].y;
	temp_qr_info.x3 = code->corners[3].x;
	temp_qr_info.y3 = code->corners[3].y;
}

int Explorer::scaleQR(double side) {
	static double fact = (double)this->scale_factor * (double)this->qr_size_mm;
	return fact/side;
}

/** Calculate perspective rotation and distance of the QR. ---------------------------------------*/
void Explorer::calcPerspective_Distance(double side2, double side4) {

	int qr_pixel_size = average(side2, side4);
	int s2_dist = this->scaleQR(side2);
	int s4_dist = this->scaleQR(side4);
	temp_qr_info.distance = average(s2_dist, s4_dist);

	static double threshold = qr_pixel_size/double(THRESH); // progressive based on how far the QR code is (near -> increase).
	int s_LR_dist_delta = s2_dist - s4_dist;
	
	if (abs(s_LR_dist_delta) < threshold)
		s_LR_dist_delta = 0;
		
	temp_qr_info.perspective_rotation = getAngleLR(s_LR_dist_delta, this->qr_size_mm);
}

void Explorer::calcCenter_VerticalRot() {
	
	int x_center = average(temp_qr_info.x0, temp_qr_info.x2);
	int y_center = average(temp_qr_info.y0, temp_qr_info.y2);

	int dx = temp_qr_info.x1 - temp_qr_info.x0;
	int dy = temp_qr_info.y1 - temp_qr_info.y0;
	temp_qr_info.vertical_rotation = getAngleV(dy, dx) + PIDEG;
}

void Explorer::printQRInfo() {
	printf("*********************************************\n");
    printf("Perspective rotation\t\t%d\n", temp_qr_info.perspective_rotation);
    printf("Distance from camera\t\t%d\n", temp_qr_info.distance);
    printf("Payload\t\t%s\n ", temp_qr_info.qr_message);
    printf("*********************************************\n\n");
}

/** Extrapolate QR informations. -----------------------------------------------------------------*/
void Explorer::extrapolate_qr_informations() {

	Explorer::copyCorners();
	Explorer::calcCenter_VerticalRot();
	
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
     
	double side2 = pitagora((double) (temp_qr_info.x1 - temp_qr_info.x2), (double)(temp_qr_info.y1 - temp_qr_info.y2));
	double side4 = pitagora((double) (temp_qr_info.x3 - temp_qr_info.x0), (double)(temp_qr_info.y3 - temp_qr_info.y0));
  
	calcPerspective_Distance(side2, side4);
	
	gettimeofday(&temp_qr_info.timestamp_recognition, NULL);
}

/** Copies payload from data to info structure. --------------------------------------------------*/
void Explorer::copyPayload() {

	int payload_len = data->payload_len;
    int payloadTruncated = 0;
    if(payload_len > MAXLENGTH-1){
      payload_len = MAXLENGTH-1;  // truncate long payloads
      payloadTruncated = 1;
    }
    temp_qr_info.payload_truncated = payloadTruncated;
    memcpy(temp_qr_info.qr_message, data->payload, payload_len+1 ); // copy the '\0' too.
    temp_qr_info.qr_message[MAXLENGTH] = '\0';
}

/** Processes QR code. ---------------------------------------------------------------------------*/
int Explorer::processQR() {
  
  quirc_end(q);
  
  int count = quirc_count(q);
  if(count == 0){ // no QR codes found.
    return 0;
  }

  struct quirc_code code2;
  struct quirc_data data2;
  quirc_decode_error_t err;

  quirc_extract(q, 0, &code2); // only recognize the first QR code found in the image
  err = quirc_decode(&code2, &data2);
  
  if(err == 0){
    while(pthread_mutex_lock(this->getWorldKB().getMutex()) != 0)
	{ ; }
	
    temp_qr_info.message_length = MAXLENGTH;
    extrapolate_qr_informations();
    copyPayload();
    printQRInfo(); // the only excerpt of a QR that should be printed in a release.
    
    while(pthread_mutex_unlock(this->getWorldKB().getMutex()) != 0)
    { ; }
  }
  return 0;
}

/** Thread Behaviour call handler. ---------------------------------------------------------------*/
void* Explorer::call_threadBehaviour(void * arg){
	Explorer* temp = (Explorer*) arg;
	temp->threadBehaviour();
	return NULL;
}

/** Scanning function. ---------------------------------------------------------------------------*/
void* Explorer::threadBehaviour() {
	
	printf("Scanning thread started.\n");

	Mat frame, frame_undistort, frame_BW;
	while(1) {
        
        if( !capture.isOpened() )
			{ perror("Error opening capture.\n"); ThreadOnMutex::closeAllThreads(); }

		Mat frame0;
        capture >> frame0;
        frame0.copyTo(frame);

		if (!frame.data) 
			{ perror("Error loading the frame.\n"); ThreadOnMutex::closeAllThreads(); }

		undistort(frame, frame_undistort, intrinsic_matrix, distortion_coeffs);
		cvtColor(frame_undistort, frame_BW, CV_BGR2GRAY);
		
		cv_to_quirc(q, frame_BW);
		processQR();
		resetQR();
		
		int key = 0xff & waitKey(capture.isOpened() ? 50 : 500);
        if( (key & 255) == 27 )
           exit(0);
	}
}

int Explorer::getCameraAngle() {
	return this->camera_angle;
}

void Explorer::resetQR() {
	if(q)
		quirc_destroy(q);
	q = quirc_new();
	if(!q) {
		perror("Can't create quirc object");
		ThreadOnMutex::closeAllThreads();
	}
	temp_qr_info.distance = 0;
	temp_qr_info.message_length = 0;
	temp_qr_info.payload_truncated = 0;
	temp_qr_info.perspective_rotation = 0;
	temp_qr_info.vertical_rotation = 0;
	temp_qr_info.x0 = temp_qr_info.y0 = temp_qr_info.x1 = temp_qr_info.y1 = temp_qr_info.x2 = temp_qr_info.y2 = temp_qr_info.x3 = temp_qr_info.y3 = 0;
}
