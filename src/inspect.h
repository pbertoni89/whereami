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
#define THRESH 13 	 // mysterious magic number.
#define LBOUND 135	 // minimum angle of vertical rotation allowed. set to 0 to disable.
#define UBOUND 225	 // maximum angle of vertical rotation allowed. set to 359 to disable.
#define DEBUG		 // it will help us. Comment for excluding preprocessing.

pthread_t server_thread;
pthread_t scanning_thread;
pthread_mutex_t mutex;
const string msg = "press ESC to abort the inspection.";
const char* window_title = "Camera Source";
char* file_path;
int camera_id;
bool camera_feedback;
VideoCapture capture;
int frame_number;
int qr_number; //in the future it will be increased only when a DIFFERENT QR is found
QRInfos qr_info;
// scale_factor = known_qr_distance_mm * pixels_measured / known_size_mm. It is 
int scale_factor;
// the size in millimetres of the QR code acquired during CALIBRATION
int qr_size_mm;

void close_all_threads();
int load_camera_params(char* filename, Mat& intrinsic_matrix, Mat& distortion_coeffs);
void extrapolate_qr_informations(const struct quirc_code *code);
int process_qr(struct quirc *q);
void* scanning_func(void* arg);
void sigchld_handler(int s);
void* get_in_addr(struct sockaddr *sa);
void* server_func(void* arg);
int getOpt(int cargc, char** cargv);
void help();
void init_inspect();
void calcPerspective_Distance(double side_a, double side_b);
void calcCenter_VerticalRot();
void copyPayload(quirc_data *data);
void printQRInfo();
