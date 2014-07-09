#ifndef WORLDKB_H_
#define WORLDKB_H_

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

#include <vector>
#include <algorithm>
#include <iterator>

#include "quirc_internal.h"
#include "util.h"
#include "message.h"

using namespace std;

//#define ECLIPSE // keep this line IF whereami is launched WITHIN eclipse environment, i.e. eclipse workspace directory.

#ifdef ECLIPSE
	#define PARAM_FILE "/home/patrizio/Desktop/progettoQR/whereami/parameters.txt"
	#define CALIB_FILE "/home/patrizio/Desktop/progettoQR/whereami/out_camera_data.yml"
	#define KB_FILE    "/home/patrizio/Desktop/progettoQR/whereami/staticKB.txt"
#else
	#define PARAM_FILE "parameters.txt"
	#define CALIB_FILE "out_camera_data.yml"
	#define KB_FILE    "staticKB.txt"
#endif

// ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~

typedef struct Point2D {
	double x, y;
} Point2D;

/** A fact in the KB which represent a known position in the world, marked by a QR code. */
class Landmark {
	/** Cartesian 2D coordinates. */
	Point2D coords;
	/** Unique ID as QR's payload. */
	string label;
	/** Distance (mm) from camera at acquiring time. */
	double distance;
	/** Camera angle (deg) at acquiring time. */
	double delta_angle;
	/** True iff Landmark is in DynamicKB; false iff Landmark is only in StaticKB. */
	bool recognized;
public:
	Landmark(Point2D _qr_points, string _qr_label);
	Landmark();
	~Landmark();
	void recognize(double _distance, double _delta_angle);
	bool isRecognized();
	Point2D getCoords();
	double getX();
	double getY();
	string getLabel();
	double getDistance();
	double getDeltaAngle();
	/** Formatted output for Landmark data structure.*/
	void print();
};

// ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~

/** this class should represent the whole knowledge base the Explore agent has on the world. 
 * It is the resource under race condition: the Explore has to WRITE on it when it get some relevant 
 * information on the world where it is (single QRs, localization proposals) 
 * and the Server has to READ it when a client requires informations on the agent. */
class WorldKB {

private:
	/** Old macros, now parsed from a config file. */
	/** maximum abs diff between QR center and vertical center of the frame. */
	/** because initial movement may be very disrupting (at max 180 deg!) */
	/** because step movement should be very little (usually 1-2 deg) */
	int p_cameraID, p_centerTolerance, p_startAngle, p_endAngle;
	int p_stepAngle_rough, p_stepAngle_fine;
	int p_Ntry_rough, p_Ntry_fine;
	float p_stepSleep_rough, p_stepSleep_fine;
	float p_startSleep;
	/** RELATIVE rotation of the robot camera with respect to INITIAL angle.
	 * E.g., if camera starts from -90deg, and has turned right till 0deg, cameraAngle will be 90deg.*/
	int cameraAngle;
	/** Set of recognized landmarks acquired during exploration. */
	vector<Landmark> kb;
	/** Static initialization of the QR Table, In the future, it will be read from a file. */
	void parseStaticKB(string filename);
	void parseParameters(string filename);
public:
	WorldKB();
	//~WorldKB();
	/** Pushes a QRInfo just processed to the worldKB */
	void pushQR(string _label, double _distance, double _acquired_angle);
	/** Gets the actual number of QR found. Unique way to access this number. */
	int getRecognizedQRs();
	/** Unique external way to know the camera angle of the robot. */
	int getCameraAngle();
	/** Unique external way to set the camera angle of the robot. */
	void setCameraAngle(int _camera_angle);
	/** Unique external way to increment buy a fixed quantity the camera angle of the robot. */
	void incrementCameraAngle();
	/** Checks whether QR `label` is known in the QR table. Boolean pointer serves as a second output for the question `has QR yet been recognized?` */
	bool isQRInKB(string label, bool* isRecognized);
	/** Prints out static KB facts. If QR are recognized, additional informations will be printed out. */
	void printKB();
	/** Return a copy of the KB. */
	vector<Landmark> getKB();
	/** PRECONDITION: QR labelled `label~` really exists in KB.
	 * So, this method should be called only after a succesful isQRinKB call
	 * If a QR labelled by `label` is in KB, its relative Landmark will be returned. Else, NULL will be returned. */
	Landmark* getLandmark(string label);
	/** If the (pos+1)-th QR is in KB, AND 0 <= pos < kb.size(), its relative Landmark will be returned. Else, NULL will be returned. */
	Landmark* getLandmark(unsigned int pos);
	/** Parameters getters. */
	int getpCameraID();
	int getpStartAngle();
	int getpEndAngle();
	float getpStartSleep();
	int getpCenterTolerance();
	int getpStepAngle(bool mode);
	int getpNtry(bool mode);
	float getpStepSleep(bool mode);
	/** Return TRUE IFF startAngle <= cameraAngle <= endAngle */
	bool isInRange();
	/** Fills the `distance` and `delta_angle` fields of the first two landmarks. */
	void triangleTest();
};
#endif
