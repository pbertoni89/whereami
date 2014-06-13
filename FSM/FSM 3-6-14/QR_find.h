#ifndef STATE_H
#define STATE_H    
#include "state.h"
#endif

#ifndef GLOBAL_H
#define GLOBAL_H
#include "global.h"
#endif

#ifndef QR_CENTER_H
#define QR_CENTER_H    
#include "QR_center.h"
#endif

#ifndef CHECK_STATUS_H
#define CHECK_STATUS_H  
#include "check_status.h"
#endif

class QR_FIND: public State
{
  public:
    QR_FIND();
    ~QR_FIND();
    State* executeState();
};

