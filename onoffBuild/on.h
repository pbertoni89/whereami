#ifndef STATE_H
#define STATE_H    
#include "state.h"
#endif

class ON: public State
{
  public:
    ON();
    ~ON();
    State* exec(int);
};

