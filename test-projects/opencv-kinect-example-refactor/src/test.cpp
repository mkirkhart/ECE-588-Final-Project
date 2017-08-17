#include <iostream>
#include <vector>
#include <cmath>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "KinectInterface.h"


int main(int argc, char **argv)
{
	bool die = false;
	const std::string RGBCaptureWindowName("RGB");
	const std::string DepthCaptureWindowName("Depth");

	std::string RGBFilename("rgb-snapshot");
	std::string DepthFilename("depth-snapshot");
	std::string suffix(".png");
	int i_snap(0),iter(0);

	cv::Mat depthf(cv::Size(640,480), CV_8UC1);
	cv::Mat rgbMat(cv::Size(640,480), CV_8UC3, cv::Scalar(0));

	cv::namedWindow(RGBCaptureWindowName, CV_WINDOW_AUTOSIZE);
	cv::namedWindow(DepthCaptureWindowName, CV_WINDOW_AUTOSIZE);

	KinectStart();

	while(!die)
	{
		KinectGetRGBandRangedDepthImages(rgbMat, depthf);

		cv::imshow(RGBCaptureWindowName, rgbMat);
		cv::imshow(DepthCaptureWindowName, depthf);
		char k = cv::waitKey(5);

		switch(k)
		{
			case 27:
				die = true;
				break;

			case 'C':
			case 'c':
				{
					std::ostringstream rgbfile, depthfile;
					rgbfile << RGBFilename << i_snap << suffix;
					depthfile << DepthFilename << i_snap << suffix;
					cv::imwrite(rgbfile.str(), rgbMat);
					cv::imwrite(depthfile.str(), depthf);

					i_snap++;
				}
				break;

			default:
				break;
		}

#ifdef _LIMIT_ITERATIONS
		if(iter < 1000)
		{
			iter++;
		}
		else
		{
			die = true;
		}
#else
		iter++;
#endif	//_LIMIT_ITERATIONS

	}

	KinectStop();

	cv::destroyWindow(RGBCaptureWindowName);
	cv::destroyWindow(DepthCaptureWindowName);

	return 0;
}

