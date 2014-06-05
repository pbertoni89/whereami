#ifndef EXPLORER_H_
#define EXPLORER_H_

#include "threadonmutex.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>
using namespace cv;

#define EXPLORERTHREADINDEX 1
#define THRESH 13 	 // mysterious magic number.
#define LBOUND 135	 // minimum angle of vertical rotation allowed. set to 0 to disable.
#define UBOUND 225	 // maximum angle of vertical rotation allowed. set to 359 to disable.
#define CAMERA_INIT_ANGLE -90
#define DEBUG		 // it will help us. Comment for excluding preprocessing.

class Explorer : public ThreadOnMutex {

private:
	/** ID of the camera acquiring video. */
	int camera_id;
	/** OpenCV object describing a video capture. */
	VideoCapture capture;
	/** Temporary QRInfos structure describing the CURRENT QR to be processed and sent to worldKB. */
	QRInfos temp_qr_info;
	/** Structure strongly bounded with temp_qr_info. */
	struct quirc* q;
	/** Structure strongly bounded with temp_qr_info. */
	struct quirc_code* code;
	/** Structure strongly bounded with temp_qr_info. */
	struct quirc_data* data;
	/** Scaling factor acquired from calibration program output. */
	int scale_factor;
	/** Size (in millimetres) of the QR code pattern, acquired from calibration program output. */
	int qr_size_mm;
	/** OpenCV matrices, acquired from calibration program output. */
	Mat intrinsic_matrix, distortion_coeffs;
	/** RELATIVE rotation of the robot camera with respect to INITIAL angle.
	 * E.g., if camera starts from -90deg, and has turned right till 0deg, camera_angle will be 90deg.*/
	int camera_angle;

	void extrapolate_qr_informations();
	int processQR();
	int scaleQR(double side);
	void copyCorners();
	void calcPerspective_Distance(double side_a, double side_b);
	void calcCenter_VerticalRot();
	void copyPayload();
	void printQRInfo(); //will be deleted
	/** Used every time FSM wants to start with a new QR. */
	void resetQR();

public:
	Explorer(WorldKB& _worldKB, int _camera_id);
	~Explorer();
	virtual void* threadBehaviour();
	static void* call_threadBehaviour(void * arg);
	/** Unique external way to know what the camera angle of the robot is. */
	int getCameraAngle();
};
#endif
