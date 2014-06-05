#include "QR_find.h"
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

	  delete this;
	  return new QR_FIND();
	}



