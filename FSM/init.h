#ifndef STATE_H
#define STATE_H    
#include "state.h"
#endif

#ifndef GLOBAL_H
#define GLOBAL_H
#include "global.h"
#endif

#ifndef QR_FIND_H
#define QR_FIND_H
#include "QR_find.h"
#endif

class INIT: public State
{
  public:
    INIT();
    ~INIT();
    State* exec();
};

