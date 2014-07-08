#include "explorerFSM.h"

State::State(WorldKB* _worldKB)
{
	cout << "INIZIALIZZO STATO\n";
	this->worldKB = _worldKB;
}

State::~State(){;}

WorldKB* State::getWorldKB()
{
	return this->worldKB;
}

void State::setWorldKB(WorldKB* kb) 
{
	this->worldKB = kb;
}

void State::morgulservo_wrapper(float sleeptime)
{
	stringstream comando;
	comando << "morgulservo -- " << this->getWorldKB()->getCameraAngle();
	cout << "sto chiamando : " << comando.str() << endl;
	system(comando.str().c_str()); //magari si annullano le due sottochiamate
	sleep(sleeptime);
}

// --------------------- ---------------- ---------------- ---------------- ---------------- ----------------
State1_Init::State1_Init(WorldKB* _worldKB) : State(_worldKB)
{
	this->getWorldKB()->setCameraAngle(this->getWorldKB()->getpStartAngle());
	this->morgulservo_wrapper(this->getWorldKB()->getpStartSleep());
}

State1_Init::~State1_Init() { ; }

State* State1_Init::executeState(void)
{	
	//delete this;
	return new State2_QR(this->getWorldKB());
}

// --------------------- ---------------- ---------------- ---------------- ---------------- ----------------

State2_QR::State2_QR(WorldKB* _worldKB) : State(_worldKB)
{
	cout << "SONO LO STATO 2\n";

	this->mutex = PTHREAD_MUTEX_INITIALIZER;
	this->turnSearching = false;

	qrStuff.q = (quirc*)malloc(sizeof(quirc));
	QRInfos* temp = (QRInfos*)malloc(sizeof(QRInfos));
	qrStuff.qr_info = *temp;

	CvFileStorage* fs = cvOpenFileStorage(CALIB_FILE, 0, CV_STORAGE_READ);
	if (fs==NULL)
		{ perror("Error during calibration file loading.\n"); exit(1); }
	this->intrinsic_matrix = (CvMat*)cvReadByName(fs, 0, "camera_matrix");
	this->distortion_coeffs = (CvMat*)cvReadByName(fs, 0, "distortion_coefficients");
	this->scale_factor = cvReadIntByName(fs, 0, "scale_factor");
	this->qr_size_mm = cvReadIntByName(fs, 0, "qr_size_mm");

	resetQR();
	capture.open(this->getWorldKB()->getpCameraID());

	if (!capture.isOpened())
		{ perror("Error during capture opening.\n"); exit(1); }
	Mat framet;
    capture >> framet;
    this->frameCols = framet.cols;
}

State2_QR::~State2_QR()
{
	free(qrStuff.q);
	free(&(qrStuff.qr_info));
}

State* State2_QR::executeState()
{
	pthread_t moveCamera_thread;
	pthread_create(&moveCamera_thread, NULL, &State2_QR::moveCamera_wrapper, this);

	bool stopWhile = false;
	do {
		if(turnSearching) {
//			while(pthread_mutex_lock(&mutex)   != 0);
				stopWhile = this->searching();															// CRITICAL REGION
				//cout << "dentro while, angle = " << this->getWorldKB()->getCameraAngle() << endl;		// CRITICAL REGION
				turnSearching = false;																	// CRITICAL REGION
//			while(pthread_mutex_unlock(&mutex) != 0);
		}
		/* c'è una lieve ma presente SFASATURA tra chiamate a morgulservo e iterazioni di questo while. esaminare output per rendersene conto
		 * più specificamente, il thread va molto più veloce e questo while non riesce a stargli dietro, di fatto il cout precedente non viene
		 * stampato ad ogni step, ma ogni 6-8 steps.
		 * QUESTO È MOLTO GRAVE, PERCHÈ SE PER CASO DOVESSIMO AVERE QUALCHE ISTRUZIONE IN QUESTO CICLO, QUESTA NON AVVERREBBE A OGNI STEP !
		 */
	} while (stopWhile == false && this->getWorldKB()->isInRange() == true);

	if(this->qrStuff.q)
		quirc_destroy(this->qrStuff.q);

						//pthread_kill(moveCamera_thread, SIGTERM);
	//delete this;
	return new State3_Checking(this->getWorldKB());
}

