#include "worldKB.h"

WorldKB::WorldKB() {
	this->mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_unlock(&(this->mutex));
	this->qr_found = 0;
	this->camera_angle = CAMERA_INIT_ANGLE;
}

WorldKB::~WorldKB() {
	
}

pthread_mutex_t* WorldKB::getMutex() {
	return &(this->mutex);
}

int WorldKB::get_qr_found() {
	return this->qr_found;
}

void WorldKB::pushQR(QRInfos* qr_info) {

}

int WorldKB::getCameraAngle() {
	return this->camera_angle;
}
