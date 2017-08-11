
#ifndef _ELLIPSEHANDDETECTIONH_
#define _ELLIPSEHANDDETECTIONH_

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <vector>

void EllipseHandDetection(cv::Mat &Image);
void EllipseHandDetectionCreateTrackbarWindow(void (*pCallbackFunction)(int, void *));
void EllipseHandDetectionDestroyTrackbarWindow(void);

#endif	//_ELLIPSEHANDDETECTIONH_

