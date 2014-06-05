#ifndef UTIL_H_
#define UTIL_H_

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "quirc.h"

#define PIDEG 180

using namespace cv;

void cv_to_quirc(struct quirc *q, Mat& img);

double pitagora(double a, double b);
double average(double a, double b);
double getAngleV(int dy, int dx);
double getAngleLR(int dx, int dim);

#endif
