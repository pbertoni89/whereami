#include "machine.h"
#include <iostream>

using namespace std;

Machine::Machine()
{
  current = new INIT();
  cout << "Machine built! \n";
  QR_found=2;
}

void Machine::setCurrent(State *s)
{
	current = s;
}


void Machine::move()
{
  setCurrent(current->exec());
}



