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
#include <iostream>
#include <string>
#include <vector>

#include "EllipseHandDetection.h"
#include "HSVHandDetection.h"
#include "KinectInterface.h"

typedef enum
{
	DETECTION_METHOD_HSV,
	DETECTION_METHOD_ELLIPSE
}	DetectionMethod_t;



static bool TrackbarValueChanged = false;

static void TrackbarValueChangeCallback(int Value, void *pParam)
{
	TrackbarValueChanged = true;
}


int main(int argc, char** argv)
{
	bool UseLiveVideo = false;
	bool ReadFromFile = false;
	bool UseKinect = false;

	bool Run = true;
	std::string InputFileName;
	DetectionMethod_t DetectionMethod = DETECTION_METHOD_HSV;

	const char InputImageWindow[] = "Input Image";
	const char HSVDetectionWindow[] = "HSV Detect Results";
	const char EllipseDetectionWindow[] = "Ellipse Detect Results";
	const char KinectDepthRangeDetectionWindow[] = "Kinect Depth Range Results";
	const char *middleWindow1 = NULL;
	const char middleWindow2[] = "Canny Results (Thickened Edges)";
	const char OutputImageWindow[] = "Hand Detection Results";

	// RGB image
	cv::Mat RGBInput;
	// Hand detection result image
	cv::Mat HandDetection;
	// gray scale of RGB image prior to Gaussian blur/Canny edge detect
	cv::Mat GrayInput;
	// Gaussian blurred gray scale image
	cv::Mat GrayBlur;
	// Canny edge detect on grayscale blurred input image
	cv::Mat InputCanny;
	// Edges with thick lines
	cv::Mat CannyThickEdges;
	// Output image
	cv::Mat OutputImage;

	cv::VideoCapture video;

	const int MaxKinectImageRetryCount = 10;

	const cv::String CommandLineParserKeys =
		"{help h | |print this message}{detect d |h |detection method to use (--detect=h (HSV), --detect=e (ellipse))}{videosource v |c |video source to use (--videosource=c (webcam), --videosource=k (kinect))}{@input | |input image to use (leave blank for live video)}";

	// use the OpenCV command line parser to parse the command line arguments
	cv::CommandLineParser parser(argc, argv, CommandLineParserKeys);
	parser.about("ECE-588 Summer 2017 Final Project for Aaron Romain and Michael Kirkhart - version 0.14");

	if(!parser.check())
	{
		parser.printErrors();
		return -1;
	}

	if(parser.has("help"))
	{
		parser.printMessage();
		return 0;
	}

	if(parser.has("@input"))
	{
		InputFileName = parser.get<std::string>("@input");
		ReadFromFile = true;
	}
	else if(parser.has("videosource"))
	{
		std::string video_source = parser.get<std::string>("videosource");

		if("k" == video_source)
		{
			UseKinect = true;
		}
		else
		{
			UseLiveVideo = true;
		}
	}

	std::string detection_method = parser.get<std::string>("detect");
	if("e" == detection_method)
	{
		DetectionMethod = DETECTION_METHOD_ELLIPSE;
	}
	else if("h" == detection_method)
	{
		DetectionMethod = DETECTION_METHOD_HSV;
	}
	else
	{
		std::cout << "Invalid detection method specified" << std::endl;
		return -1;
	}

	if(UseLiveVideo)
	{
		video.open(0);

		if(!video.isOpened())
		{
			std::cout << "ERROR: can not open camera" << std::endl;
			return -1;
		}
		else
		{
			// Show video information
			int VideoWidth = (int)video.get(CV_CAP_PROP_FRAME_WIDTH);
			int VideoHeight = (int)video.get(CV_CAP_PROP_FRAME_HEIGHT);

			std::cout << "Input video: " << VideoWidth << " x " << VideoHeight << std::endl;
		}
	}

	// Create window for input image
	cv::namedWindow(InputImageWindow, CV_WINDOW_AUTOSIZE);
	//Create windows for intermediary steps
	if(UseKinect)
	{
		middleWindow1 = KinectDepthRangeDetectionWindow;
	}
	else if(DETECTION_METHOD_ELLIPSE == DetectionMethod)
	{
		middleWindow1 = EllipseDetectionWindow;
	}
	else
	{
		middleWindow1 = HSVDetectionWindow;
	}

	cv::namedWindow(middleWindow1, CV_WINDOW_AUTOSIZE);
	cv::namedWindow(middleWindow2, CV_WINDOW_AUTOSIZE);
	// Create window for output image
	cv::namedWindow(OutputImageWindow, CV_WINDOW_AUTOSIZE);

	if(UseKinect)
	{
		KinectStart();
	}
	else if(DETECTION_METHOD_ELLIPSE == DetectionMethod)
	{
		EllipseHandDetectionCreateTrackbarWindow(TrackbarValueChangeCallback);
	}
	else
	{
		HSVHandDetectionCreateTrackbarWindow(TrackbarValueChangeCallback);
	}

	while(Run)
	{
		if(ReadFromFile)
		{
			RGBInput = cv::imread(InputFileName);

			if(!RGBInput.data)
			{
				std::cout << "Could not load specified image data" << std::endl;
				return -1;
			}

			if(CV_8UC3 != RGBInput.type())
			{
				std::cout << "Input image is not RGB type" << std::endl;
				return -1;
			}
		}
		else if(UseKinect)
		{
			int RetryCount = 0;
			bool RGBEmpty = false;
			bool DepthEmpty = false;

			// NOTE: especially at the start, the OpenKinect interface may not return one or both images
			// We need to guard against this, as trying to run OpenCV operations on empty images can cause
			// problems
			do
			{
				KinectGetRGBandRangedDepthImages(RGBInput, HandDetection);
				RGBEmpty = RGBInput.empty();
				DepthEmpty = HandDetection.empty();

				RetryCount++;
			}
			while((RGBEmpty) && (DepthEmpty) && (RetryCount < MaxKinectImageRetryCount));

			if(RetryCount >= MaxKinectImageRetryCount)
			{
				std::cout << "Unable to capture images from Kinect" << std::endl;
				return -1;
			}
		}
		else if(UseLiveVideo)
		{
			// Get current image
			video >> RGBInput;

			if(RGBInput.empty())
			{
				std::cout << "Unable to capture video" << std::endl;
				video.release();
				return -1;
			}

			if(CV_8UC3 != RGBInput.type())
			{
				std::cout << "Input video is not RGB type" << std::endl;
				video.release();
				return -1;
			}
		}

		// run hand detection on mid1 image
		if(!UseKinect)
		{
			// create output image using input as a starting point
			// (cannot use cv::Mat = operator - must use cv::Mat::copyTo() function to do deep copy
			RGBInput.copyTo(HandDetection);

			if(DETECTION_METHOD_ELLIPSE == DetectionMethod)
			{
				EllipseHandDetection(HandDetection);
			}
			else
			{
				HSVHandDetection(HandDetection);
			}
		}

		// run Canny on mid2 image
		  //first convert to grayscale
		cv::cvtColor(RGBInput, GrayInput, cv::COLOR_BGR2GRAY);
		  //blur image
		cv::blur(GrayInput, GrayBlur, cv::Size(3,3));
		  //run canny
		cv::Canny(GrayBlur, InputCanny, 10, 80, 3);

		// and thicken the line using a box filter
		cv::boxFilter(InputCanny, CannyThickEdges, -1, cv::Size(3,3), cv::Point(-1, -1), false, cv::BORDER_DEFAULT);

		//AND the hand detection image with the thickened Canny edge detect image to produce the output image
		cv::bitwise_and(HandDetection, CannyThickEdges, OutputImage);

		// display input image
		cv::imshow(InputImageWindow, RGBInput);

		// display intermediary steps windows
		cv::imshow(middleWindow1, HandDetection);
		cv::imshow(middleWindow2, CannyThickEdges);

		// display output image
		cv::imshow(OutputImageWindow, OutputImage);

		do
		{
			char KeyPress = cv::waitKey(10);

			switch(KeyPress)
			{
				case 'C':
				case 'c':
					// save output image
					// TODO: dump hand detection parameters to text file
					cv::imwrite("input.jpg", RGBInput);
					cv::imwrite("grayscale.jpg", GrayInput);
					cv::imwrite("handdetect.jpg", HandDetection);
					cv::imwrite("grayblur.jpg", GrayBlur);
					cv::imwrite("canny.jpg", InputCanny);
					cv::imwrite("thickcanny.jpg", CannyThickEdges);
					cv::imwrite("output.jpg", OutputImage);
					break;

				case 27:
					Run = false;
					break;

				default:
					break;
			}
		}
		while(ReadFromFile && Run && (!TrackbarValueChanged));
	}

	if(UseKinect)
	{
		KinectStop();
	}
	else if(UseLiveVideo)
	{
		video.release();
	}

	return 0;
}


