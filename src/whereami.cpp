#include "worldKB.h"
#include "explorerFSM.h"
#include "server.h"

int _camera_id = 0;

/** Prints help. ---------------------------------------------------------------------------------*/
void help() {
    printf( "QRlocate codes inspector.\n"
        "Usage: inspect\n"
        "\t[-id <camera_id>]\t# the camera ID for capture opening. default is 0\n"
        "\n" );
}

/** Get argv parameters. -------------------------------------------------------------------------*/
int getOpt(int cargc, char** cargv) {

  if(cargc < 1){
    help();
    exit(1);
  }

  for(int i=1; i<cargc; i++)
    {
        const char* s = cargv[i];
        if( (s[0] != '-') && isdigit(s[0]) )
        {
			sscanf(s, "%d", &_camera_id);
        }
        else
            return fprintf( stderr, "Unknown option %s", s ), -1;
    }	
    return 0;	
}

/** MAIN function. -------------------------------------------------------------------------------*/
int main(int argc, char **argv){
	
	/*if (getOpt(argc, argv) != 0)
		return -1;
	  printf("aaa");*/

	_camera_id = 0; printf("aaa1 %d", _camera_id);
	WorldKB worldKB = WorldKB(); printf("aaa2 %d", _camera_id);

	//Server server = Server(worldKB); printf("aaa3 %d", _camera_id);

	ExplorerFSM explorerFSM = ExplorerFSM(worldKB, _camera_id); printf("aaa4 %d", _camera_id);
	explorerFSM.runFSM();

	pthread_exit(NULL);
	return 55;
}
