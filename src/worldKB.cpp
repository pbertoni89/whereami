#include "worldKB.h"

WorldKB::WorldKB() {
	this->mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_unlock(&(this->mutex));
}

WorldKB::~WorldKB() {
	
}

void saveLandmark(Landmark lm, bool isA, int triangle_index) {
	;
}

pthread_mutex_t* WorldKB::getMutex() {
	return &(this->mutex);
}

