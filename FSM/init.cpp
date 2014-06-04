#ifndef INIT_H
#define INIT_H    
#include "init.h"
#endif

#include <iostream>
#include <stdio.h>

using namespace std;

    INIT::INIT()
    {
        cout << "   INIT state\n";
    };
    
    INIT::~INIT()
    {
        cout << "   distroy INIT\n";
    };


// transizione che va da ON a OFF
//  necessita della macchina per potergli settare lo stato
	State* INIT::exec()
	{
	  system("morgulservo -- -90");
	  camera_angle=-90;
	  QR_found=0;
	  delete this;
	  return new QR_FIND();
	}



