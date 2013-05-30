/* quirc -- QR-code recognition library
 * Copyright (C) 2010-2012 Daniel Beer <dlbeer@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "quirc_internal.h"
#include "dbgutil.h"
#include <signal.h>
#ifndef __APPLE__
#include <wait.h>
#endif
#define MAXLENGTH 256

//used for the requests
#define BUFLEN 512
#define PORT "9930"
pthread_t serverThread;
pthread_t scanningThread;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
char * filePath;
VideoCapture capture;

#pragma pack(1)
typedef struct sqrinfos{
  int messageLength;
  int payloadTruncated;
  char qrMessage[MAXLENGTH+1];
  int x0,y0,x1,y1,x2,y2,x3,y3; // the coordinates of the four QR code points
  double distance;
  double perspectiveRotation; // inclinazione rispetto all'osservatore (profondita')
  double verticalRotation; // inclinazione verticale
  struct timeval timestampRecognition;
  struct timeval timestampCurrent;
} QRInfos;
#pragma pack(0)

int frameNum = 0;
QRInfos qrInfo;

using namespace cv;


int loadCameraParams(char* filename, Mat& intrinsic_matrix, Mat& distortion_coeffs){
    CvFileStorage* fs = cvOpenFileStorage( filename, 0, CV_STORAGE_READ );
    if (fs==NULL) return 1;

    intrinsic_matrix = (CvMat*)cvReadByName( fs,0,"camera_matrix");
    distortion_coeffs = (CvMat*)cvReadByName( fs,0,"distortion_coefficients");

    return 0;
}


void drawBoundingBox(Mat& img, const struct quirc_code *code){
  qrInfo.x0 = code->corners[0].x;
  qrInfo.y0 = code->corners[0].y;
  qrInfo.x1 = code->corners[1].x;
  qrInfo.y1 = code->corners[1].y;
  qrInfo.x2 = code->corners[2].x;
  qrInfo.y2 = code->corners[2].y;
  qrInfo.x3 = code->corners[3].x;
  qrInfo.y3 = code->corners[3].y;
  
  int npts = 4;
  CvPoint *points;
  points = (CvPoint*) malloc(sizeof(CvPoint)*npts);
  for(int i = 0; i< npts; i++){
    points[i] = cvPoint(code->corners[i].x,code->corners[i].y);
  }
  
  // draw center
  int xC, yC;
  xC = (qrInfo.x0+qrInfo.x2)/2;
  yC = (qrInfo.y0+qrInfo.y2)/2;
//  cvCircle( img, cvPoint(xC,yC), 3, cvScalar(127,127,127), -1, 8,0);
  circle(img, Point(xC,yC), 3, Scalar(127,127,127), -1, 8, 0);
  // compute and print rotation
  int deltaX = qrInfo.x1-qrInfo.x0;
  int deltaY = qrInfo.y1-qrInfo.y0;
  double angle = atan2(deltaY,deltaX)*180./CV_PI;
  printf("Inclinazione rispetto all'asse verticale: %f\n",angle);
  qrInfo.verticalRotation = angle;
  angle += 180;
  
  //cvFillPoly(img, &points, &npts, 1, cvScalar(127,127,127), 8, 0);
  //double lato1 = sqrt(pow((double)qrInfo.x0-qrInfo.x1,2)+pow((double)qrInfo.y0-qrInfo.y1,2));
  double lato2 = sqrt(pow((double)qrInfo.x1-qrInfo.x2,2)+pow((double)qrInfo.y1-qrInfo.y2,2));
  //double lato3 = sqrt(pow((double)qrInfo.x2-qrInfo.x3,2)+pow((double)qrInfo.y2-qrInfo.y3,2));
  double lato4 = sqrt(pow((double)qrInfo.x3-qrInfo.x0,2)+pow((double)qrInfo.y3-qrInfo.y0,2));
  
  if(angle < 45 || angle > 360-45){
    printf("AAAAAAAAAAAAAAAA");
  } else if(angle >= 45 && angle < 45+90){
    printf("BBBBBBBBBBBBBBBB");
  } else if (angle >= 45+90 && angle < 45+180){
    /*if(lato3>lato1){
      printf("Rivolto verso l'alto.\n");
    } else{
      printf("Rivolto verso il basso\n");
    }*/
    double fattore = 594; // distanza_nota * pixel_rilevati / dimensione_nota
    // 594 macbook
    // 815 philips
    double dimMMQR = 188; // 50 su iPhone, 260 su schermo intero da 19"
    double distanza_lato4 = fattore*dimMMQR/(lato4);
    double distanza_lato2 = fattore*dimMMQR/(lato2);

    int pixelQR = (lato2 + lato4)/2;

    const int threshold = pixelQR/13; // progressiva in base a quanti vicino/lontano è il QR, se vicino deve crescere.
    int latoDiff = lato4-lato2;
    latoDiff = abs(latoDiff) < threshold ? 0 : latoDiff;
    double angleProsp = asin((distanza_lato2-distanza_lato4)/dimMMQR)*180./CV_PI;
    
    qrInfo.perspectiveRotation = angleProsp;
    
    if(latoDiff > 0){
      printf("Rivolto verso destra (%lf).\n",angleProsp);
    } else if(latoDiff < 0){
      printf("Rivolto verso sinistra (%lf).\n",angleProsp);
    } else{
      printf("Rivolto frontalmente\n");
    }
    // distanza_qr = fattore * dimensione_nota / pixel_rilevati
    qrInfo.distance = (distanza_lato2+distanza_lato4)/(2 * 10);
    printf("Distanza dalla fotocamera (%d px): %lf cm\n",pixelQR,qrInfo.distance);
    gettimeofday(&qrInfo.timestampRecognition,NULL);
  } else{
    printf("DDDDDDDDDDDDDDDD");
  }
}

