#include <iostream>
#include "machine.h"
#include <iostream>

using namespace std;

int main()
{

  Machine fsm;
  int num;
  while (1)
  {
    cout << "-------------------------------\nEnter 0 to switch off and 1 to turn on: ";
    cin >> num; // inserisci un intero...
    fsm.move(num);
  }
}
