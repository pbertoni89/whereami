#include <stdio.h>
#include <string.h>
#include <math.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>
using namespace cv;

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#ifndef __APPLE__
  #include <wait.h>
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "quirc_internal.h"
#include "util.h"
#include "message.h"

#define BACKLOG 10   // how many pending connections queue will hold

pthread_t server_thread;
pthread_t scanning_thread;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
char * file_path;
VideoCapture capture;
int frame_number = 0;
QRInfos qr_info;

// scale_factor = known_qr_distance_mm * pixels_measured / known_size_mm. It is 
int scale_factor;

// the size in millimetres of the QR code
int qr_size_mm;

void close_all_threads();
int load_camera_params(char* filename, Mat& intrinsic_matrix, Mat& distortion_coeffs);
void extrapolate_qr_informations(const struct quirc_code *code);
int process_qr(struct quirc *q);
void* scanning_func(void *arg);
void sigchld_handler(int s);
void *get_in_addr(struct sockaddr *sa);
void* server_func(void* arg);
