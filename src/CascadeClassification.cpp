/*
 ============================================================================
 Name        : ComputerVision.cu
 Author      : Justin Oroz & Christian Aguilar
 Version     :
 Copyright   : POSTED
 Description : CUDA compute reciprocals
 ============================================================================
 */

#include <iostream>
#include <stdio.h>
#include <string>
#include "string_helper.hpp"

#include <numeric>
#include <stdlib.h>
#include <time.h>

#include <opencv2/calib3d.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "string_helper.hpp"

using namespace cv;
using namespace std;

/** Function Headers */
void detectAndDisplay( Mat frame );

/** Global variables */
const int cameraCount = 2; //set to 0 to use images
string testImageDirectory = "../testImages/";
string leftImagePath = testImageDirectory + "chair_left.jpg";
string rightImagePath = testImageDirectory + "chair_right.jpg";
String face_cascade_name = "/usr/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml";
String fullbody_cascade_name = "/usr/share/OpenCV/haarcascades/haarcascade_fullbody.xml";
CascadeClassifier face_cascade;
CascadeClassifier fullbody_cascade;
string window_name = "Capture - Face detection";

int main(void)
{
//-- 1. Load the cascades
	if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
	if( !fullbody_cascade.load( fullbody_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };

	cout << "testing" << endl;
	int camNum[cameraCount] = {1,2};
	VideoCapture cam[cameraCount];

		for (int i = 0; i < cameraCount; i++) {
			cam[i] = VideoCapture(camNum[i]);

			// set video capture properties for camera, forcing lower res
			cam[i].set(CV_CAP_PROP_FRAME_WIDTH, 640);
			cam[i].set(CV_CAP_PROP_FRAME_HEIGHT, 480);
		}

		VideoCapture camL, camR;
		if (cameraCount != 0) {
			camL = cam[1]; // open Camera #1
			camR = cam[0];// open Camera #2

			if( !camL.isOpened() || !camR.isOpened() )
			{
				cerr << "***Could not initialize capturing...***\n";
				cerr << "Current parameter's value: \n";
				return -1;
			}
		}

		int maxDisp = 48; // TODO: - Max Disparity calculations
		maxDisp/=2;
		if(maxDisp%16!=0)
			maxDisp += 16-(maxDisp%16);

		StereoBM sbm = StereoBM(CV_STEREO_BM_BASIC, maxDisp, 21); // TODO: - Modify block size

		sbm.state->SADWindowSize = 9;
		sbm.state->numberOfDisparities = 112;
		sbm.state->preFilterSize = 5;
		sbm.state->preFilterCap = 61;
		sbm.state->minDisparity = -39;
		sbm.state->textureThreshold = 507;
		sbm.state->uniquenessRatio = 0;
		sbm.state->speckleWindowSize = 0;
		sbm.state->speckleRange = 8;
		sbm.state->disp12MaxDiff = 1;

			// code from example: http://docs.opencv.org/trunk/d3/d14/tutorial_ximgproc_disparity_filtering.html

		Mat imageL, imageR, disp, disp8, filteredDisp, dispVisRaw, dispVisFiltered;

			//Filter Parameters
			//http://docs.opencv.org/trunk/d9/d51/classcv_1_1ximgproc_1_1DisparityWLSFilter.html

		for (;;)
		{
			double frame_timer = (double)getTickCount();

			if (cameraCount != 0) {

					//for speed and ensuring time difference is minimal grab both frames
				camL.grab();
				camR.grab();

					//retreive after both captures
				if ( !camL.retrieve(imageL)) {
					cerr << "Left Frame failed to grab a frame" << endl;
				}
				if ( !camR.retrieve(imageR)) {
					cerr << "Right Frame failed to grab a frame" << endl;
				}
			} else { // pull from images
				imageL  = imread(leftImagePath ,IMREAD_COLOR);
				if ( imageL.empty() )
				{
					cerr<<"Cannot read image file: "<< rightImagePath;
					return -1;
				}
				imageR = imread(rightImagePath,IMREAD_COLOR);
				if ( imageR.empty() )
				{
					cerr<<"Cannot read right image file: "<< rightImagePath;
					return -1;
				}


					//resize(imageL ,imageL ,Size(),0.5,0.5);
					//resize(imageR,imageR,Size(),0.5,0.5);
			}


			//TODO stereoRectifyUncalibrated();
			detectAndDisplay(imageL); // detect faces and bodies based on left camera

			cvtColor(imageL, imageL, CV_BGR2GRAY); //to Gray image
			cvtColor(imageR, imageR, CV_BGR2GRAY); //to Gray image


				// Match Images - Move to function?
			double matching_time = (double)getTickCount();
			sbm(imageL, imageR, disp);

			filterSpeckles(disp, 0, 10, 5);

			disp.convertTo(disp8, CV_8U);
			matching_time = ((double)getTickCount() - matching_time)/getTickFrequency();

			double filtering_time = (double)getTickCount();
			filtering_time = ((double)getTickCount() - filtering_time)/getTickFrequency();

			frame_timer = ((double)getTickCount() - frame_timer)/getTickFrequency();

			double fps = 1/frame_timer;

			putText(disp8, stringify(fps), cvPoint(30,30),
			    FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255,0,0), 1, CV_AA);

			//show both raw and edges
			imshow("RawL", imageL);
			imshow("RawR", imageR);
			imshow("Raw Disparity", disp8);

			if (waitKey(30) == 'q') break; //quit when 'q' is pressed
		}
		// the camera will be deinitialized automatically in VideoCapture destructor
		return 0;
}


/*int main( int argc, const char** argv )
{
CvCapture* capture;
Mat frame;


//-- 2. Read the video stream
capture = cvCaptureFromCAM( 0 ); //using external usb camera .. "-1" is default
if( capture )
{
 while( true )
 {
frame = cvQueryFrame( capture );

//-- 3. Apply the classifier to the frame
   if( !frame.empty() )
   { detectAndDisplay( frame ); }
   else
   { printf(" --(!) No captured frame -- Break!"); break; }

   int c = waitKey(10);
   if( (char)c == 'c' ) { break; }
  }
}
return 0;
}*/

/** @function detectAndDisplay */
void detectAndDisplay( Mat frame )
{
Mat frame_gray;
clock_t frame_timer = clock(); //begin time counter for FPS

cvtColor( frame, frame_gray, CV_BGR2GRAY );
equalizeHist( frame_gray, frame_gray );

//-- Detect faces
std::vector<Rect> faces;
face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );

for( size_t i = 0; i < faces.size(); i++ ){
	Point face_center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
	ellipse( frame, face_center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 0 ),4);
	Mat faceROI = frame_gray( faces[i] );
}

//-- Full Body Detection
/*std::vector<Rect> body;
fullbody_cascade.detectMultiScale( frame_gray, body, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30) );


for(size_t i = 0; i<body.size(); i++){
	Point p1(body[i].x, body[i].y ); //origin point
	Point p2(body[i].x + body[i].height, body[i].y + body[i].width);//Opposite corner
	cv::rectangle( frame, p1, p2, Scalar( 0, 255, 0 ));

	Mat faceROI = frame_gray( body[i] );
}
*/

//-- Visual Display w/ FPS

double frame_period = (clock() - frame_timer); //Displaced time period
//cout << frame_period << " divided by " << CLOCKS_PER_SEC << " = " << frame_period/CLOCKS_PER_SEC << endl;
double fps = 1/(frame_period/CLOCKS_PER_SEC); //convert to frequency
putText(frame, stringify(fps), cvPoint(30,30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255,0,0), 1, CV_AA);


imshow( window_name, frame );
}
