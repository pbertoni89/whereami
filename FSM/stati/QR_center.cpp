#include "QR_center.h"
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
	State* QR_CENTER::executeState()
	{
	  return new QR_CENTER();
	}


