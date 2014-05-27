#include <string.h>
#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "util.h"

using namespace cv;

void cv_to_quirc(struct quirc *q, Mat& img){
	uint8_t *image;
	quirc_resize(q, img.cols, img.rows);

	image = quirc_begin(q, NULL, NULL);
    
	for (int y = 0; y < img.rows; y++) {
    uint8_t *row_pointer = image + y * img.cols;
        for(int x = 0; x < img.cols; x++){
            row_pointer[x] = img.at<uint8_t>(y, x);
        }
	}
}

double pitagora(double a, double b) {
	return sqrt(pow(a, 2)+pow(b, 2));
}

double average(double a, double b) {
	return (a+b)/2;
}

double getAngleV(int dy, int dx) {
	return atan2((double)dy, (double)dx)*((double)PIDEG/(double)CV_PI);
}
double getAngleLR(int dx, int dim) {
	double 
	return asin((double)dx/(double)dim)*((double)PIDEG/(double)CV_PI);
}

