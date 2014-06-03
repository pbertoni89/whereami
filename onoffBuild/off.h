#ifndef STATE_H
#define STATE_H    
#include "state.h"
#endif

class OFF : public State{
  public:
    OFF();
    ~OFF();
    State* exec(int);
};
