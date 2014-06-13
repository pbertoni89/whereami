#include "explorerFSM.h"

State::State()
{
	cout << "   Create state\n ";
};

State::~State()
{
	cout << "   Delete state\n";
};

State* State::executeState()
{
	cout << "   Changing state!\n";
	return NULL;
}

// --------------------- ---------------- ---------------- ---------------- ---------------- ----------------

State1_Init::State1_Init()
{
	cout << "   State1_Init state\n";
};

State1_Init::~State1_Init()
{
	cout << "   distroy State1_Init\n";
};

State* State1_Init::executeState()
{
  //camera_angle=-90;
  //QR_found=0;
  delete this;
  return new State2_Finding();
}

// --------------------- ---------------- ---------------- ---------------- ---------------- ----------------

State2_Finding::State2_Finding()
{
	cout << "   State2_Finding state\n";
};

State2_Finding::~State2_Finding()
{
	cout << "   distroy State2_Finding\n";
};

State* State2_Finding::executeState()
{

  delete this;
  return new State3_Processing();
}

// --------------------- ---------------- ---------------- ---------------- ---------------- ----------------

State3_Processing::State3_Processing()
{
	cout << "   State3_Processing state\n";
};

State3_Processing::~State3_Processing()
{
	cout << "   distroy State3_Processing\n";
};

State* State3_Processing::executeState()
{
  delete this;
  return new State4_StatusChecking();
}

// --------------------- ---------------- ---------------- ---------------- ---------------- ----------------

State4_StatusChecking::State4_StatusChecking()
{
	cout << "   State4_StatusChecking state\n";
};

State4_StatusChecking::~State4_StatusChecking()
{
	cout << "   distroy State4_StatusChecking\n";
};

State* State4_StatusChecking::executeState()
{
  delete this;
  return new State5_Localizing();
}

// --------------------- ---------------- ---------------- ---------------- ---------------- ----------------

State5_Localizing::State5_Localizing()
{
	cout << "   State5_Localizing state\n";
};

State5_Localizing::~State5_Localizing()
{
	cout << "   distroy State5_Localizing\n";
};

State* State5_Localizing::executeState()
{
  delete this;
  return NULL;
}

// --------------------- ---------------- ---------------- ---------------- ---------------- ----------------

State6_Error::State6_Error()
{
	cout << "   State5_Localizing state\n";
}

State6_Error::~State6_Error()
{
	cout << "   distroy State5_Localizing\n";
}

State* State6_Error::executeState()
{
  delete this;
  return NULL;
}

// ----------------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------

ExplorerFSM::ExplorerFSM()
{
  currentState = new State1_Init();
  cout << "ExplorerFSM built! \n";
}
ExplorerFSM::~ExplorerFSM()
{
}

void ExplorerFSM::setCurrentState(State *s)
{
	currentState = s;
}

void ExplorerFSM::runFSM()
{
	State* temp;

	while(1) {
		temp = currentState->executeState();
		if(temp)
			setCurrentState(temp);
		else {
			printf("HO FINITO\n");
			break;
		}
	}
}
/*
int main(int argc, char** argv) {
	ExplorerFSM fsm;
	fsm.runFSM();

	return 1;
}
*/
