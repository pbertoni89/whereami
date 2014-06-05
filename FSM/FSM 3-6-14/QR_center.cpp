#ifndef QR_CENTER_H
#define QR_CENTER_H
#include "QR_center.h"
#endif

#include <iostream>
#include <stdio.h>

using namespace std;

    QR_CENTER::QR_CENTER()
    {
        cout << "   QR_CENTER state\n";
    };
    
    QR_CENTER::~QR_CENTER()
    {
        cout << "   distroy QR_CENTER\n";
    };


// transizione che va da ON a OFF
//  necessita della macchina per potergli settare lo stato
	State* QR_CENTER::exec()
	{
	  return new QR_PROCESS();
	}



