#ifndef INIT_H
#define INIT_H    
#include "init.h"
#endif

#ifndef GLOBAL_H
#define GLOBAL_H
#include "global.h"
#endif

class Machine
{
  void setCurrent(State*);
  public:
    State* current;
    Machine();
    void start();
    
};


