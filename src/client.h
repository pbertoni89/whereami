/*
** client -- a stream socket client, modified code from http://beej.us/guide/bgnet/examples/client.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "message.h"

#define PORT "9930" // the port client will be connecting to 
#define MAXLENGTH 256
#define MAXDATASIZE 512 // max number of bytes we can get at once 

void *get_in_addr(struct sockaddr *sa);