int elaboraQR(Mat& frame_BW, struct quirc *q){
	
	quirc_end(q);
  
	int count = quirc_count(q);
  if(count == 0){
    return 0;
  }
  
  // printf("%d QR-codes found:\n\n (frame elaborati %d)", count, ++frameNum);

	struct quirc_code code;
	struct quirc_data data;
	quirc_decode_error_t err;

	quirc_extract(q, 0, &code); // only recognize the first QR code found in the image
	err = quirc_decode(&code, &data);
  
	if (err == 0) {
    printf("Found new QR code.\n");
    printf("Requesting mutex\n");
    while(pthread_mutex_lock(&mutex) != 0);
    printf("Mutex acquired\n");
      drawBoundingBox(frame_BW, &code);
      int dataToCopy = data.payload_len;
      int payloadTruncated = 0;
      if(dataToCopy>MAXLENGTH){
        dataToCopy = MAXLENGTH;
        payloadTruncated = 1;
      }
      qrInfo.payloadTruncated = payloadTruncated;
      memcpy ( qrInfo.qrMessage, data.payload, dataToCopy+1 );
      qrInfo.qrMessage[MAXLENGTH] = '\0';
      printf("Payload: %s\n ",qrInfo.qrMessage);
      printf("\n");
      dump_data(&data);
    while(pthread_mutex_unlock(&mutex) != 0);
    return 0;
	}
  /*
  Mat frame_small;
  resize(frame_BW, frame_small, Size(frame_BW.cols/4, frame_BW.rows/4));
  imshow("Frame",frame_BW);
  */
  return 0;
}


void* scanningFunc(void *arg){
  printf("Scanning thread started!\n");
	

  Mat intrinsic_matrix, distortion_coeffs;
  loadCameraParams(filePath,intrinsic_matrix, distortion_coeffs);

	struct quirc *q; 
  
	q = quirc_new();
	if (!q) {
		perror("Can't create quirc object");
		exit(1);
	}

  Mat frame, frame_undistort, frame_BW, frame_small;
  for(;;){
    
    
    capture >> frame;
    if (!frame.data) {
      printf("Errore durante il caricamento del frame.\n");
    }
    undistort(frame, frame_undistort, intrinsic_matrix, distortion_coeffs);
    cvtColor(frame_undistort, frame_BW, CV_BGR2GRAY);
    cv_to_quirc(q, frame_BW);
    elaboraQR(frame_BW,q);
    
    
  }
  quirc_destroy(q);
}

void diep(char *s){
  perror(s);
  exit(1);
}

#define BACKLOG 10	 // how many pending connections queue will hold

void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void* serverFunc(void* arg){
  printf("Server thread started!\n");
  
  
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

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);

    while(pthread_mutex_lock(&mutex) != 0);
    gettimeofday(&qrInfo.timestampCurrent,NULL);
    
		if (send(new_fd, &qrInfo, sizeof(QRInfos), 0) == -1){
      while(pthread_mutex_unlock(&mutex) != 0);
			perror("send");
    }
    while(pthread_mutex_unlock(&mutex) != 0);
		close(new_fd);
	}
}

int main(int argc, char **argv){
  qrInfo.messageLength = MAXLENGTH;
  
  pthread_mutex_unlock(&mutex);
  if(argc < 2){
    printf("Usage: ./inspect calibration_file.yml\n");
    exit(1);
  }
  filePath = argv[1];
  
  capture.open(0);
  if (!capture.isOpened()) {
      printf("Errore durante il caricamento del capture.\n");
      exit(1);
  }
  
  int ret = pthread_create(&serverThread, NULL, &serverFunc, NULL);
  if(ret != 0){
    printf("Error creating the server thread. [%s]\n",strerror(ret));
    exit(1);
  }
  
  ret = pthread_create(&scanningThread, NULL, &scanningFunc, NULL);
  if(ret != 0){
    printf("Error creating the scanner thread. [%s]\n",strerror(ret));
    exit(1);
  }

  pthread_exit(NULL);

	return 0;
}
