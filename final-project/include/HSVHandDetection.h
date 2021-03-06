/*
 * DisplayImage.cpp
 *
 *  Created on: Aug 3, 2017
 *      Author: aaron
 */

#ifndef _HSVHANDDETECTIONH_
#define _HSVHANDDETECTIONH_


#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <fstream>
#include <vector>

void HSVHandDetection(cv::Mat &Image);
void HSVHandDetectionDumpParametersToFile(std::ofstream &filestream);
void HSVHandDetectionCreateTrackbarWindow(void (*pCallbackFunction)(int, void *));
void HSVHandDetectionDestroyTrackbarWindow(void);

#endif	//_HSVHANDDETECTIONH_

