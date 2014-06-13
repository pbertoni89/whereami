#include "prova.h"

Wrapper::State::State()
{
	;
}
Wrapper::State* Wrapper::State::exec()
{
	return NULL;
}
void Wrapper::set(State* t)
{
	this->s = t;
}
//-------------------------------
Wrapper::Init::Init(int* _pcapture) : State()
{
	pcapture = _pcapture;
	std::cout << "constructing an Init. this is variabile value: " << *this->pcapture << std::endl;
}

Wrapper::State* Wrapper::Init::exec()
{
	int num = 22;
  return new Init(&num);
}
//-------------------------------
Wrapper::Wrapper() {
	//this->s = new State(); non posso allocare un oggetto astratto!!!
	int num = 11;
	this->s = new Init(&num);
}
void Wrapper::startFSM()
{
	State* tmp = this->s->exec();
	this->set(tmp);
}

//-------------------------------
int main(int argc, char** argv)
{
	Wrapper w = Wrapper();
	w.startFSM();
	return 0;
}
