#include "inspect.h"

/** Closes all thread launched at server side. ---------------------------------------------------*/
void close_all_threads() {
  // one of the two threads caught an error and will now close all threads to terminate the program
  int rc;
  rc = pthread_kill(server_thread, SIGTERM);
  rc = pthread_kill(scanning_thread, SIGTERM);
}

/** Load camera parameters. ----------------------------------------------------------------------*/
int load_camera_params(char* filename, Mat& intrinsic_matrix, Mat& distortion_coeffs) {
  CvFileStorage* fs = cvOpenFileStorage(filename, 0, CV_STORAGE_READ);
  if (fs==NULL){
    return 1;
  }

  intrinsic_matrix = (CvMat*)cvReadByName(fs, 0, "camera_matrix");
  distortion_coeffs = (CvMat*)cvReadByName(fs, 0, "distortion_coefficients");
  scale_factor = cvReadIntByName(fs, 0, "scale_factor");
  qr_size_mm = cvReadIntByName(fs, 0, "qr_size_mm");

  return 0;
}

void copyCorners(const struct quirc_code *code) {
	qr_info.x0 = code->corners[0].x;
	qr_info.y0 = code->corners[0].y;
	qr_info.x1 = code->corners[1].x;
	qr_info.y1 = code->corners[1].y;
	qr_info.x2 = code->corners[2].x;
	qr_info.y2 = code->corners[2].y;
	qr_info.x3 = code->corners[3].x;
	qr_info.y3 = code->corners[3].y;
}

int scale(double side) {
	return scale_factor*qr_size_mm/side;
}

/** Calculate perspective rotation and distance of the QR. ---------------------------------------*/
void calcPerspective_Distance(double side2, double side4) {

	int qr_pixel_size = average(side2, side4);
	int s2_dist = scale(side2);
	int s4_dist = scale(side4);
	qr_info.distance = average(s2_dist, s4_dist);

	static double threshold = qr_pixel_size/double(THRESH); // progressive based on how far the QR code is (near -> increase).
	int s_LR_dist_delta = s2_dist - s4_dist;
	
	if (abs(s_LR_dist_delta) < threshold)
		s_LR_dist_delta = 0;
	else
		s_LR_dist_delta;
		
	qr_info.perspective_rotation = getAngleLR(s_LR_dist_delta, qr_size_mm); 
}

void calcCenter_VerticalRot() {
	
	int x_center = average(qr_info.x0, qr_info.x2);
	int y_center = average(qr_info.y0, qr_info.y2);
	#ifdef DEBUG
		printf("QR number: %d\n", qr_number);
		//printf("Center coordinates: (%d, %d)\n", x_center, y_center);
	#endif

	int dx = qr_info.x1 - qr_info.x0;
	int dy = qr_info.y1 - qr_info.y0;
	qr_info.vertical_rotation = getAngleV(dy, dx) + PIDEG;
}

void printQRInfo() {
	printf("*********************************************\n");
    printf("Perspective rotation\t\t%d\n", qr_info.perspective_rotation);
    printf("Distance from camera\t\t%d\n", qr_info.distance);
    printf("Payload\t\t%s\n ",qr_info.qr_message);
    printf("*********************************************\n\n");
}

/** Extrapolate QR informations. -----------------------------------------------------------------*/
void extrapolate_qr_informations(const struct quirc_code *code) {

	copyCorners(code);
	calcCenter_VerticalRot();
	
	/* p0        side1        p1
       ▄▄▄▄▄▄▄ ▄▄▄▄▄ ▄▄▄▄▄▄▄ 
       █ ▄▄▄ █  █▄▀  █ ▄▄▄ █ 
    s  █ ███ █  ▀█▀▄ █ ███ █ s
    i  █▄▄▄▄▄█ ▄ ▄▀█ █▄▄▄▄▄█ i
    d  ▄▄ ▄▄ ▄ ▀██▀█ ▄     ▄ d
    e  ▀▀█▄▀█▄▄▄   ▀▄█ █▄█▀  e
    4  ▀ ███▀▄█▄▄█▀  ▄  ██▀█ 2
       ▄▄▄▄▄▄▄ ▀▀█▀█▄▄█▄▀ ▀█ 
       █ ▄▄▄ █ ▄ ▄██▀▄█▀█▄ █ 
       █ ███ █ ▀▄█▀▀ ▄ ▀▀█▄▄ 
       █▄▄▄▄▄█ █▄▀▄ ▀▀ ▀▄█▄▀ 
     p3        side3        p2    */
     
	double side2 = pitagora((double) (qr_info.x1 - qr_info.x2), (double)(qr_info.y1 - qr_info.y2));
	double side4 = pitagora((double) (qr_info.x3 - qr_info.x0), (double)(qr_info.y3 - qr_info.y0));
  
	calcPerspective_Distance(side2, side4);
	
	gettimeofday(&qr_info.timestamp_recognition, NULL);
}

/** Copies payload from data to info structure. --------------------------------------------------*/
void copyPayload(quirc_data *data) {

	int payload_len = data->payload_len;
    int payloadTruncated = 0;
    if(payload_len > MAXLENGTH-1){
      payload_len = MAXLENGTH-1;  // truncate long payloads
      payloadTruncated = 1;
    }
    qr_info.payload_truncated = payloadTruncated;
    memcpy (qr_info.qr_message, data->payload, payload_len+1 ); // copy the '\0' too.
	qr_info.qr_message[MAXLENGTH] = '\0';
}

