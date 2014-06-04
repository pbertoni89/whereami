#include "machine.h"
#include <iostream>

using namespace std;

Machine::Machine()
{
  cout << "Machine built! \n";
  current = new INIT();
}

void Machine::setCurrent(State *s)
{
	current = s;
}


void Machine::start()
{
  setCurrent(current->exec());
}



