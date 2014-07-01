#include "worldKB.h"

Landmark::Landmark(QRInfos _qr_info) {
	this->qr_info = _qr_info;
	this->phi_angle = 0;
}
Landmark::~Landmark() { ; }

QRInfos Landmark::getQRInfos() {
	return this->qr_info;
}
void Landmark::setPhiAngle(int _phi_angle) {
	this->phi_angle = _phi_angle;
}
int Landmark::getPhiAngle() {
	return this->phi_angle;
}

// ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~

WorldKB::WorldKB() {
	this->camera_angle = CAMERA_INIT_ANGLE;
	this->createStaticKB();
}

WorldKB::~WorldKB() { ; }

void WorldKB::createStaticKB() {
	//for(int i=0; i<MAXQR; i++) {
	qrTable.qr_coords[0].x = 30;
	qrTable.qr_coords[0].y = 20;
	qrTable.qr_label[0] = "landmark0";
	
	qrTable.qr_coords[1].x = 50; 
	qrTable.qr_coords[1].y = 60;
	qrTable.qr_label[1] = "prova111";
	//qrTable.qr_label[1] = "prova11fffff1";

	//}
}

int WorldKB::get_qr_found() {
	return this->landmarks.size();
}

void WorldKB::pushQR(QRInfos* qr_info) {
	Landmark temp = Landmark(*qr_info);
	//printf("qr info just copied to be passed. its payload is %s\n", temp.getQRInfos().qr_message);
	this->landmarks.push_back(temp);
	//printf("qr info just passed. its payload is %s\n", this->landmarks.back().getQRInfos().qr_message);
}

int WorldKB::getCameraAngle() {
	return this->camera_angle;
}

void WorldKB::setCameraAngle(int _camera_angle) {
	this->camera_angle = _camera_angle;
}

void WorldKB::incrementCameraAngle()  {
	this->setCameraAngle(this->getCameraAngle() + CAMERA_STEP_ANGLE);
}

bool WorldKB::isQRInDynamicKB(string label) {
	string tmp;
	vector<Landmark>::iterator it = this->landmarks.begin();
	while(it != this->landmarks.end()) {
		tmp = string((*it).getQRInfos().qr_message);
		if(label.compare(tmp)==0)
			return true;
		it++;
	}
	return false;
}

bool WorldKB::isQRInStaticKB(string label) {
	string tmp;
	int i = 0;
	while(i<MAXQR && this->qrTable.qr_label[i].size()>0) {
		if(label.compare(this->qrTable.qr_label[i])==0)
			return true;
		i++;
	}
	return false;
}
