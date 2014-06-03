#include "state.h"
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
    
    State* State::exec(int)
    {
		cout << "   Changing state!\n";
    }

