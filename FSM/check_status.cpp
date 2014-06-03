#ifndef CHECK_STATUS_H
#define CHECK_STATUS_H  
#include "check_status.h"
#endif

#include <iostream>
#include <stdio.h>

using namespace std;

    CHECK_STATUS::CHECK_STATUS()
    {
        cout << "   CHECK_STATUS state\n";
    };
    
    CHECK_STATUS::~CHECK_STATUS()
    {
        cout << "   distroy CHECK_STATUS\n";
    };


// transizione che va da ON a OFF
//  necessita della macchina per potergli settare lo stato
	State* CHECK_STATUS::exec()
	{
	  delete this;
	  return new LOCALIZE();
	}



