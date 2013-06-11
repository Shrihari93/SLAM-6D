#ifndef CORNERS_HPP
#define CORNERS_HPP 
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

namespace Corners
{
  Mat harrisCorner(Mat src, long long numPoints) {
    Mat dst, dst_norm, dst_norm_scaled;
    dst = Mat::zeros( src.size(), CV_32FC1 );

    /// Detector parameters
    int blockSize = 1;
    int apertureSize = 7;
    double k = 0.01;

    /// Detecting corners
    cornerHarris( src, dst, blockSize, apertureSize, k, BORDER_DEFAULT );

    /// Normalizing
    normalize( dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
    convertScaleAbs(dst_norm, dst_norm_scaled);
    
    /// Histogram for finding threshold
    /// Finding histogram threshold with (values > threshold) just > numPoints.
    Point minPoint, maxPoint;
    double minVal, maxVal;
    minMaxLoc(dst_norm, &minVal, &maxVal, &minPoint, &maxPoint);
    float range[] = { minVal, maxVal+1 } ;
    int histSize = range[1] - range[0];
    const float* histRange = { range };
    bool uniform = true; bool accumulate = false;
    Mat hist;
    calcHist( &dst_norm, 1, 0, Mat(), hist, 1, &histSize, &histRange, uniform, accumulate );
    int histSum = 0;
    for (int i = 0; i < hist.rows; ++i)
        histSum += hist.at<float>(i, 0);
    int count = histSum, idx = 0;
    for (idx = 0; idx < hist.rows; ++idx)
    {
        if(count < numPoints)
            break;
        count -= hist.at<float>(idx, 0);
        // printf("%d\n", count);
    }
    int threshold = idx;//+range[0];
    printf("hist.rows = %d, threshold = %d, count = %d, %% = %f\n", hist.rows, threshold, count, count*100.0/(src.rows*src.cols));
    /// Making ret matrix
    Mat result = Mat::zeros( src.size(), CV_8UC1 );
    for (int i = 0; i < dst_norm.rows; ++i)
    {
      for (int j = 0; j < dst_norm.cols; ++j)
      {
        if(dst_norm.at<float>(i,j) >= threshold) {
          result.at<uchar>(i,j) = (uchar)255;
        }
      }
    }
    return result;
  }
}

#endif
