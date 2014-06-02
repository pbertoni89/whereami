#include "on.h"
#include "off.h"

class Machine
{
  void setCurrent(State*);
  public:
    State* current;
    Machine();
    void move(int);

};


