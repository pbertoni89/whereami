#ifndef STATE_H
#define STATE_H    
#include "state.h"
#endif

#ifndef GLOBAL_H
#define GLOBAL_H
#include "global.h"
#endif


class LOCALIZE: public State
{
  public:
    LOCALIZE();
    ~LOCALIZE();
    State* executeState();
};

