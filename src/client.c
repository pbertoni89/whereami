/*
** client.c -- a stream socket client demo
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


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa){
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[]){
  
	QRInfos qr_info;
	int sockfd, numbytes, rv;
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	
	char s[INET6_ADDRSTRLEN];

	if (argc != 2) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client:onnect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	if ((numbytes = recv(sockfd, &qr_info, sizeof(QRInfos), 0)) == -1) {
	    perror("recv");
	    exit(1);
	}

	double center = sqrt(pow((double)qr_info.x3-qr_info.x1,2)+pow((double)qr_info.y3-qr_info.y1,2));

	printf("Received data from server...\n\n");
	
	printf("*** Message proprieties *** \n");
	printf("Length: %d, Payload Truncated: %s\n", qr_info.message_length, (qr_info.payload_truncated == 0? "no" : "yes"));
	printf("*** end Message proprieties ***\n\n");
	
	printf("*** QR payload *** \n");
	printf("%s\n", qr_info.qr_message);
	printf("*** end QR payload ***\n\n");
	
	printf("*** QR proprieties ***\n");
	printf("Points:\n (%d,%d), (%d,%d)\n (%d,%d), (%d,%d)\n ",qr_info.x0,qr_info.y0,qr_info.x1,qr_info.y1,qr_info.x2,qr_info.y2,qr_info.x3,qr_info.y3);
	printf("Distance: %fmm\nPerspective Rotation: %f\nVertical Rotation: %f\nQR Center:%f\n",qr_info.distance, 			qr_info.perspective_rotation, qr_info.vertical_rotation, center);
	printf("*** end QR proprieties ***\n");
	
	printf("*** Timestamps ***\n");
	printf("Detected %ld time ago\n", qr_info.timestamp_recognition.tv_sec-qr_info.timestamp_current.tv_sec);
	printf("*** end Timestamps ***\n");

	close(sockfd);

	return 0;
}

