#include "worldKB.h"
#include "explorerFSM.h"

/** MAIN function. -------------------------------------------------------------------------------*/
int main(int argc, char **argv){
	ExplorerFSM explorerFSM = ExplorerFSM();
	explorerFSM.runFSM();
	return 0;
}
