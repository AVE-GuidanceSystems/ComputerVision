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

#include <numeric>
#include <stdlib.h>

#include <opencv2/opencv.hpp>
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/objdetect/objdetect.hpp>
//#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

const int cameraCount = 2; //set to 0 to use images
string testImageDirectory = "../testImages/";
string leftImagePath = testImageDirectory + "chair_left.jpg";
string rightImagePath = testImageDirectory + "chair_right.jpg";

int main(void)
{
	int camNum[cameraCount] = {0,2};
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

		//StereoBM_GPU sbgpu;

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




			cvtColor(imageL, imageL, CV_BGR2GRAY); //to Gray image
			cvtColor(imageR, imageR, CV_BGR2GRAY); //to Gray image


				// Match Images - Move to function?
			double matching_time = (double)getTickCount();
			sbm(imageL, imageR, disp);
			disp.convertTo(disp8, CV_8U);
			matching_time = ((double)getTickCount() - matching_time)/getTickFrequency();

			double filtering_time = (double)getTickCount();
			filtering_time = ((double)getTickCount() - filtering_time)/getTickFrequency();

			//show both raw and edges
			imshow("RawL", imageL);
			imshow("RawR", imageR);
			imshow("raw disparity", disp8);

			if (waitKey(30) >= 'q') break; //quit when 'q' is pressed
		}
		// the camera will be deinitialized automatically in VideoCapture destructor
		return 0;
}
