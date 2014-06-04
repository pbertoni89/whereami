#ifndef QR_FIND_H
#define QR_FIND_H  
#include "QR_find.h"
#endif

#include <iostream>
#include <stdio.h>

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
      cout << "   arrivato in QR_FIND\n";
	  delete this;
	  return NULL;
	}



