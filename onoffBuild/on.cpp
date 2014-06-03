#include "on.h"
#include "off.h"
#include <iostream>
#include <stdio.h>

using namespace std;

    ON::ON()
    {
        cout << "   Costruisco ON ";
    };
    
    ON::~ON()
    {
        cout << "   Distruggo ON\n";
    };


// transizione che va da ON a OFF
//  necessita della macchina per potergli settare lo stato
	State* ON::exec(int choice)
	{
	  if(choice==0){
	  cout << "   going from ON to OFF\n";
	  delete this;
	  return new OFF();
	  }else{
		  cout << "i'm already ON\n";
		  return this;
		  }
	}


