#ifndef WORLDKB_H_
#define WORLDKB_H_

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <vector>
#include <algorithm>
//#include <unordered_map>
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
#define CAMERA_END_ANGLE 900
/** How much degrees over 360° would be incremented during each search step. */
#define CAMERA_STEP_ANGLE 1

// ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~ ~~~~~~~~

typedef struct PatPoint {
	int x, y;
} Point2D;

/** A fact in the KB which represent a static parsed-from-a-file table of the Landmark positions in the world. */
typedef struct QRTable {
	Point2D qr_coords[MAXQR];
	string qr_label[MAXQR];
} QRTable;

/** A fact in the KB which represent a successful recognition of a QR code from the camera. */
class Landmark {

private:
	/** All info related to the QR processed which represent a landmark.*/
	QRInfos qr_info;
	/** Angle (deg) between segments AR and axis y. */
	int phi_angle;
public:
		/** Constructor knows only of the QRInfo structure. Phi angle needs to be calculated later. */
		Landmark(QRInfos qr_info);
		~Landmark();
		QRInfos getQRInfos();
		void setPhiAngle(int _phi_angle);
		int getPhiAngle();
};

/** A fact in the KB which contains all the information gathered from a pair of landmarks. */
typedef struct Triangle {
	/** REFERENCES to real Landmarks found during exploration. This is leftmost landmark of the triangle. */
	Landmark& landmarkA;
	/** REFERENCES to real Landmarks found during exploration. This is rightmost landmark of the triangle. */
	Landmark& landmarkB;
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
	/** Set of landmarks acquired during exploration. */
	vector<Landmark> landmarks;
	/** Set of triangles which can be built over landmarks. */
	Triangle* triangles;
	/** QR Static table. */
	QRTable qrTable;
	/** Static initialization of the QR Table, In the future, it will be read from a file. */
	void createStaticKB();

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
};
#endif
