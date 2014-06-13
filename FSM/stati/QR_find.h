#ifndef STATE_H
#define STATE_H    
#include "state.h"
#endif
#include "global.h"

class QR_FIND: public State
{
  public:
    QR_FIND();
    ~QR_FIND();
    State* executeState();
};

