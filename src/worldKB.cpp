#include "worldKB.h"

WorldKB::WorldKB() {
	this->qr_found = 0;
	this->camera_angle = CAMERA_INIT_ANGLE;
	this->createStaticQRTable();
}

void WorldKB::createStaticQRTable() {
	//for(int i=0; i<MAXQR; i++) {
	qrTable.qr_coords[0].x = 30;
	qrTable.qr_coords[0].y = 20;
	qrTable.qr_label[0] = "landmark0";
	
	qrTable.qr_coords[1].x = 50; 
	qrTable.qr_coords[1].y = 60;
	qrTable.qr_label[1] = "landmark1";
	//}
}

WorldKB::~WorldKB() { ; }

int WorldKB::get_qr_found() {
	return this->qr_found;
}

void WorldKB::pushQR(QRInfos* qr_info) {
	// importanstissimo
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

bool WorldKB::isQRInKB(string label) {
	for(int i=0; i<this->qr_found; i++)
		//if(compare(label, qrTable.qr_label[i])==0)
			return true;
	return false;
}
