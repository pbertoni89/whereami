#include <iostream>
#include <stdio.h>
#include <stdlib.h>

class Wrapper {

public:
	class State {
		private:

		public:
			State();
			virtual State* exec()=0;

	};

	class Init : public State {
	  public:
		int* pcapture;
		Init(int* _pcapture);
		State* exec();
	};

	Wrapper();
	void startFSM();
	void set(State* t);
	//int getVariabile();

private:
	State* s;
	int var;

};
