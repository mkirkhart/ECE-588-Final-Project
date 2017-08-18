#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "EllipseHandDetection.h"

#define _USE_ORIGINAL_ELLIPSE_AB_VALUES

#if defined(_USE_ORIGINAL_ELLIPSE_AB_VALUES)
//static const double EllipseA = 0.02;
//static const double EllipseB = 0.06;
#else
//static const double EllipseA = 0.14;
//static const double EllipseB = 0.24;
#endif	//_USE_ORIGINAL_ELLIPSE_AB_VALUES


static const char EllipseTrackbarWindowName[] = "Ellipse Detection Parameters";

static const char EllipseTrackbarEllipseAName[] = "Ellipse A";
static const char EllipseTrackbarEllipseBName[] = "Ellipse B";

static int ScaledEllipseA = 20;

static const int MinScaledEllipseA = 1;
static const int MaxScaledEllipseA = 200;

static int ScaledEllipseB = 60;

static const int MinScaledEllipseB = 1;
static const int MaxScaledEllipseB = 400;

static const double EllipseABScalingFactor = 0.001;

static bool IsInsideStandardEllipse(const cv::Vec3b &BGRPixel, const double EllipseA, const double EllipseB)
{
	bool ReturnValue = false;
	static const double EllipseX0 = 0.32;
	static const double EllipseY0 = 0.4;

	const double Blue = BGRPixel.val[0];
	const double Green = BGRPixel.val[1];
	const double Red = BGRPixel.val[2];

	const double RGBSum = Red + Green + Blue;

	// compute normalized red, green, provided all are greater than zero
	if(RGBSum > 0.0)
	{
		const double NormalizedRed = Red / RGBSum;
		const double NormalizedGreen = Green / RGBSum;

		double EllipseValueX = (NormalizedGreen - EllipseX0) / EllipseA;
		double EllipseValueY = (NormalizedRed - EllipseY0) / EllipseB;

		EllipseValueX *= EllipseValueX;
		EllipseValueY *= EllipseValueY;

		const double EllipseValue = EllipseValueX + EllipseValueY;
		if(EllipseValue < 1.0)
		{
			ReturnValue = true;
		}
	}

	return ReturnValue;
}


void EllipseHandDetection(cv::Mat &Image)
{
	double EllipseA = ScaledEllipseA * EllipseABScalingFactor;
	double EllipseB = ScaledEllipseB * EllipseABScalingFactor;

	// test each pixel to see if its red, green values fall within the "standard ellipse"
	// if so, make it white
	// if not, set it to black
	for(int i = 0;i < Image.rows;i++)
	{
		for(int j = 0;j < Image.cols;j++)
		{
			cv::Vec3b& bgrPixel = Image.at<cv::Vec3b>(i,j);

			if(IsInsideStandardEllipse(bgrPixel, EllipseA, EllipseB))
			{
				bgrPixel.val[0] = 255;
				bgrPixel.val[1] = 255;
				bgrPixel.val[2] = 255;
			}
			else
			{
				bgrPixel.val[0] = 0;
				bgrPixel.val[1] = 0;
				bgrPixel.val[2] = 0;
			}
		}
	}

	cv::cvtColor(Image, Image, CV_BGR2GRAY);
}


void EllipseHandDetectionDumpParametersToFile(std::ofstream &filestream)
{
	if(filestream.is_open())
	{
		filestream << "Ellipse parameters:" << std::endl;
		filestream << "Scaled Ellipse A = " << ScaledEllipseA << std::endl;
		filestream << "Scaled Ellipse B = " << ScaledEllipseB << std::endl;
	}
}


void EllipseHandDetectionCreateTrackbarWindow(void (*pCallbackFunction)(int, void *))
{
	cv::namedWindow(EllipseTrackbarWindowName, CV_WINDOW_AUTOSIZE);

	cv::createTrackbar(EllipseTrackbarEllipseAName, EllipseTrackbarWindowName, &ScaledEllipseA, MaxScaledEllipseA, pCallbackFunction, NULL);
	cv::createTrackbar(EllipseTrackbarEllipseBName, EllipseTrackbarWindowName, &ScaledEllipseB, MaxScaledEllipseB, pCallbackFunction, NULL);

	cv::setTrackbarMin(EllipseTrackbarEllipseAName, EllipseTrackbarWindowName, MinScaledEllipseA);
	cv::setTrackbarMin(EllipseTrackbarEllipseBName, EllipseTrackbarWindowName, MinScaledEllipseB);
}


void EllipseHandDetectionDestroyTrackbarWindow(void)
{
	cv::destroyWindow(EllipseTrackbarWindowName);
}

