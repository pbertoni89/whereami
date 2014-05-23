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
  CvFileStorage* fs = cvOpenFileStorage( filename, 0, CV_STORAGE_READ );
  if (fs==NULL){
    return 1;
  }

  intrinsic_matrix = (CvMat*)cvReadByName( fs,0,"camera_matrix");
  distortion_coeffs = (CvMat*)cvReadByName( fs,0,"distortion_coefficients");
  scale_factor = cvReadIntByName( fs,0,"scale_factor");
  qr_size_mm = cvReadIntByName( fs,0,"qr_size_mm");

  return 0;
}

/** Extrapolate QR informations. -----------------------------------------------------------------*/
void extrapolate_qr_informations(const struct quirc_code *code) {
  qr_info.x0 = code->corners[0].x;
  qr_info.y0 = code->corners[0].y;
  qr_info.x1 = code->corners[1].x;
  qr_info.y1 = code->corners[1].y;
  qr_info.x2 = code->corners[2].x;
  qr_info.y2 = code->corners[2].y;
  qr_info.x3 = code->corners[3].x;
  qr_info.y3 = code->corners[3].y;
  
  int n_points = 4;
  CvPoint *points;
  points = (CvPoint*) malloc(sizeof(CvPoint)*n_points);
  for(int i=0; i < n_points; i++){
    points[i] = cvPoint(code->corners[i].x,code->corners[i].y);
  }
  
  // compute center coordinates
  int x_center, y_center;
  x_center = (qr_info.x0+qr_info.x2)/2;
  y_center = (qr_info.y0+qr_info.y2)/2;
  printf("Center coordinates: (%d, %d)\n",x_center, y_center);

  // compute and print rotation
  int dx = qr_info.x1-qr_info.x0;
  int dy = qr_info.y1-qr_info.y0;
  int angle = atan2(dy,dx)*180./CV_PI;
  printf("Vertical rotation: %d deg\n",angle);
  qr_info.vertical_rotation = angle;
  angle += 180;
  /*
     p0        side1        p1
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
     p3        side4        p2
  */
  double side2 = sqrt(pow((double)qr_info.x1-qr_info.x2,2)+pow((double)qr_info.y1-qr_info.y2,2));
  double side4 = sqrt(pow((double)qr_info.x3-qr_info.x0,2)+pow((double)qr_info.y3-qr_info.y0,2));
  
  // we only care of the QR code when it is correctly rotated (135 to 225 degrees)
  if(angle < 45 || angle > 360-45){  //--------------------------------------------------------------------
			int side4_distance = scale_factor*qr_size_mm/side4;
			int side2_distance = scale_factor*qr_size_mm/side2;

			int qr_pixel_size = (side2 + side4)/2;

			const int threshold = qr_pixel_size/13; // progressive based on how far the QR code is (near -> increase).
			int side_difference = side4-side2;
			side_difference = abs(side_difference) < threshold ? 0 : side_difference;
			int perspective_rotation = asin((side2_distance - side4_distance)/qr_size_mm)*180./CV_PI;
			
			qr_info.perspective_rotation = perspective_rotation;
			qr_info.distance = (side2_distance + side4_distance)/2;
			printf("[0-45]U[315-360]: Distance from the camera (%d px): %d mm\n", qr_pixel_size, qr_info.distance);
  } else if(angle >= 45 && angle < 45+90){ //------------------------------------------------------------------
			int side4_distance = scale_factor*qr_size_mm/side4;
			int side2_distance = scale_factor*qr_size_mm/side2;

			int qr_pixel_size = (side2 + side4)/2;

			const int threshold = qr_pixel_size/13; // progressive based on how far the QR code is (near -> increase).
			int side_difference = side4-side2;
			side_difference = abs(side_difference) < threshold ? 0 : side_difference;
			int perspective_rotation = asin((side2_distance - side4_distance)/qr_size_mm)*180./CV_PI;
			
			qr_info.perspective_rotation = perspective_rotation;
			qr_info.distance = (side2_distance + side4_distance)/2;
			printf("[45-135]: Distance from the camera (%d px): %d mm\n", qr_pixel_size, qr_info.distance);
  } else if (angle >= 45+90 && angle < 45+180){
    /*if(side3>side1){
      printf("Facing upwards.\n");
    } else{
      printf("Facing downwards.\n");
    }*/
    
    // distance = scale_factor * qr_size_mm / side_pixel_size
    int side4_distance = scale_factor*qr_size_mm/side4;
    int side2_distance = scale_factor*qr_size_mm/side2;

    int qr_pixel_size = (side2 + side4)/2;

    const int threshold = qr_pixel_size/13; // progressive based on how far the QR code is (near -> increase).
    int side_difference = side4-side2;
    side_difference = abs(side_difference) < threshold ? 0 : side_difference;
    int perspective_rotation = asin((side2_distance - side4_distance)/qr_size_mm)*180./CV_PI;
    
    qr_info.perspective_rotation = perspective_rotation;
    
    if(side_difference > 0){
      printf("Facing right (%d deg).\n", perspective_rotation);
    } else if(side_difference < 0){
      printf("Facing left (%d deg).\n", perspective_rotation);
    } else{
      printf("Facing front.\n");
    }
    
    qr_info.distance = (side2_distance + side4_distance)/2;
    printf("Distance from the camera (%d px): %d mm\n", qr_pixel_size, qr_info.distance);
    gettimeofday(&qr_info.timestamp_recognition, NULL);
  } 
  else{ //-----------------------------------------------------------------------------------------
	  int side4_distance = scale_factor*qr_size_mm/side4;
    int side2_distance = scale_factor*qr_size_mm/side2;

    int qr_pixel_size = (side2 + side4)/2;

    const int threshold = qr_pixel_size/13; // progressive based on how far the QR code is (near -> increase).
    int side_difference = side4-side2;
    side_difference = abs(side_difference) < threshold ? 0 : side_difference;
    int perspective_rotation = asin((side2_distance - side4_distance)/qr_size_mm)*180./CV_PI;
    
    qr_info.perspective_rotation = perspective_rotation;
    printf("[225-315]: Distance from the camera (%d px): %d mm\n", qr_pixel_size, qr_info.distance);
  }
}

