#ifndef STATE_H
#define STATE_H    
#include "state.h"
#endif
#include "global.h"

class CHECK_STATUS: public State
{
  public:
    CHECK_STATUS();
    ~CHECK_STATUS();
    State* exec();
};

