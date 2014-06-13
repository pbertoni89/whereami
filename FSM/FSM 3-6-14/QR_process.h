#ifndef STATE_H
#define STATE_H    
#include "state.h"
#endif

#ifndef GLOBAL_H
#define GLOBAL_H
#include "global.h"
#endif

#ifndef CHECK_STATUS_H
#define CHECK_STATUS_H  
#include "check_status.h"
#endif

class QR_PROCESS: public State
{
  public:
    QR_PROCESS();
    ~QR_PROCESS();
    State* executeState();
};

