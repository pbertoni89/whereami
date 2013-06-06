#ifndef UTIL_H_
#define UTIL_H_

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "quirc.h"

using namespace cv;

void cv_to_quirc(struct quirc *q, Mat& img);

#endif