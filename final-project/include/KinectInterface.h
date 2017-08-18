
#ifndef _KINECTINTERFACEH_
#define _KINECTINTERFACEH_

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <fstream>

void KinectStart(void);
void KinectGetRGBandRangedDepthImages(cv::Mat &RGB, cv::Mat &RangedDepth);
void KinectDepthDetectionDumpParametersToFile(std::ofstream &filestream);
void KinectStop(void);

#endif	//_KINECTINTERFACEH_