bool State2_QR::searching()
{
	this->qrStuff.q = quirc_new();
	if(!this->qrStuff.q)
		{ perror("Can't create quirc object"); exit(1); }

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
	if (preProcessing())
		return true;
	return false; //handle
}

/** Processes QR code. ---------------------------------------------------------------------------*/
bool State2_QR::preProcessing()
{
	int min_payload=2;
	quirc_end(this->qrStuff.q);
	int count = quirc_count(this->qrStuff.q);
	if(!count)
		return false; // no QR codes found.
	if(count > 1)
		cout << "WARNING: FOUND >1 QR. CONSEGUENZE IMPREVEDIBILI SULL'ORDINAMENTO SPAZIALE" << endl;
	quirc_decode_error_t err;
	// only recognize the first QR code found in the image
	quirc_extract(this->qrStuff.q, 0, &(this->qrStuff.code));
	err = quirc_decode(&(this->qrStuff.code),&(this->qrStuff.data));
	if(!err) {
		int len_payload = copyPayload();
		char* label = this->qrStuff.qr_info.qr_message;
		copyCorners();
		if(len_payload>min_payload &&  isCentered()){
			bool isKnown;
			bool isRecognized;
			isKnown = this->getWorldKB()->isQRInKB(label, &isRecognized);
			if(isKnown && !isRecognized){
				cout << "Nuovo QR: \""<< label <<"\"" << endl;
				this->processing();
				return false;
			}else{
				cout << label << " NON in statica OR GIÀ in dinamica, in particolare: ";
				if(!isKnown)
					cout << "NON in statica -> non può esistere" << endl;
				if(isRecognized)
					cout << "IN dinamica -> è un QR già conosciuto" << endl;
				return false;
			}
		}
	}
	return false;
}

bool State2_QR::processing()
{
	double side2 = pitagora((double) (this->qrStuff.qr_info.x1 - this->qrStuff.qr_info.x2), (double)(this->qrStuff.qr_info.y1 - this->qrStuff.qr_info.y2));
	double side4 = pitagora((double) (this->qrStuff.qr_info.x3 - this->qrStuff.qr_info.x0), (double)(this->qrStuff.qr_info.y3 - this->qrStuff.qr_info.y0));
	calcPerspective_Distance(side2, side4);
	cout << "Push QR: message: " << this->qrStuff.qr_info.qr_message
			 << ", distance: " << this->qrStuff.qr_info.distance
			 << ", delta angle: " << this->getWorldKB()->getCameraAngle() << endl;
	this->getWorldKB()->pushQR(string(this->qrStuff.qr_info.qr_message),
								this->qrStuff.qr_info.distance,
								(double)this->getWorldKB()->getCameraAngle());
	return false;
}

void State2_QR::copyCorners()
{
	this->qrStuff.qr_info.x0 = this->qrStuff.code.corners[0].x;
	this->qrStuff.qr_info.y0 = this->qrStuff.code.corners[0].y;
	this->qrStuff.qr_info.x1 = this->qrStuff.code.corners[1].x;
	this->qrStuff.qr_info.y1 = this->qrStuff.code.corners[1].y;
	this->qrStuff.qr_info.x2 = this->qrStuff.code.corners[2].x;
	this->qrStuff.qr_info.y2 = this->qrStuff.code.corners[2].y;
	this->qrStuff.qr_info.x3 = this->qrStuff.code.corners[3].x;
	this->qrStuff.qr_info.y3 = this->qrStuff.code.corners[3].y;
}

int State2_QR::scaleQR(double side)
{
	static double fact = (double)this->scale_factor * (double)this->qr_size_mm;
	return fact/side;
}

