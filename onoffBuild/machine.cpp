#include "machine.h"
#include <iostream>
using namespace std;

Machine::Machine()
{
  current = new ON();
  cout << "Machine built! \n";
}

void Machine::setCurrent(State *s)
{
	current = s;
}


void Machine::move(int choice)
{
  setCurrent(current->exec(choice));
}



