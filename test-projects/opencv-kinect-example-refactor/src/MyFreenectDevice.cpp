#include <iostream>
#include <vector>
#include <cmath>
#include <pthread.h>
#include "libfreenect.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "MyFreenectDevice.h"

using namespace cv;
using namespace std;


myMutex::myMutex()
{
	pthread_mutex_init(&m_mutex, NULL);
}

myMutex::~myMutex()
{
	pthread_mutex_destroy(&m_mutex);
}

void myMutex::lock()
{
	pthread_mutex_lock(&m_mutex);
}

void myMutex::unlock()
{
	pthread_mutex_unlock(&m_mutex);
}

MyFreenectDevice::MyFreenectDevice(freenect_context *_ctx, int _index) :
	Freenect::FreenectDevice(_ctx, _index),
	m_buffer_depth(FREENECT_DEPTH_11BIT),
	m_buffer_rgb(FREENECT_VIDEO_RGB),
	m_gamma(2048),
	depthMat(Size(640,480),CV_16UC1),
	rgbMat(Size(640,480), CV_8UC3, Scalar(0)),
	ownMat(Size(640,480),CV_8UC3,Scalar(0))
{
	m_new_rgb_frame = false;
	m_new_depth_frame = false;

	for(unsigned int i = 0 ; i < 2048 ; i++)
	{
		float v = i / 2048.0;
		v = std::pow(v, 3) * 6;
		m_gamma[i] = v * 6 * 256;
	}
}


// Do not call directly even in child
void MyFreenectDevice::VideoCallback(void* _rgb, uint32_t timestamp)
{
	std::cout << "RGB callback" << std::endl;
	m_rgb_mutex.lock();
	uint8_t* rgb = static_cast<uint8_t*>(_rgb);
	rgbMat.data = rgb;
	m_new_rgb_frame = true;
	m_rgb_mutex.unlock();
}


// Do not call directly even in child
void MyFreenectDevice::DepthCallback(void* _depth, uint32_t timestamp)
{
	std::cout << "Depth callback" << std::endl;
	m_depth_mutex.lock();
	uint16_t* depth = static_cast<uint16_t*>(_depth);
	depthMat.data = (uchar*) depth;
	m_new_depth_frame = true;
	m_depth_mutex.unlock();
}

bool MyFreenectDevice::getVideo(Mat& output)
{
	m_rgb_mutex.lock();
	if(m_new_rgb_frame)
	{
		cv::cvtColor(rgbMat, output, CV_RGB2BGR);
		m_new_rgb_frame = false;
		m_rgb_mutex.unlock();
		return true;
	}
	else
	{
		m_rgb_mutex.unlock();
		return false;
	}
}

bool MyFreenectDevice::getDepth(Mat& output)
{
	m_depth_mutex.lock();
	if(m_new_depth_frame)
	{
		depthMat.copyTo(output);
		m_new_depth_frame = false;
		m_depth_mutex.unlock();
		return true;
	}
	else
	{
		m_depth_mutex.unlock();
		return false;
	}
}


