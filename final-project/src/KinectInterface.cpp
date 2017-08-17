#include <iostream>
#include <vector>
#include <cmath>
#include "libfreenect.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "MyFreenectDevice.h"


static int DepthLowerRange = 25;
static int DepthUpperRange = 80;
static const int DepthHighLimit = 300;

static const char KinectDepthTrackbarWindowName[] = "Kinect Depth Detection Parameters";

static const char KinectDepthTrackbarMinDepthName[] = "Min Depth";
static const char KinectDepthTrackbarMaxDepthName[] = "Max Depth";

static MyFreenectDevice *pDevice;
static Freenect::Freenect *pFreenect;

static cv::Mat DepthFMat(cv::Size(640,480), CV_8UC1);
static cv::Mat DepthMat(cv::Size(640, 480), CV_16UC1);


static void KinectDepthDetectionCreateTrackbarWindow(void (*pCallbackFunction)(int, void *))
{
	cv::namedWindow(KinectDepthTrackbarWindowName, CV_WINDOW_AUTOSIZE);

	cv::createTrackbar(KinectDepthTrackbarMinDepthName, KinectDepthTrackbarWindowName, &DepthLowerRange, DepthHighLimit, pCallbackFunction, NULL);
	cv::createTrackbar(KinectDepthTrackbarMaxDepthName, KinectDepthTrackbarWindowName, &DepthUpperRange, DepthHighLimit, pCallbackFunction, NULL);
}


static void KinectDepthDetectDestroyTrackbarWindow(void)
{
	cv::destroyWindow(KinectDepthTrackbarWindowName);
}


void KinectStart(void)
{
	// The next two lines must be changed as Freenect::Freenect
	// isn't a template but the method createDevice:
	// Freenect::Freenect<MyFreenectDevice> freenect;
	// MyFreenectDevice& device = freenect.createDevice(0);
	// by these two lines:
	//	Freenect::Freenect freenect;
	//	MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);
	pFreenect = new Freenect::Freenect;
	MyFreenectDevice &device = pFreenect->createDevice<MyFreenectDevice>(0);
	pDevice = &device;

	// This might have been the functionality we have been looking for...
	// This sets the depth image format to be in "registered" mode, which
	// in libfreenect.h line 104 indicates "processed depth data in mm, aligned to 640x480 RGB"
	// Not sure how well aligned it is, but it does appear to be much, much closer to being aligned
	pDevice->setDepthFormat(FREENECT_DEPTH_REGISTERED);

	pDevice->startVideo();
	pDevice->startDepth();

	KinectDepthDetectionCreateTrackbarWindow(NULL);
}


void KinectGetRGBandRangedDepthImages(cv::Mat &RGB, cv::Mat &RangedDepth)
{
	if(NULL != pDevice)
	{
		pDevice->getVideo(RGB);
		pDevice->getDepth(DepthMat);

		// This might need some adjustment after setting the depth format to FREENECT_DEPTH_REGISTERED
		DepthMat.convertTo(DepthFMat, CV_8UC1, 255.0/2048.0);

		// show objects within given range as white, everything else as black
		cv::inRange(DepthFMat, DepthLowerRange, DepthUpperRange, RangedDepth);
	}
}


void KinectStop(void)
{
	if(NULL != pFreenect)
	{
		pDevice->stopVideo();
		pDevice->stopDepth();

		delete pFreenect;
		pFreenect = NULL;
		pDevice = NULL;
	}

	KinectDepthDetectDestroyTrackbarWindow();
}

