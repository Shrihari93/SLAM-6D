#ifndef RESIZE_HPP
#define RESIZE_HPP 
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

namespace Resize
{
	Mat resizeImage(Mat src, Size size) {
		Mat res;
		// src = src.clone();
		// GaussianBlur(src, src, Size(3, 3), 0, 0);
		resize(src, res, size, 0, 0, INTER_LINEAR); /// should generally be used only for downsizing!
		// normalize(res, res, 0, 255, CV_MINMAX);
		return res;
	}
}
#endif
