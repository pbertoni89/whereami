#ifndef EXPLORERFSM_H
#define EXPLORERFSM_H

#include "worldKB.h"

#include <time.h>
#include <unistd.h>
#include <thread>
#include <signal.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <math.h>

#include <iterator>

using namespace cv;

#define DEBUG		 // it will help us. Comment for excluding preprocessing.
#define THRESH 13 	 // mysterious magic number.
#define PI 3.14159265
//#define TESTCORE

/** Structure to easily handle QR data which is strictly related. */
typedef struct QRStuff {
	/** Temporary QRInfos structure describing the CURRENT QR to be processed and sent to worldKB. */
	QRInfos qr_info;
	/** Structure strongly bounded with temp_qr_info. */
	struct quirc* q;
	/** Structure strongly bounded with temp_qr_info. */
	struct quirc_code code;
	/** Structure strongly bounded with temp_qr_info. */
	struct quirc_data data;
} QRStuff;


class State
{
private:
	WorldKB* worldKB;
public:
	State(WorldKB* _worldKB);
	virtual ~State(void);
	virtual State* executeState(void)=0;
	WorldKB* getWorldKB();
	void setWorldKB(WorldKB* kb);
	/** Calls `morgulservo -- currentCameraAngle && sleep sleeptime`*/
	void morgulservo_wrapper(float sleeptime);
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
	/** Frame size, used while centering QR in frame view. */
	int frameCols;
	/** Maximum absolute number of pixels measuring distance from QR center and Frame center. */
	int centering_tolerance;
	/** OpenCV object for camera handle. */
	VideoCapture capture;
	/** Temporary structure for QR processing. */
	QRStuff qrStuff;
	/** Scaling factor acquired from calibration program output.*/
	int scale_factor;
	/** Size (in millimetres) of the QR code pattern, acquired from calibration program output.*/
	int qr_size_mm;
	/** OpenCV matrices, acquired from calibration program output.*/
	Mat intrinsic_matrix, distortion_coeffs;
	/** Mutex semaphore to synchronize */
	pthread_mutex_t mutex;
	/** Concurrent variable, protected by `mutex`. IFF true, searching can be called and moveCamera cannot; VICEVERSA IFF false.*/
	bool turnSearching;
	/** Takes values between ROUGH=0 and FINE=1 */
	bool mode;

	/** @return TRUE IFF mode==FINE. */
	bool isFine();
	void setRough();
	void setFine();
	bool isRough();

	void parseParameters(string filename);
	int scaleQR(double side);
	void copyCorners();
	void saveSnapshot(Mat frame);
	/** Calculate perspective rotation and distance of the QR. ---------------------------------------*/
	void calcPerspective_Distance(double side_a, double side_b);
	bool isQrCentered();
	/** @return difference between qr center AND frame center. */
	int calcDeltaCenter();
	/** @return TRUE IFF qr center is RIGHTmost respect to frame center. */
	bool isQrRX();
	/** @return TRUE IFF qr center is LEFTmost respect to frame center. */
	bool isQrLX();
	/** Copies payload from data to info structure. --------------------------------------------------*/
	int copyPayload();
	void resetQR();
	/** First of two top-view methods of State2.
	 * @param `snapshot` lets frame capture available for further analysis.
	 * @return a QRInfo iff a QR is found; else returns NULL*/
	bool searching(bool snapshot);
	/** Internal method used by searching. ENSURES THAT 1) QR is in KB facts 2) QR is centered enough
	 *   @return TRUE IFF the preprocessed qr will be pushed in KB; i.e. isCentered && isKnown && !isRecognized */
	bool preProcessing();
	/** Second of two top-view methods of State2. */
	void processing();

	bool moveCamera()
	{
		if (this->getWorldKB()->isInRange()) {
			if(!turnSearching) {
				//cout << "is moveCamera turn." << endl;
				while(pthread_mutex_lock(&mutex)   != 0);
					this->getWorldKB()->incrementCameraAngle(getWorldKB()->getpStepAngle(mode));	// CRITICAL REGION
					stringstream comando;
					comando << endl << endl << "morgulservo -- " << this->getWorldKB()->getCameraAngle();
					cout << comando.str() << endl;
					int ret = system(comando.str().c_str());
					sleep(this->getWorldKB()->getpStepSleep(this->mode));
					turnSearching = true;										// CRITICAL REGION
				while(pthread_mutex_unlock(&mutex)   != 0);
			}
			return true;
		}
		return false;
	}

	static void* moveCamera_wrapper(void* arg)
	{
		while(true)
			if (! ((State2_QR*) arg)->moveCamera())
				break;
		return 0;
	}

public:
	State2_QR(WorldKB* _worldKB);
	~State2_QR();
	State* executeState();
};

class State3_Checking: public State
{
  public:
	State3_Checking(WorldKB* _worldKB);
	~State3_Checking();
	State* executeState();
};

/** A fact in the KB which contains all the information gathered from a pair of landmarks. */
class Triangle {
	/** POINTER to real Landmarks found during exploration. This is leftmost landmark of the triangle. */
	Landmark* lmA;
	/** POINTER to real Landmarks found during exploration. This is rightmost landmark of the triangle. */
	Landmark* lmB;
	/** Angle (deg) between segment AR and AB. */
	double alpha_angle;
	/** Angle (deg) between segment AB and BR. Probably unused.*/
	double beta_angle;
	/** Angle (deg) between segment AB and axis x. */
	double theta_angle;
	/** Angle (deg) between segment AR and BR. */
	double gamma_angle;
	/** Angle (deg) between segment AR and axis y. */
	double phi_angle;
	/** Proposed global cartesian coordinates where the robot should be. */
	Point2D robot_coords;
public:
	Triangle(Landmark* _lA, Landmark* _lB);
	/** Core of the triangulation algorithm. */
	void triangulation();
	/** Formatted output for this class. */
	void print();
	/** One-line essential output. */
	void printShort();
	/** get robot coords */
	Point2D get_robot_coords();
};

class State4_Localizing: public State
{
	/** Set of triangles which can be built over landmarks. */
	vector<Point2D> solutions;
	/** Basic wrapper for triangulation algorithm. Simply uses the two first landmarks in KB to triangulate. */
	Triangle basicLocalization(int n1, int n2);
	/** Test wrapper for triangulation algorithm. */
	void testLocalization();

  public:
	State4_Localizing(WorldKB* _worldKB);
	~State4_Localizing();
	State* executeState();
	/** Iterates through this->triangles to print all the robot coords suggested. */
	void printAllRobotCoords();
};

class State5_Error: public State
{
  public:
	State5_Error(WorldKB* _worldKB);
	~State5_Error();
	State* executeState();
};

// ~~~~~~~~~~~~~ ~~~~~~~~~~~~~ ~~~~~~~~~~~~~ ~~~~~~~~~~~~~ ~~~~~~~~~~~~~ ~~~~~~~~~~~~~ ~~~~~~~~~~~~~ ~~~~~~~~~~~~~

class ExplorerFSM {

public:
	ExplorerFSM();
	~ExplorerFSM();
	void* runFSM();

private:
	State* currentState;
	void setCurrentState(State*);
	/** the REAL NEW WORLDKB: an OBJECT possessed by ExplorerFSM. */
	WorldKB worldKB;
};

#endif
