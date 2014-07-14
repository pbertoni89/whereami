/*
 Copyright (C) 2014 brm: Patrizio Bertoni, Giovanni Richini, Michael Maghella
 
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    For any further information please contact 
	Patrizio Bertoni at giskard89@gmail.com, 
	Giovanni Richini at richgio2@hotmail.it, 
	Michael Maghella at magoo90@gmail.com
*/

#ifndef UTIL_H_
#define UTIL_H_

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "quirc.h"

#define PIDEG 180
#define HALFPIDEG PIDEG/2

using namespace cv;

void cv_to_quirc(struct quirc *q, Mat& img);

double pitagora(double a, double b);
double average(double a, double b);
double getAngleV(int dy, int dx);
double getAngleLR(int dx, int dim);
double degToRad(double degs);
double radToDeg(double rads);

#endif
