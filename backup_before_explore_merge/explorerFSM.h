#ifndef EXPLORERFSM_H
#define EXPLORERFSM_H

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

class State
{
public:
    State(void);
    ~State(void);
    virtual State* executeState()=0;
};

/** TURNS THE CAMERA TO INITIAL ANGLE. ID EST, LETS A SYSCALL DO THIS STUFF */
class State1_Init: public State
{
  public:
	State1_Init();
    ~State1_Init();
    State* executeState();
};

class State2_Finding: public State
{
  public:
	State2_Finding();
    ~State2_Finding();
    State* executeState();
};

class State3_Processing: public State
{
  public:
    State3_Processing();
    ~State3_Processing();
    State* executeState();
};

class State4_StatusChecking: public State
{
  public:
    State4_StatusChecking();
    ~State4_StatusChecking();
    State* executeState();
};


class State5_Localizing: public State
{
  public:
    State5_Localizing();
    ~State5_Localizing();
    State* executeState();
};

class State6_Error: public State
{
  public:
	State6_Error();
    ~State6_Error();
    State* executeState();
};

class ExplorerFSM {

private:
	State* currentState;
	void setCurrentState(State*);
public:
	ExplorerFSM();
	~ExplorerFSM();
	void runFSM();

};

#endif
