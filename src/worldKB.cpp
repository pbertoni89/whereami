#include "worldKB.h"

WorldKB::WorldKB() {
	this->mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_unlock(&(this->mutex));
	this->qr_found = 0;

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
