#include "QR_process.h"
#include <iostream>
#include <stdio.h>

using namespace std;

    QR_PROCESS::QR_PROCESS()
    {
        cout << "   QR_PROCESS state\n";
    };
    
    QR_PROCESS::~QR_PROCESS()
    {
        cout << "   distroy QR_PROCESS\n";
    };


// transizione che va da ON a OFF
//  necessita della macchina per potergli settare lo stato
	State* QR_PROCESS::executeState()
	{
	  delete this;
	  return new QR_PROCESS();
	}



