#ifndef STATE_H
#define STATE_H    
#include "state.h"
#endif

#include <iostream>
using namespace std;

    State::State()
    {
        cout << "   Create state\n ";
    };
    
    State::~State()
    {
        cout << "   Delete state\n";
    };
    
    State* State::exec()
    {
		cout << "   Changing state!\n";
    }

