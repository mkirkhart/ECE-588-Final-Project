/*
 * DisplayImage.cpp
 *
 *  Created on: Aug 3, 2017
 *      Author: aaron
 */

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "HSVHandDetection.h"

// In GIMP color chooser, HSV ranges are as follows:
// Hue: 0 to 360
// Saturation: 0 to 100
// Value: 0 to 100
// In OpenCV, HSV ranges are as follows:
// Hue: 0 to 180
// Saturation: 0 to 255
// Value: 0 to 255

// From number01.jpg in GIMP:
// Hue range from 11 to 24
// Saturation range from 38 to 68
// Value range from 47 to 94
// This translates to OpenCV ranges of:
// Hue: 5 to 12 (use 5 to 15)
// Saturation: 97 to 240 (use 90 to 240)
// Value: 120 to 240 (use 120 to 240)

//#define _USE_ORIGINAL_HSV_LIMITS

#ifdef _USE_ORIGINAL_HSV_LIMITS
//static const cv::Scalar HSVMin = cv::Scalar(0, 30, 80);
//static const cv::Scalar HSVMax = cv::Scalar(20, 150, 255);
#else
//static const cv::Scalar HSVMin = cv::Scalar(10, 30, 50);
//static const cv::Scalar HSVMax = cv::Scalar(25, 200, 255);
//static const cv::Scalar HSVMin = cv::Scalar(0, 20, 20);
//static const cv::Scalar HSVMax = cv::Scalar(40, 255, 255);
#endif	//_USE_ORIGINAL_HSV_LIMITS

static const char HSVTrackbarWindowName[] = "HSV Detection Parameters";

static const char HSVTrackbarMinHueName[] = "Min H";
static const char HSVTrackbarMaxHueName[] = "Max H";

static const char HSVTrackbarMinSaturationName[] = "Min S";
static const char HSVTrackbarMaxSaturationName[] = "Max S";

static const char HSVTrackbarMinValueName[] = "Min V";
static const char HSVTrackbarMaxValueName[] = "Max V";

static int MinHue = 80;
static int MaxHue = 140;
static const int HueLowLimit = 0;
static const int HueHighLimit = 180;

static int MinSaturation = 20;
static int MaxSaturation = 255;

static const int SaturationLowLimit = 0;
static const int SaturationHighLimit = 255;

static int MinValue = 20;
static int MaxValue = 255;

static const int ValueLowLimit = 0;
static const int ValueHighLimit = 255;


void HSVHandDetection(cv::Mat &Image)
{
	// NOTE: while the input image is actually BGR, we specify
	// it as RGB when converting to HSV, as this automatically
	// "rotates" the hue by 180 degrees (90 in OpenCV), putting
	// red at 90 instead of 0.  This allows us to set low limits
	// to negative hues (reds with small amounts of blue)
	cv::cvtColor(Image, Image, CV_RGB2HSV);

	const cv::Scalar HSVMin(MinHue, MinSaturation, MinValue);
	const cv::Scalar HSVMax(MaxHue, MaxSaturation, MaxValue);

	cv::inRange(Image, HSVMin, HSVMax, Image);
}


void HSVHandDetectionCreateTrackbarWindow(void (*pCallbackFunction)(int, void *))
{
	cv::namedWindow(HSVTrackbarWindowName, CV_WINDOW_AUTOSIZE);

	cv::createTrackbar(HSVTrackbarMinHueName, HSVTrackbarWindowName, &MinHue, HueHighLimit, pCallbackFunction, NULL);
	cv::createTrackbar(HSVTrackbarMaxHueName, HSVTrackbarWindowName, &MaxHue, HueHighLimit, pCallbackFunction, NULL);
	cv::createTrackbar(HSVTrackbarMinSaturationName, HSVTrackbarWindowName, &MinSaturation, SaturationHighLimit, pCallbackFunction, NULL);
	cv::createTrackbar(HSVTrackbarMaxSaturationName, HSVTrackbarWindowName, &MaxSaturation, SaturationHighLimit, pCallbackFunction, NULL);
	cv::createTrackbar(HSVTrackbarMinValueName, HSVTrackbarWindowName, &MinValue, ValueHighLimit, pCallbackFunction, NULL);
	cv::createTrackbar(HSVTrackbarMaxValueName, HSVTrackbarWindowName, &MaxValue, ValueHighLimit, pCallbackFunction, NULL);
}


void HSVHandDetectionDestroyTrackbarWindow(void)
{
	cv::destroyWindow(HSVTrackbarWindowName);
}

