#ifndef STATE_H
#define STATE_H    
#include "state.h"
#endif

#ifndef GLOBAL_H
#define GLOBAL_H
#include "global.h"
#endif

#ifndef QR_PROCESS_H
#define QR_PROCESS_H    
#include "QR_process.h"
#endif


class QR_CENTER: public State
{
  public:
    QR_CENTER();
    ~QR_CENTER();
    State* executeState();
};

