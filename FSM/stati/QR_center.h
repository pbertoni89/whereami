#ifndef STATE_H
#define STATE_H    
#include "state.h"
#endif
#include "global.h"

class QR_CENTER: public State
{
  public:
    QR_CENTER();
    ~QR_CENTER();
    State* exec();
};