/** Processes QR code. ---------------------------------------------------------------------------*/
int process_qr(struct quirc *q) {
  
  quirc_end(q);
  
  int count = quirc_count(q);
  if(count == 0){ // no QR codes found.
    return 0;
  }
  
  // printf("%d QR-codes found. Analyzin the first one. (frame_number: %d)", count, ++frame_number);

  struct quirc_code code;
  struct quirc_data data;
  quirc_decode_error_t err;

  quirc_extract(q, 0, &code); // only recognize the first QR code found in the image
  err = quirc_decode(&code, &data);
  
  if(err == 0){
    while(pthread_mutex_lock(&mutex) != 0);
    qr_info.message_length = MAXLENGTH;
    extrapolate_qr_informations(&code);
    int payload_len = data.payload_len;
    int payloadTruncated = 0;
    if(payload_len > MAXLENGTH-1){
      // truncate long payloads
      payload_len = MAXLENGTH-1;
      payloadTruncated = 1;
    }
    qr_info.payload_truncated = payloadTruncated;
    memcpy ( qr_info.qr_message, data.payload, payload_len+1 ); // copy the '\0' too.

    qr_info.qr_message[MAXLENGTH] = '\0';
    printf("Payload: %s\n ",qr_info.qr_message);
    printf("\n");
    
    while(pthread_mutex_unlock(&mutex) != 0);
  }
  return 0;
}

/** Scanning function. ---------------------------------------------------------------------------*/
void* scanning_func(void *arg) {
	
	const char* window_title = "Image View";
	printf("Scanning thread started.\n");
	struct quirc *q;
	q = quirc_new();
	if(!q){
		perror("Can't create quirc object");
		close_all_threads();
	}

	Mat intrinsic_matrix, distortion_coeffs;
	load_camera_params(file_path, intrinsic_matrix, distortion_coeffs);

	namedWindow(window_title, 1);

	Mat frame, frame_undistort, frame_BW, frame_small;
	while(1) {
        
        if( capture.isOpened() )
        {
            Mat frame0;
            capture >> frame0;
            frame0.copyTo(frame);
        }

		if (!frame.data)
			printf("Error loading the frame.\n");
			
		Point textOrigin(50, frame.rows-10);
        putText( frame, msg, textOrigin, 1, 1, Scalar(0,0,255) );
                 
		imshow(window_title, frame);

		undistort(frame, frame_undistort, intrinsic_matrix, distortion_coeffs);
		cvtColor(frame_undistort, frame_BW, CV_BGR2GRAY);
		cv_to_quirc(q, frame_BW);
		process_qr(q);
		
		int key = 0xff & waitKey(capture.isOpened() ? 50 : 500);
        if( (key & 255) == 27 )
           exit(0);
	}
	cvDestroyWindow(window_title);
	quirc_destroy(q);
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
	if ( getOpt(argc, argv) != 0)
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
