#ifndef PYRAMID_HPP
#define PYRAMID_HPP
/* Arpit
*/
#include <stdio.h>
#include <highgui/highgui.hpp>
#include <imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include "filtering.hpp"
#include "corners.hpp"
#include "resize.hpp"
#include "block-matching.hpp"
#include <stdlib.h>
#include <time.h>
using namespace cv;
using namespace std;
#define INF 99999999
#define WINDOWNAME1 "Display1"
#define WINDOWNAME2 "DIsplay2"


typedef Vec2s DisparityElemType;
// All blocksize/windowsize definitions here global. <- not really
static const int STAGES = 4;

namespace BlockMatching
{  
  Mat pyramidalMatching(Mat src1, Mat src2, int shiftJ)
  {
    assert(src1.size() == src2.size());
    Mat result;
    int stage = 0;
    // creating resized images
    vector <Mat> resized1, resized2;
    resized1.push_back(Resize::resizeImage(src1, Size(src1.cols/8, src1.rows/8)));
    resized1.push_back(Resize::resizeImage(src1, Size(src1.cols/4, src1.rows/4)));
    resized1.push_back(Resize::resizeImage(src1, Size(src1.cols/2, src1.rows/2)));
    resized1.push_back(src1);
    resized2.push_back(Resize::resizeImage(src2, Size(src2.cols/8, src2.rows/8)));
    resized2.push_back(Resize::resizeImage(src2, Size(src2.cols/4, src2.rows/4)));
    resized2.push_back(Resize::resizeImage(src2, Size(src2.cols/2, src2.rows/2)));
    resized2.push_back(src2);
    Mat current = Mat::zeros(resized1[0].size(), CV_16SC2);
    while(stage < STAGES)
    {
      resize(current, current, resized1[stage].size(), 0, 0, INTER_LINEAR);
      current *= 2; /// scaling disparity to new size
      Mat &img1 = resized1[stage];
      Mat &img2 = resized2[stage];
      Mat mask = Corners::harrisCorner(img1, 0.20*img1.rows*img1.cols);
      Mat scattered = BlockMatching::blockMatching(mask, img1, img2, current, 24+stage+stage%2, !stage, shiftJ);
      vector<Mat> planes;
      split(scattered, planes);
      assert(planes.size() == 2);
      Mat temp;
      convertScaleAbs(planes[0], temp);
      result = Filter::applyModalFilter(mask, planes[0], planes[1], 30);
      imshow("1", temp);
      imshow("mask", mask);
      // imshow("result", result);
      vector<Mat> planes2;
      split(result, planes2);
      Mat temp2;
      convertScaleAbs(planes2[0], temp2);
      imshow("result", temp2);
      merge(planes2, result);
      waitKey(0);
      current = result;
      stage++;
    }
    return result;
  }
}


#endif