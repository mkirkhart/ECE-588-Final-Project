
#ifndef _KINECTINTERFACEH_
#define _KINECTINTERFACEH_

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

void KinectStart(void);
void KinectGetRGBandRangedDepthImages(cv::Mat &RGB, cv::Mat &RangedDepth);
void KinectStop(void);

#endif	//_KINECTINTERFACEH_

