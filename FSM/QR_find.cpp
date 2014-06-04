#ifndef QR_FIND_H
#define QR_FIND_H  
#include "QR_find.h"
#endif
#include <time.h>       /* time */
#include <iostream>
#include <stdio.h>
#include <string>     // std::string, std::to_string
#include <stdlib.h>     /* srand, rand */
#include <sstream>
#include <unistd.h>
using namespace std;

    QR_FIND::QR_FIND()
    {
        cout << "   QR_FIND state\n";
    };
    
    QR_FIND::~QR_FIND()
    {
        cout << "   distroy QR_FIND\n";
    };


// transizione che va da ON a OFF
//  necessita della macchina per potergli settare lo stato
	State* QR_FIND::exec()
	{
	  bool trovato=true;
      while(trovato){


		//std::string s = std::to_string(camera_angle);
		
		
		if(camera_angle==85){
			camera_angle=-85;
		}
		camera_angle =camera_angle+1;
		
	
		
		stringstream ss;
		ss << "morgulservo -- " << camera_angle;
		cout << ss.str();
		//const char* x=ss.str();
		
		string s = ss.str();
		const char* p = s.c_str();
		system(p);
		sleep(0.5); 
			
}
	
	  delete this;
	  return NULL;
	}