/** Processes QR code. ---------------------------------------------------------------------------*/
int process_qr(struct quirc *q) {
  
  quirc_end(q);
  
  int count = quirc_count(q);
  if(count == 0){ // no QR codes found.
    return 0;
  }
  
  qr_number++;
  //printf("%d QR-codes found. Analyzin the first one. (frame_number: %d)", count, ++frame_number);

  struct quirc_code code;
  struct quirc_data data;
  quirc_decode_error_t err;

  quirc_extract(q, 0, &code); // only recognize the first QR code found in the image
  err = quirc_decode(&code, &data);
  
  if(err == 0){
    while(pthread_mutex_lock(&mutex) != 0)
	{ ; }
	
    qr_info.message_length = MAXLENGTH;
    extrapolate_qr_informations(&code);
    copyPayload(&data);
    printQRInfo(); // the only excerpt of a QR that should be printed in a release.
    
    while(pthread_mutex_unlock(&mutex) != 0)
    { ; }
  }
  return 0;
}

/** Scanning function. ---------------------------------------------------------------------------*/
void* scanning_func(void *arg) {
	
	printf("Scanning thread started.\n");
	struct quirc *q;
	Mat intrinsic_matrix, distortion_coeffs;
	load_camera_params(file_path, intrinsic_matrix, distortion_coeffs);

	namedWindow(window_title, 1);

	Mat frame, frame_undistort, frame_BW;
	while(1) {
        
        if( !capture.isOpened() )
			{ perror("Error opening capture.\n"); close_all_threads(); }

		Mat frame0;
        capture >> frame0;
        frame0.copyTo(frame);

		if (!frame.data) 
			{ perror("Error loading the frame.\n"); close_all_threads(); }
			
		Point textOrigin(50, frame.rows-10);
        putText(frame, msg, textOrigin, 1, 1, Scalar(0,0,255) );
                 
		imshow(window_title, frame);

		undistort(frame, frame_undistort, intrinsic_matrix, distortion_coeffs);
		cvtColor(frame_undistort, frame_BW, CV_BGR2GRAY);
		
		q = quirc_new();
		if(!q)
			{ perror("Can't create quirc object"); close_all_threads(); }
		cv_to_quirc(q, frame_BW);
		process_qr(q);
		quirc_destroy(q);
		
		int key = 0xff & waitKey(capture.isOpened() ? 50 : 500);
        if( (key & 255) == 27 )
           exit(0);
	}
	cvDestroyWindow(window_title);
}

/** Signal "chld" handler. -----------------------------------------------------------------------*/
void sigchld_handler(int s) {
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

/** Get socket address, IPv4 xor IPv6. -----------------------------------------------------------*/
void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/** Server function. -----------------------------------------------------------------------------*/
void* server_func(void* arg) {
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
    close_all_threads();
  }

  // loop through all the results and bind to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
      perror("server: socket");
      continue;
    }

    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
      perror("setsockopt");
      close_all_threads();
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
    close_all_threads();
  }

  freeaddrinfo(servinfo); // all done with this structure

  if(listen(sockfd, BACKLOG) == -1){
    perror("listen");
    close_all_threads();
  }

  sa.sa_handler = sigchld_handler; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if(sigaction(SIGCHLD, &sa, NULL) == -1){
    perror("sigaction");
    close_all_threads();
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

    while(pthread_mutex_lock(&mutex) != 0);
    gettimeofday(&qr_info.timestamp_current, NULL);
    
    if (send(new_fd, &qr_info, sizeof(QRInfos), 0) == -1){
      while(pthread_mutex_unlock(&mutex) != 0);
      perror("send");
    }
    while(pthread_mutex_unlock(&mutex) != 0);
    close(new_fd);
  }
}

/** Prints help. ---------------------------------------------------------------------------------*/
void help() {
    printf( "QRlocate codes inspector.\n"
        "Usage: inspect\n"
        "\tcalibration_file.yml\t# the yml file where calibration data are stored\n"
        "\t[-cf]\t\t\t# camera feedback through a window of its stream. default is false\n"
        "\t[-id <camera_id>]\t# the camera ID for capture opening. default is 0\n"
        "\n" );
}

/** Sets some things up. -------------------------------------------------------------------------*/
void init_inspect() {
	mutex = PTHREAD_MUTEX_INITIALIZER;
	file_path = "out_camera_data.yml";
	camera_id = 0;
	camera_feedback = false;
	frame_number = 0;
	qr_number = 0;
	qr_info.message_length = -1;
}

/** Get argv parameters. -------------------------------------------------------------------------*/
int getOpt(int cargc, char** cargv) {

  if(cargc < 2){
    help();
    exit(1);
  }
  file_path = cargv[1];
    
  for(int i=2; i<cargc; i++)
    {
        const char* s = cargv[i];
        if( strcmp( s, "-cf" ) == 0 )
        {
            camera_feedback = true;
        }
        else if( (s[0] != '-') && isdigit(s[0]) )
        {
			sscanf(s, "%d", &camera_id);
        }
        else
            return fprintf( stderr, "Unknown option %s", s ), -1;
    }	
    return 0;	
}

/** MAIN function. -------------------------------------------------------------------------------*/
int main(int argc, char **argv){
	
	init_inspect();
	if (getOpt(argc, argv) != 0)
		return -1;
  
	pthread_mutex_unlock(&mutex);
  
	capture.open(camera_id);
	if (!capture.isOpened()) {
		printf("Error during capture opening.\n");
		exit(1);
	  }
	  
	int ret = pthread_create(&server_thread, NULL, &server_func, NULL);
	if(ret != 0) {
		printf("Error creating the server thread. [%s]\n", strerror(ret));
		exit(1);
	}
	  
	ret = pthread_create(&scanning_thread, NULL, &scanning_func, NULL);
	if(ret != 0) {
		printf("Error creating the scanner thread. [%s]\n", strerror(ret));
		close_all_threads();
		exit(1);
	}

	pthread_exit(NULL);
	return 0;
}
