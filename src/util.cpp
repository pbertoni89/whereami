#include "util.h"

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
	return radToDeg(atan2((double)dy, (double)dx));
}

double getAngleLR(int dx, int dim) {
	double angle = (double)dx / (double) dim;
	return radToDeg(asin(angle));
}

double degToRad(double degs)
{
	static double scale = (double)CV_PI/(double)PIDEG;
	return degs*scale;
}

double radToDeg(double rads)
{
	static double scale = (double)PIDEG/(double)CV_PI;
	return rads*scale;
}
