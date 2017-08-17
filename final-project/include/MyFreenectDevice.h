#include <iostream>
#include <vector>
#include <cmath>
#include <pthread.h>
#include "libfreenect.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"


using namespace cv;
using namespace std;


class myMutex
{
	public:
		myMutex();
		~myMutex();
		void lock();
		void unlock();

	private:
		pthread_mutex_t m_mutex;
};


class MyFreenectDevice : public Freenect::FreenectDevice
{
	public:
		MyFreenectDevice(freenect_context *_ctx, int _index);

		// Do not call directly even in child
		void VideoCallback(void* _rgb, uint32_t timestamp);

		// Do not call directly even in child
		void DepthCallback(void* _depth, uint32_t timestamp);

		bool getVideo(Mat& output);

		bool getDepth(Mat& output);

	private:
		std::vector<uint8_t> m_buffer_depth;
		std::vector<uint8_t> m_buffer_rgb;
		std::vector<uint16_t> m_gamma;
		Mat depthMat;
		Mat rgbMat;
		Mat ownMat;
		myMutex m_rgb_mutex;
		myMutex m_depth_mutex;
		bool m_new_rgb_frame;
		bool m_new_depth_frame;
};


