/** Server is where the main program is. It handles the entire exploration FSM, launching it,
 * and takes care of the communication with client and/or stdout. 							*/

#include "server.h"

/** Signal child handler. DON'T MOVE FROM HERE -------------------------------------------------*/
void sigchld_handler(int s) {
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

Server::Server(WorldKB& _worldKB) : ThreadOnMutex(_worldKB) {

	setThreadIndex(SERVERTHREADINDEX);
	int ret = pthread_create(getOwnThread(), NULL, &Server::call_threadBehaviour, NULL);
	if(ret != 0) {
		printf("Error creating the server thread. [%s]\n", strerror(ret));
		ThreadOnMutex::closeAllThreads();
		exit(1);
	}
}

Server::~Server() {
	;
}

/** Get socket address, IPv4 xor IPv6. -----------------------------------------------------------*/
void* Server::get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/** Thread Behaviour call handler. ---------------------------------------------------------------*/
void* Server::call_threadBehaviour(void * arg){
	Server* temp = (Server*) arg;
	temp->threadBehaviour();
	return NULL;
}

/** Server function. -----------------------------------------------------------------------------*/
void* Server::threadBehaviour() {
  printf("Server thread started.\n");
  
  int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size;
  struct sigaction sa;
  int yes=1;
  char s[INET6_ADDRSTRLEN];
  int rv;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP

  if((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0){
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    Server::closeAllThreads();
  }

  // loop through all the results and bind to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
      perror("server: socket");
      continue;
    }

    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
      perror("setsockopt");
      Server::closeAllThreads();
    }

    if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
      close(sockfd);
      perror("server: bind");
      continue;
    }
    break;
  }

  if(p == NULL){
    fprintf(stderr, "server: failed to bind\n");
    Server::closeAllThreads();
  }

  freeaddrinfo(servinfo); // all done with this structure

  if(listen(sockfd, BACKLOG) == -1){
    perror("listen");
    ThreadOnMutex::closeAllThreads();
  }

  sa.sa_handler = sigchld_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if(sigaction(SIGCHLD, &sa, NULL) == -1){
    perror("sigaction");
    Server::closeAllThreads();
  }

  printf("server: waiting for connections...\n");

  while(1){  // main accept() loop
    sin_size = sizeof their_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1) {
      perror("accept");
      continue;
    }

    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
    printf("server: got connection from %s\n", s);

    while(pthread_mutex_lock(this->getWorldKB().getMutex()) != 0);
    gettimeofday(&(this->getWorldKB().fooQR.timestamp_current), NULL);
    
    if (send(new_fd, &(this->getWorldKB().fooQR), sizeof(QRInfos), 0) == -1){
      while(pthread_mutex_unlock(this->getWorldKB().getMutex()) != 0);
      perror("send");
    }
    while(pthread_mutex_unlock(this->getWorldKB().getMutex()) != 0);
    close(new_fd);
  }
  return NULL;
}
