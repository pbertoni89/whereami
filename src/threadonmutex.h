#ifndef PATTHREAD_H_
#define PATTHREAD_H_

#include "worldKB.h"

#include <pthread.h>
#ifndef __APPLE__
	#include <wait.h>
#endif

#define NTHREADS 2

class ThreadOnMutex {

private:
	/** The list of threads which have to be closed at the same time if any of them fails. */
	static pthread_t thread_list[NTHREADS];
	/** The index that will assume each instance of ThreadOnMutex in array thread_list. */
	int thread_index;
	/** A REFERENCE on the Knowledge Base of the World, which will be used either for writing on or reading from.
	 *  Remember that a reference to the mutex semaphore of the worldKB is obtainable from the same KB object. */
	WorldKB& worldKB;

public:
	ThreadOnMutex(WorldKB& _worldKB);
	~ThreadOnMutex();
	/**Getter for own pthread_t. */
	pthread_t* getOwnThread();
	/**Getter for a pthread_t in thread_list. */
	static pthread_t* getThread(int idx);
	 /** Closes all thread in the list. */
	static void closeAllThreads();
	/** Getter for ThreadIndex. */
	int getThreadIndex();
	/** Setter for ThreadIndex. */
	void setThreadIndex(int _thread_index);
	/** Getter for worldKB. */
	WorldKB& getWorldKB();
	/** TODOC */
	virtual void* threadBehaviour() = 0;
	/** TODOC */
	static void* call_threadBehaviour(void * arg); //it would be static later to be passed to pthread; so it cannot be virtual
};
#endif
