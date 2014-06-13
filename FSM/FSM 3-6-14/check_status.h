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

#ifndef LOCALIZE_H
#define LOCALIZE_H  
#include "localize.h"
#endif

class CHECK_STATUS: public State
{
  public:
    CHECK_STATUS();
    ~CHECK_STATUS();
    State* executeState();
};

