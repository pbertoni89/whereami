#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <time.h>

#include "quirc_internal.h"
#include "util.h"

using namespace cv;
using namespace std;

const char * usage =
" \nexample command line for calibration from a live feed.\n"
"   calibration -w 9 -h 6 -o parameters.yml -op -oe\n";

const char* liveCaptureHelp =
    "The following hot-keys may be used:\n"
        "  <ESC>, 'q' - quit the program\n"
        "  'g' - start capturing images\n"
        "  'u' - switch undistortion on/off\n";
 
enum { DETECTION = 0, CAPTURING = 1, CALIBRATED = 2, QR_CALIBRATION = 3, QR_CALIBRATED = 4 };
        
void help();

static double computeReprojectionErrors(
        const vector<vector<Point3f> >& objectPoints,
        const vector<vector<Point2f> >& imagePoints,
        const vector<Mat>& rvecs, const vector<Mat>& tvecs,
        const Mat& cameraMatrix, const Mat& distCoeffs,
        vector<float>& perViewErrors );
        
static void calcChessboardCorners(Size boardSize, float squareSize, vector<Point3f>& corners);

static bool runCalibration( vector<vector<Point2f> > imagePoints,
                    Size imageSize, Size boardSize,
                    float squareSize, float aspectRatio,
                    int flags, Mat& cameraMatrix, Mat& distCoeffs,
                    vector<Mat>& rvecs, vector<Mat>& tvecs,
                    vector<float>& reprojErrs,
                    double& totalAvgErr);
                    
void saveCameraParams( const string& filename,
                       Size imageSize, Size boardSize,
                       float squareSize, float aspectRatio, int flags,
                       const Mat& cameraMatrix, const Mat& distCoeffs,
                       const vector<Mat>& rvecs, const vector<Mat>& tvecs,
                       const vector<float>& reprojErrs,
                       const vector<vector<Point2f> >& imagePoints,
                       double totalAvgErr, int scale_factor, int qr_size_mm );
                       
static bool readStringList( const string& filename, vector<string>& l );

bool runAndSave(const string& outputFilename,
                const vector<vector<Point2f> >& imagePoints,
                Size imageSize, Size boardSize, float squareSize,
                float aspectRatio, int flags, Mat& cameraMatrix,
                Mat& distCoeffs, bool writeExtrinsics, bool writePoints, int qr_size_px );
