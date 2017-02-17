// ComputerVision_0.1.cpp : Defines the entry point for the console application. To test stereoscopic vision using open source software (openCV)
//							This program should define a basic principle of the functionality of two cameras to find distance of any object. To
//							perform this task, a stictched up map displaying distances of every pixle will be generated that can then be scanned
//							for particular patterns.
//
// Version 0.1
// Christian Aguilar

#include <iostream>
	//#include "stdafx.h"
#include <stdio.h>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/ximgproc.hpp"


using namespace cv;
using namespace std;
using namespace ximgproc;

static float getMaxDisparity( VideoCapture& capture ) {
		//TODO: Calculate maxDisp
	float maxDisp;



	return maxDisp;
}

const int cameraCount = 0; //set to 0 to use images
string leftImagePath = "testImages/chair_left.jpg";
string rightImagePath = "testImages/chair_right.jpg";


int main(int, char**)
{
	VideoCapture cam[cameraCount];

	for (int i = 0; i < cameraCount; i++) {
		cam[i] = VideoCapture(i);

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


	float maxDisp = getMaxDisparity(camL);
	maxDisp = 48;
	int max_disp =  roundf(maxDisp);

	max_disp/=2;
	if(max_disp%16!=0)
		max_disp += 16-(max_disp%16);

	Ptr<StereoBM> sbm = StereoBM::create(max_disp, 21);

		// code from example: http://docs.opencv.org/trunk/d3/d14/tutorial_ximgproc_disparity_filtering.html
	Ptr<DisparityWLSFilter> wls_filter = createDisparityWLSFilter(sbm);
	Ptr<StereoMatcher> right_matcher = createRightMatcher(sbm);


	Mat edges,disparity, colorized, imageL, imageR, dispL, dispR, filteredDisp, dispVisRaw, dispVisFiltered;

		//Filter Parameters
		//http://docs.opencv.org/trunk/d9/d51/classcv_1_1ximgproc_1_1DisparityWLSFilter.html
	double lambda = 8000, sigma = 2.0;

	for (;;)
	{

			//for speed and ensuring time difference is minimal grab both frames
		camL.grab();
		camR.grab();

		if (cameraCount != 0) {
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


			// Match Images
		double matching_time = (double)getTickCount();
		sbm-> compute(imageL, imageR, dispL);
		right_matcher->compute(imageR, imageL, dispR);
		matching_time = ((double)getTickCount() - matching_time)/getTickFrequency();


		wls_filter->setLambda(lambda);
		wls_filter->setSigmaColor(sigma);
		double filtering_time = (double)getTickCount();
		wls_filter->filter(dispL,imageL,filteredDisp,dispR);
		filtering_time = ((double)getTickCount() - filtering_time)/getTickFrequency();


			//GaussianBlur(imageL, edges, Size(3, 3), 1.5, 1.5); //Reduce noise
			//Canny(edges, edges, 10, 70, 3); //Find edges and ommit everything else

		//show both raw and edges
		imshow("RawL", imageL);
		imshow("RawR", imageR);
		getDisparityVis(dispL, dispVisRaw);
		imshow("raw disparity", dispVisRaw);
		getDisparityVis(filteredDisp, dispVisFiltered);
		imshow("filtered disparity", dispVisFiltered);

		if (waitKey(30) >= 'q') break; //quit when 'q' is pressed
	}
	// the camera will be deinitialized automatically in VideoCapture destructor
	return 0;
}

