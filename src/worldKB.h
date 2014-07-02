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

#define MAXQR 10
#define MAXSTRING 3
/** If set to `1` means only localization2 is implemented. 
 * 	When extended to localization3 it will become equal to `3` (dispositions without ripetition) */
#define TRIANGLES 1
/** We follow the nomenclature used in our localization algorithm schema. */
#define CAMERA_INIT_ANGLE -90
#define CAMERA_END_ANGLE 90
/** How much degrees over 360° would be incremented during each search step. */
#define CAMERA_STEP_ANGLE 5

// ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~

typedef struct PatPoint {
	int x, y;
} Point2D;

/** A single QR bounded by two cartesian cooordinates and labelled by an unique ID. */
typedef struct Landmark {
	Point2D qr_points;
	string qr_label;
} Landmark;

/** A fact in the KB which represent a successful recognition of a QR code from the camera. */
class RecognizedLandmark {

private:
	/** All info related to the QR processed which represent a landmark.*/
	QRInfos qr_info;
	/** Angle (deg) between segments AR and axis y. */
	int phi_angle;
public:
		/** Constructor knows only of the QRInfo structure. Phi angle needs to be calculated later. */
		RecognizedLandmark(QRInfos qr_info);
		~RecognizedLandmark();
		QRInfos getQRInfos();
		void setPhiAngle(int _phi_angle);
		int getPhiAngle();
};

/** A fact in the KB which contains all the information gathered from a pair of landmarks. */
typedef struct Triangle {
	/** REFERENCES to real Landmarks found during exploration. This is leftmost landmark of the triangle. */
	RecognizedLandmark& landmarkA;
	/** REFERENCES to real Landmarks found during exploration. This is rightmost landmark of the triangle. */
	RecognizedLandmark& landmarkB;
	/** Angle (deg) between segment AR and AB. */
	int alpha_angle;
	/** Angle (deg) between segment AB and BR. Probably unused.*/
	int beta_angle;
	/** Angle (deg) between segment AB and axis x. */
	int theta_angle;
	/** Angle (deg) between segment AR and BR. */
	int gamma_angle;
	/** Proposed global cartesian coordinates where the robot should be. */
	Point2D robot_coords;
} Triangle;

// ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~

/** this class should represent the whole knowledge base the Explore agent has on the world. 
 * It is the resource under race condition: the Explore has to WRITE on it when it get some relevant 
 * information on the world where it is (single QRs, localization proposals) 
 * and the Server has to READ it when a client requires informations on the agent. */
class WorldKB {

private:
	/** RELATIVE rotation of the robot camera with respect to INITIAL angle.
	 * E.g., if camera starts from -90deg, and has turned right till 0deg, camera_angle will be 90deg.*/
	int camera_angle;
	/** Set of recognized landmarks acquired during exploration. */
	vector<RecognizedLandmark> dynamicKB;
	/** Set of triangles which can be built over landmarks. */
	Triangle* triangles;
	/** A set of facts in the KB which represent a static parsed-from-a-file table of the Landmark positions in the world. */
	vector<Landmark> staticKB;
	/** Static initialization of the QR Table, In the future, it will be read from a file. */
	void parseStaticKB(string filename);

public:
	WorldKB();
	~WorldKB();
	/** Pushes a QRInfo just processed to the worldKB */
	void pushQR(QRInfos* qr_info);
	/** Gets the actual number of QR found. Unique way to access this number. */
	int get_qr_found();
	/** Unique external way to know the camera angle of the robot. */
	int getCameraAngle();
	/** Unique external way to set the camera angle of the robot. */
	void setCameraAngle(int _camera_angle);
	/** Unique external way to increment buy a fixed quantity the camera angle of the robot. */
	void incrementCameraAngle();
	/** Checks whether QR `label` has already been recognized. */
	bool isQRInDynamicKB(string label);
	/** Checks whether QR `label` is known in the static table. */
	bool isQRInStaticKB(string label);
	/** Prints out static KB facts. */
	void printStaticKB();
};
#endif
