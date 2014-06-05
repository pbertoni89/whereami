#include "threadonmutex.h"

pthread_t ThreadOnMutex::thread_list[NTHREADS];

ThreadOnMutex::ThreadOnMutex(WorldKB& _worldKB) : worldKB(_worldKB) {
	;
}

ThreadOnMutex::~ThreadOnMutex() {
	;
}

WorldKB& ThreadOnMutex::getWorldKB() {
	return this->worldKB;
}

int ThreadOnMutex::getThreadIndex() {
	return this->thread_index;
}

void ThreadOnMutex::setThreadIndex(int _thread_index) {
	this->thread_index = _thread_index;
}

pthread_t* ThreadOnMutex::getOwnThread() {
	return &ThreadOnMutex::thread_list[this->thread_index];
}

// se possibile togliamo lo "static" alla dichiarazione!!!!!!
pthread_t* ThreadOnMutex::getThread(int idx) {
	return &(ThreadOnMutex::thread_list[idx]);
}

void ThreadOnMutex::closeAllThreads() {
	for (int i=0; i<NTHREADS; i++)
		if( pthread_kill(*(ThreadOnMutex::getThread(i)), SIGTERM) )
			std::cout << "Error while closing all threads. You should handle this!!!!\n";
}

void* call_threadBehaviour(void * arg) {
	ThreadOnMutex* temp = (ThreadOnMutex*) arg;
	temp->threadBehaviour();
	return NULL;
}
