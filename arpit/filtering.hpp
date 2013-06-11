#ifndef FILTERING_HPP
#define FILTERING_HPP
/* Arpit
*/
#include <stdio.h>
#include <highgui/highgui.hpp>
#include <imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include "interpolation.hpp"
#include <stdlib.h>
#include <time.h>
#include <set>
#include <vector>
using namespace cv;
using namespace std;
typedef Vec2s DisparityElemType;

namespace Filter {
  /// block1 is I, block2 is J.
  /// both have to be either CV_8UC1 or CV_32FC1 !
  Point getModalPoint(Mat block1, Mat block2, Mat mask = Mat(), double* pmodalValue = NULL) {
    assert(block1.size() == block2.size());
    Point minPoint, maxPoint;
    double minVal, maxVal;
    MatND result;
    minMaxLoc(block1, &minVal, &maxVal, &minPoint, &maxPoint);
    float range1[] = {minVal,maxVal+1};
    minMaxLoc(block2, &minVal, &maxVal, &minPoint, &maxPoint);
    float range2[] = {minVal,maxVal+1};
    int histSize[] = {(range1[1] - range1[0]), (range2[1] - range2[0])};
    const float * histRange[] = { range1, range2};
    Mat mArr[] = {block1, block2};
    /// we compute the histogram from the 0-th and 1-st channels
    int channels[] = {0, 1};
    calcHist( mArr, 2, channels, mask,
             result, 2, histSize, histRange,
             true, // the histogram is uniform
             false );
    convertScaleAbs(result, result);
    normalize(result, result, 0, 255, CV_MINMAX);
    minMaxLoc(result, &minVal, &maxVal, &minPoint, &maxPoint);
    maxPoint.x += range2[0];
    maxPoint.y += range1[0];
    // cout << maxPoint << " " << minPoint << endl;
    // resize(result, result, cv::Size(result.cols*3, result.rows*3), 0, 0, INTER_NEAREST);
    // imshow("1", result);
    // waitKey();
    if(pmodalValue)
      *pmodalValue = maxVal;
    return maxPoint;
  }
  /// block has to be 2 channel
  /// channel1 is delI, channel2 is delJ
  Point getModalPoint(Mat &block, double *pmodalValue = NULL) {
    vector<Mat> planes;
    split(block, planes);
    assert(planes.size() == 2);
    Point ret = getModalPoint(planes[0], planes[1], pmodalValue);
    merge(planes, block);
    return ret;
  }
  /// applies getModalPoint, getWrongPoints and interpolation on src1 and src2
  /// input images should not be blocky!
  /// blockSize is for region to consider for modal evaluation
  /// most prob(!) src1 is I, src2 is J.
  Mat applyModalFilter(Mat mask, Mat src1, Mat src2, int blockSize, double sdI, double sdJ) {
    assert(src1.size() == src2.size() && mask.size() == src1.size());
    assert(src1.rows >= blockSize && src1.cols >= blockSize);
    Mat resI = Mat::zeros(Size(src1.cols/blockSize, src1.rows/blockSize), CV_16SC1);
    Mat resJ = Mat::zeros(resI.size(), CV_16SC1);
    for (int i = 0; i < src1.rows-blockSize+1; i += blockSize)
    {
      for (int j = 0; j < src1.cols-blockSize+1; j += blockSize)
      {
        int rowStart = i;
        int rowEnd = std::min(src1.rows, i+blockSize);
        int colStart = j;
        int colEnd = std::min(src1.cols, j+blockSize);
        Mat roi_src1 = src1.rowRange(rowStart, rowEnd).colRange(colStart, colEnd);
        Mat roi_src2 = src2.rowRange(rowStart, rowEnd).colRange(colStart, colEnd);
        Mar roi_mask = mask.rowRange(rowStart, rowEnd).colRange(colStart, colEnd);
        Mat b1, b2;
        roi_src1.convertTo(b1, CV_32FC1);
        roi_src2.convertTo(b2, CV_32FC1);
        Point p = getModalPoint(b1, b2, roi_mask, NULL);
        int &dispI = p.y;
        int &dispJ = p.x;
        /// Right now not finding centroid. Using the centre of the block itself for interpolation.
        resI.at<DisparityElemType>(i/blockSize, j/blockSize) = dispI;
        resJ.at<DisparityElemType>(i/blockSize, j/blockSize) = dispJ;
      }
    }
    resize(resI, resI, src1.size(), 0, 0, INTER_CUBIC);
    resize(resJ, resJ, src1.size(), 0, 0, INTER_CUBIC);
    vector<Mat> mArr; 
    mArr.push_back(resI);
    mArr.push_back(resJ);
    Mat result;
    merge(mArr, result);
    return result;
  }
}
#endif