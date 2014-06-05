#ifndef LOCALIZE_H
#define LOCALIZE_H    
#include "localize.h"
#endif
#include <iostream>
#include <stdio.h>

using namespace std;

    LOCALIZE::LOCALIZE()
    {
        cout << "   LOCALIZE state\n";
    };
    
    LOCALIZE::~LOCALIZE()
    {
        cout << "   distroy LOCALIZE\n";
    };


// transizione che va da ON a OFF
//  necessita della macchina per potergli settare lo stato
	State* LOCALIZE::exec()
	{
	  delete this;
	  return NULL;
	}



