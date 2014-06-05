#ifndef SERVER_H_
#define SERVER_H_

#include "threadonmutex.h"

#include <unistd.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BACKLOG 10  /** how many pending connections queue will hold */
#define SERVERTHREADINDEX 0

class Server : public ThreadOnMutex {

public:
	Server(WorldKB& _worldKB);
	~Server();
	virtual void* threadBehaviour();
	static void* call_threadBehaviour(void * arg);

private:
	//static void sigchld_handler(int s);
	void* get_in_addr(struct sockaddr *sa);
};
#endif