/** Calculate perspective rotation and distance of the QR. ---------------------------------------*/
void State2_QR::calcPerspective_Distance(double side2, double side4)
{
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

bool State2_QR::isCentered()
{
	int x_center = average(qrStuff.qr_info.x0, qrStuff.qr_info.x2);
	printf("x_center %d\tframeCols %d\tcentering_tolerance %d\n", x_center, frameCols, this->getWorldKB()->getpCenterTolerance());
	if (abs( x_center - frameCols/2 ) < this->getWorldKB()->getpCenterTolerance() ){
		printf("\nQR rilevato e centrato\n");
		return true;
	}else{
		printf("\nQR rilevato ma non centrato\n");
		return false;
	}
}

/** Copies payload from data to info structure. --------------------------------------------------*/
int State2_QR::copyPayload()
{
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

void State2_QR::resetQR()
{
	this->qrStuff.qr_info.distance = 0;
	this->qrStuff.qr_info.message_length = -1;
	this->qrStuff.qr_info.payload_truncated = 0;
	this->qrStuff.qr_info.perspective_rotation = 0;
	this->qrStuff.qr_info.vertical_rotation = 0;
	this->qrStuff.qr_info.x0 = this->qrStuff.qr_info.y0 = this->qrStuff.qr_info.x1 = this->qrStuff.qr_info.y1 = this->qrStuff.qr_info.x2 = this->qrStuff.qr_info.y2 = this->qrStuff.qr_info.x3 = this->qrStuff.qr_info.y3 = 0;
}

// --------------------- ---------------- ---------------- ---------------- ---------------- ----------------

State3_Checking::State3_Checking(WorldKB* _worldKB) : State(_worldKB)
{
	//cout << "SONO LO STATO 3\n";
}

State3_Checking::~State3_Checking() { ; }

State* State3_Checking::executeState()
{
	this->getWorldKB()->printKB();
	if (this->getWorldKB()->getRecognizedQRs() < 2) {
		cout << "K contains {0|1} Landmarks and a triangulation CANNOT be performed. I'm sorry, I don't know where I am :(" << endl;
		return new State5_Error(this->getWorldKB());
	}
	cout << "K contains {2,...} Landmarks and a triangulation CAN be performed" << endl;
  //delete this;
  return new State4_Localizing(this->getWorldKB());
}

// --------------------- ---------------- ---------------- ---------------- ---------------- ----------------

void Triangle::triangulation()
{
	this->gamma_angle = lmB->getDeltaAngle() - lmA->getDeltaAngle();
	double AR = this->lmA->getDistance();
	double RB = this->lmB->getDistance();
	double dxAB = lmB->getX() - lmA->getX();
	double dyAB = lmB->getY() - lmA->getY();
	double AB = pitagora(dyAB, dxAB);					// radius of AB
	this->theta_angle = radToDeg(atan2(dyAB, dxAB));	// phase  of AB
	this->alpha_angle = radToDeg(asin( sin(degToRad(gamma_angle)) * (RB/AB)));
	this->phi_angle = theta_angle - alpha_angle + HALFPIDEG;
	double dxAR = AR * sin(degToRad(phi_angle));
	double dyAR = AR * cos(degToRad(phi_angle));
	//cout << "temp: AB = " << AB << ", dxAB = " << dxAB << ", dyAB = " << dyAB << ", dxAR = " << dxAR << ", dyAR = " << dyAR << endl;
	this->robot_coords.x = lmA->getX() + dxAR;
	this->robot_coords.y = lmA->getY() - dyAR; // OCCHIO AL MENO
}

Triangle::Triangle(Landmark* _lA, Landmark* _lB)
{
	this->lmA = _lA;
	this->lmB = _lB;
	this->alpha_angle = this->beta_angle = this->gamma_angle = this->phi_angle = this->theta_angle = 0;
	this->robot_coords.x = this->robot_coords.y = 0;
}

void Triangle::print()
{
	cout << "Printing Triangle" << endl;
	this->lmA->print();
	this->lmB->print();
	cout << "angles: alpha = " << this->alpha_angle << ", beta = " << this->beta_angle << ", gamma = " << this->gamma_angle << ", phi = " << this->phi_angle << ", theta = " << this->theta_angle << endl;
	cout << "Robot: X = " << this->robot_coords.x << ", Y = " << this->robot_coords.y << endl;
	cout << "~~~~~ ~~~~~ ~~~~~ ~~~~~ ~~~~~ ~~~~~ ~~~~~ ~~~~~ ~~~~~ " << endl;
}

void Triangle::printShort()
{
	cout << "Landmark A: "; this->lmA->getLabel();
	cout << "\tLandmark B: "; this->lmB->getLabel();
	cout << "\tRobot: X = " << this->robot_coords.x << ", Y = " << this->robot_coords.y << endl;
}

State4_Localizing::State4_Localizing(WorldKB* _worldKB) : State(_worldKB)
{//cout << "   State4_Localizing state\n";
}

State4_Localizing::~State4_Localizing() { ; }

Triangle State4_Localizing::basicLocalization()
{
	Landmark* lA = this->getWorldKB()->getLandmark(0);
	Landmark* lB = this->getWorldKB()->getLandmark(1);
	Triangle tr = Triangle(lA, lB);
	tr.triangulation();
	tr.print();
	return tr;
}

void State4_Localizing::testLocalization()
{
	this->getWorldKB()->triangleTest();
	Landmark* lA = this->getWorldKB()->getLandmark(0);
	Landmark* lB = this->getWorldKB()->getLandmark(1);
	Triangle tr1 = Triangle(lA, lB);
	tr1.triangulation();
	tr1.print();
	Landmark* lC = this->getWorldKB()->getLandmark(2);
	Landmark* lD = this->getWorldKB()->getLandmark(3);
	Triangle tr2 = Triangle(lC, lD);
	tr2.triangulation();
	tr2.print();
}

void State4_Localizing::printAllRobotCoords()
{
	cout << "WHERE AM I ? Printing all possible suggestions. " << endl;
	vector<Triangle>::iterator it = this->triangles.begin();
	while(it != this->triangles.end()) {
		(*it).printShort();
		it++;
	}
	cout << "~~~~~ ~~~~~ ~~~~~ ~~~~~ ~~~~~ ~~~~~ ~~~~~ ~~~~~ ~~~~~ " << endl;
}

State* State4_Localizing::executeState()
{
	cout << "State5_Localizing: calling BASIC LOCALIZATION" << endl;

	Triangle tr = this->basicLocalization();
	this->triangles.push_back(tr);
	//this->testLocalization();
	this->printAllRobotCoords();

	//delete this;
	return NULL;
}

// --------------------- ---------------- ---------------- ---------------- ---------------- ----------------

State5_Error::State5_Error(WorldKB* _worldKB) : State(_worldKB) {;}

State5_Error::~State5_Error() {;}

State* State5_Error::executeState()
{
  //delete this;
  return NULL;
}

// --------------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ----------------
// --------------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ----------------
// --------------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ----------------
// --------------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ---------------- ----------------

ExplorerFSM::ExplorerFSM()
{
	// DO NOT CONSTRUCT ANYTHING HERE ! it's called by default, somehow
	//this->worldKB = *(new WorldKB);
	//this->worldKB = WorldKB;
	this->currentState = new State1_Init(&(this->worldKB));
}

ExplorerFSM::~ExplorerFSM() {;}

void ExplorerFSM::setCurrentState(State *s)
{
	currentState = s;
}

void* ExplorerFSM::runFSM()
{
	State* temp;
	while(true) {
		temp = currentState->executeState();
		if(temp!=NULL)
			setCurrentState(temp);
		else {
			cout << "Whereami exiting successfully." << endl;
			break;
		}
	}
	return NULL;
}
