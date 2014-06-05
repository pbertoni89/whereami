#ifndef STATE_H
#define STATE_H    
#include "state.h"
#endif
#include "global.h"

class LOCALIZE: public State
{
  public:
    LOCALIZE();
    ~LOCALIZE();
    State* exec();
};

