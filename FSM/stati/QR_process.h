#ifndef STATE_H
#define STATE_H    
#include "state.h"
#endif
#include "global.h"

class QR_PROCESS: public State
{
  public:
    QR_PROCESS();
    ~QR_PROCESS();
    State* exec();
};

