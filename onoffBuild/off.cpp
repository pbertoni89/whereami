#include "on.h"
#include "off.h"
#include <iostream>
#include <stdio.h>

using namespace std;

    OFF::OFF()
    {
        cout << "   Costruisco OFF\n";
    };
    
    OFF::~OFF()
    {
        cout << "   Distruggo OFF\n";
    };


	State* OFF::exec(int choice)
	{
	  if(choice==1){	
	  cout << "   going from OFF to ON\n";
	  delete this;
	  return new ON();
	  }else{
		  cout << "i'm already OFF\n";
		  return this;
		  }
	}
