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

namespace Filter {
  /// block1 is I, block2 is J.
  /// both have to be either CV_8UC1 or CV_32FC1 !
  Point getModalPoint(Mat block1, Mat block2, double* pmodalValue = NULL) {
    assert(block1.size() == block2.size());
    Point minPoint, maxPoint;
    double minVal, maxVal;
    MatND result;
    minMaxLoc(block1, &minVal, &maxVal, &minPoint, &maxPoint);
    float range1[] = {minVal,maxVal+1};
    minMaxLoc(block2, &minVal, &maxVal, &minPoint, &maxPoint);
    float range2[] = {minVal,maxVal+1};
    int histSize[] = {range1[1] - range1[0], range2[1] - range2[0]};
    const float * histRange[] = { range1, range2};
    Mat mArr[] = {block1, block2};
    /// we compute the histogram from the 0-th and 1-st channels
    int channels[] = {0, 1};
    calcHist( mArr, 2, channels, Mat(), // do not use mask
             result, 2, histSize, histRange,
             true, // the histogram is uniform
             false );
    convertScaleAbs(result, result);
    normalize(result, result, 0, 255, CV_MINMAX);
    minMaxLoc(result, &minVal, &maxVal, &minPoint, &maxPoint);
    maxPoint.x += range2[0];
    maxPoint.y += range1[0];
    // cout << maxPoint << " " << minPoint << endl;
    resize(result, result, cv::Size(result.cols*3, result.rows*3), 0, 0, INTER_NEAREST);
    imshow("1", result);
    waitKey();
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
  /// block has to be 2 channel
  /// channel1 is delI, channel2 is delJ
  /// DOES NOT return points with ROI offset added!
  template <typename T>
  vector<Point> getWrongPoints(double modeI, double modeJ, Mat block, double sdI, double sdJ) {
    vector<Point> wrongPoints;
    Size s; Point ofs;
    /// doesnt really do anything with it...
    block.locateROI(s, ofs);
    for (int i = 0; i < block.rows; ++i) {
      for (int j = 0; j < block.cols; ++j) {
        double tempI = block.at<T>(i,j)[0] - (double)modeI;
        double tempJ = block.at<T>(i,j)[1] - (double)modeJ;
        if(tempI*tempI/(sdI*sdI) + tempJ*tempJ/(sdJ*sdJ) > 1) {
          wrongPoints.push_back(Point(j, i));
        }
      }
    }
    return wrongPoints;
  }
  struct ClassComp {
    bool operator() (const Point &p1, const Point &p2) const {
      return p1.x > p2.x? true: p2.x > p1.x ? false: p1.y > p2.y;
    }
  };
  /// has to be a single channel image! for 2 channel, split and apply seperately, the merge.
  /// modifies src itself.
  template<typename T>
  void nnInterpolation(vector<Point> &wrongPoints, Mat &src) {
    set<Point, ClassComp> wPoints(wrongPoints.begin(), wrongPoints.end());
    static const int neighbours[][2] = {{-1,-1},
                                        {-1, 0},
                                        {-1, 1},
                                        {0, -1},
                                        {0, +1},
                                        {1, -1},
                                        {+1, 0},
                                        {+1, 1}};
    /// not using the iterative method of ../interpolation.hpp, just 1 pass
    /// actually i'm using the code from ../interpolation.hpp now... ><
    /*for (int i = 0; i < wrongPoints.size(); ++i)
    {
      Point p = wrongPoints[i];
      double sum = 0;
      int count = 0;
      for (int j = 0; j < 8; ++j)
      {
        int ni = p.y + neighbours[j][0];
        int nj = p.x + neighbours[j][1];
        if(wPoints.find(Point(nj, ni)) != wPoints.end()) {
          sum += src.at<T>(i,j);
          count++;
        }
      }
      if(count)
        sum /= count;
      src.at<T>(p.x, p.y) = sum;
    }*/
      int iter = 0;
      while(wPoints.size()) {
      ++iter;
      for (set<Point, ClassComp>::iterator k = wPoints.begin(); k != wPoints.end(); )
      {
        int i = k->y;
        int j = k->x;
        double sum = 0; /// assuming T may be typecast from/to double!
        int count = 0;
        for (int l = 0; l < 8; ++l)
        {
          int ni = (i+neighbours[l][0]);
          int nj = (j+neighbours[l][1]);
          if(ni < 0 || ni >= src.rows || nj < 0 || nj >= src.cols)
            continue;
          if(wPoints.find(Point(nj,ni)) == wPoints.end()) {
            count++;
            sum += src.at<T>(ni, nj);
          }
        }
        if(count > 0) {
          /// Some neighbour(s) valid. removing from invalid list.
          set<Point>::iterator t = k;
          ++k;
          wPoints.erase(t);
          src.at<T>(i,j) = (T)(sum/count);
          if(!wPoints.size())
            break;
        }
        else {
          ++k;
        }
      }
    }
    // cout << "iter = " << iter << endl;
  }
  /// applies getModalPoint, getWrongPoints and nnInterpolation on src1 and src2
  /// input images should not be blocky!
  /// blockSize is for region to consider for modal evaluation
  void applyModalFilter(Mat src1, Mat src2, int blockSize, double sdI, double sdJ) {
    assert(src1.size() == src2.size());
    assert(src1.rows >= blockSize && src1. cols >= blockSize);
    std::vector<Point,ClassComp> gWrongPoints;
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
        Mat b1;
        Mat b2;
        roi_src1.convertTo(b1, CV_32FC1);
        roi_src2.convertTo(b2, CV_32FC1);
        Point p = getModalPoint(b1, b2, NULL);
        vector<Mat> mArr;
        mArr.push_back(b1); mArr.push_back(b2);
        Mat dst;
        merge(mArr, dst);
        std::vector<Point> wPoints = getWrongPoints<Vec2f>(p.y, p.x, dst, sdI, sdJ);
        for (int l = 0; l < wPoints.szie(); ++l)
        {
          wPoints[l].x += j;
          wPoints[l].y += i;
        }
        gWrongPoints.append(wPoints.begin(), wPoints.end());        
      }
    }
    
  }
}
#endif