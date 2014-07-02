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


