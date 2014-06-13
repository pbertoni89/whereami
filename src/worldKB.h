#ifndef WORLDKB_H_
#define WORLDKB_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "quirc_internal.h"
#include "util.h"
#include "message.h"

#define MAXQR 10
/** If set to `1` means only localization2 is implemented. 
 * 	When extended to localization3 it will become equal to `3` (dispositions without ripetition) */
#define TRIANGLES 1
/** We follow the nomenclature used in our localization algorithm schema. */

#define CAMERA_INIT_ANGLE -90


typedef struct PatPoint {
	int x, y;
} Point2D;

/** A fact in the KB which represent a static parsed-from-a-file table of the Landmark positions in the world.
 * 	Each `i`-th array value is the global coordinates CVPoint of the QR which has `i` as its payload
 * 	`i` acts as an ID in this meaning. */
typedef struct QRTable {
	Point2D qr_coords[MAXQR];
} QRTable;

/** A fact in the KB which represent a successful recognition of a QR code from the camera. */
typedef struct Landmark {
	/** All info related to the QR processed which represent a landmark.*/
	QRInfos qr_info;
	/** Angle (deg) between segments AR and axis y. */
	int phi_angle;
} Landmark;

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
	Landmark* landmarks;
	/** Set of triangles which can be built over landmarks. */
	Triangle* triangles;
	/** Number of unique QR codes found and processed in the exploration. That is, actual size of landmarks.*/
	int qr_found;
	/** The mutex semaphore that bounds all thread in thread_list. */
	pthread_mutex_t mutex;

public:
	WorldKB();
	~WorldKB();
	pthread_mutex_t* getMutex();
	/** Pushes a QRInfo just processed to the worldKB */
	void pushQR(QRInfos* qr_info);
	// this will be deleted when worldKB will be really implemented.
	QRInfos fooQR;
	/** Gets the actual number of QR found. Unique way to access this number. */
	int get_qr_found();
	/** Unique external way to know what the camera angle of the robot is. */
	int getCameraAngle();
	
};
#endif
