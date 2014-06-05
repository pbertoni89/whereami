#ifndef QR_PROCESS_H
#define QR_PROCESS_H
#include "QR_process.h"
#endif

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
	State* QR_PROCESS::exec()
	{
	  delete this;
	  return new CHECK_STATUS();
	}



