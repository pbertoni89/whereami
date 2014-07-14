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
