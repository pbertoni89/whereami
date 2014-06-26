#ifndef EXPLORERFSM_H
#define EXPLORERFSM_H
#include <iostream>
using namespace std;

#include "worldKB.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
using namespace cv;

#define THRESH 13 	 // mysterious magic number.
#define LBOUND 135	 // minimum angle of vertical rotation allowed. set to 0 to disable.
#define UBOUND 225	 // maximum angle of vertical rotation allowed. set to 359 to disable.

#define DEBUG		 // it will help us. Comment for excluding preprocessing.

typedef struct QRStuff {
	/** Temporary QRInfos structure describing the CURRENT QR to be processed and sent to worldKB. */
	QRInfos temp_qr_info;
	/** Structure strongly bounded with temp_qr_info. */
	struct quirc* q;
	/** Structure strongly bounded with temp_qr_info. */
	struct quirc_code* code;
	/** Structure strongly bounded with temp_qr_info. */
	struct quirc_data* data;
} QRStuff;

class State
{
private:
	WorldKB* worldKB;
public:
	State(WorldKB* _worldKB);
	~State(void);
	virtual State* executeState(void)=0;
	WorldKB* getWorldKB();
	void setWorldKB(WorldKB* kb);
};

/** TURNS THE CAMERA TO INITIAL ANGLE. ID EST, LETS A SYSCALL DO THIS STUFF */
class State1_Init: public State
{
public:
	State1_Init(WorldKB* _worldKB);
	~State1_Init(void);
	State* executeState(void);
};

class State2_QR: public State
{
private:
	int camera_id;
	/** Frame size, used while centering QR in frame view. */
	int frameCols;
	/** Maximum absolute number of pixels measuring distance from QR center and Frame center. */
	int centering_tolerance;
	VideoCapture capture;
	int camera_angle;
	/** Alcune caratteristiche del QR (qua in fase di PREPROCESSING vengono scritte qua;
	 * la struttura verrà poi passata allo stato di PROCESSING per il calcolo della distanza.
	 * il processing la pusherà poi in worldKB, dove verrà COPIATA, e le strutture temporanee saranno pulite. */
	QRStuff qrStuff;
	/** Scaling factor acquired from calibration program output. */
	int scale_factor;
	/** Size (in millimetres) of the QR code pattern, acquired from calibration program output. */
	int qr_size_mm;
	/** OpenCV matrices, acquired from calibration program output. */
	Mat intrinsic_matrix, distortion_coeffs;
	int scaleQR(double side);
	void copyCorners();
	void calcPerspective_Distance(double side_a, double side_b);
	bool isCentered();
	void copyPayload();
	void printQRInfo(); //will be deleted
	void resetQR();
	/** First of two top-view methods of State2. Return a QRInfo iff a QR is found; else returns NULL*/
	bool searching();
	/** Internal method used by searching. ENSURES THAT 1) QR is in KB facts 2) QR is centered enough */
	bool preProcessing(Mat frame_BW);
	/** Second of two top-view methods of State2. Return true iff QR is correctly pushed into worldKB */
	bool processing();

public:
	State2_QR(WorldKB* _worldKB);
	~State2_QR();
	State* executeState();
};


class State3_StatusChecking: public State
{
  public:
	State3_StatusChecking(WorldKB* _worldKB);
	~State3_StatusChecking();
	State* executeState();
};

class State4_Localizing: public State
{
  public:
	State4_Localizing(WorldKB* _worldKB);
	~State4_Localizing();
	State* executeState();
};

class State5_Error: public State
{
  public:
	State5_Error(WorldKB* _worldKB);
	~State5_Error();
	State* executeState();
};

// --------------------------------------------------------------------------------------------------------------------------------------

class ExplorerFSM {

public:
	ExplorerFSM(int _camera_id);
	~ExplorerFSM();
	void* runFSM();

private:
	/** ID of the camera acquiring video. */
	int camera_id;
	State* currentState;
	void setCurrentState(State*);
	/** the REAL NEW WORLDKB: an OBJECT possessed by ExplorerFSM. */
	WorldKB worldKB;
};

#endif
