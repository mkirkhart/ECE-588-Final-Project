#include <iostream>
#include <vector>
#include <cmath>
#include "libfreenect.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "MyFreenectDevice.h"

using namespace cv;
using namespace std;


int main(int argc, char **argv)
{
	bool die = false;
	const std::string RGBCaptureWindowName("RGB");
	const std::string DepthCaptureWindowName("Depth");

	string filename("snapshot");
	string suffix(".png");
	int i_snap(0),iter(0);

	Mat depthMat(Size(640,480),CV_16UC1);
	Mat depthf (Size(640,480),CV_8UC1);
	Mat rgbMat(Size(640,480),CV_8UC3,Scalar(0));
	Mat ownMat(Size(640,480),CV_8UC3,Scalar(0));

	// The next two lines must be changed as Freenect::Freenect
	// isn't a template but the method createDevice:
	// Freenect::Freenect<MyFreenectDevice> freenect;
	// MyFreenectDevice& device = freenect.createDevice(0);
	// by these two lines:
	//	Freenect::Freenect freenect;
	//	MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);

	// Well, apparently this is no longer true, and we need create the device
	// via the "prior" method
#ifdef _UBUNTU_12_04
	Freenect::Freenect<MyFreenectDevice> freenect;
	MyFreenectDevice& device = freenect.createDevice(0);
#else
	Freenect::Freenect freenect;
	MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);
#endif	//_UBUNTU_12_04


	namedWindow(RGBCaptureWindowName, CV_WINDOW_AUTOSIZE);
	namedWindow(DepthCaptureWindowName, CV_WINDOW_AUTOSIZE);
	device.startVideo();
	device.startDepth();

	while(!die)
	{
		device.getVideo(rgbMat);
		device.getDepth(depthMat);
		cv::imshow(RGBCaptureWindowName, rgbMat);
		depthMat.convertTo(depthf, CV_8UC1, 255.0/2048.0);
		cv::imshow(DepthCaptureWindowName, depthf);
		char k = cvWaitKey(5);

		switch(k)
		{
			case 27:
				die = true;
				break;

			case 'C':
			case 'c':
				{
					std::ostringstream file;
					file << filename << i_snap << suffix;
					cv::imwrite(file.str(), rgbMat);
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

	destroyWindow(RGBCaptureWindowName);
	destroyWindow(DepthCaptureWindowName);

	device.stopVideo();
	device.stopDepth();

	return 0;
}


