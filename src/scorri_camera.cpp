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

#include<iostream>
#include<fstream>
#include <string>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <sstream>
#include <unistd.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
using namespace std;

int main() {
int cameraAngle=-90;
sleep(2);
while(cameraAngle<90){
	
		stringstream comando;
		cameraAngle=cameraAngle+1;
		comando << "morgulservo -- " << cameraAngle;
		string support = comando.str();
		const char* comandoDaEseguire = support.c_str();
		system(comandoDaEseguire);
		sleep(1);
}
return 0;
}


