#ifndef FILTERING_HPP
#define FILTERING_HPP
/* Arpit
*/
#include <stdio.h>
#include <highgui/highgui.hpp>
#include <imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <csa.h>
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
  Point getModalPoint(Mat &block, Mat mask = Mat(), double *pmodalValue = NULL) {
    vector<Mat> planes;
    split(block, planes);
    assert(planes.size() == 2);
    Point ret = getModalPoint(planes[0], planes[1], mask, pmodalValue);
    merge(planes, block);
    return ret;
  }
  /// block1, block2: short
  /// mask: uchar
  Point getCentroid(Mat block1, Mat block2, Mat mask, Point mean, double sdI, double sdJ) {
    assert(block1.size() == block2.size());
    double sumi=0, sumj=0;
    int count = 0;
    for (int i = 0; i < block1.rows; ++i)
    {
      for (int j = 0; j < block1.cols; ++j)
      {
        if(!mask.at<uchar>(i,j))
          continue;
        short &vi = block1.at<short>(i,j);
        short &vj = block2.at<short>(i,j);
        if((vi-mean.y)*(vi-mean.y)/(sdI*sdI) - (vj-mean.x)*(vj-mean.x)/(sdJ*sdJ) <= 1) {
          sumi += i;
          sumj += j;
          count++;
        }
      }
    }
    if(count) {
      sumi /= count;
      sumj /= count;
    }
    else {
      printf("count 0!!\n");
    }
    Size size; Point ofs;
    block1.locateROI(size, ofs);
    return Point(sumj+ofs.x, sumi+ofs.y);
  }
  double distSq(DisparityElemType p1, DisparityElemType p2) {
    return (p1[0]-(double)p2[0])*(p1[0]-p2[0])+(p1[1]-(double)p2[1])*(p1[1]-p2[1]);
  }
  /// points: dim=2; ch1: i, ch2: j
  /// values: dim=2; ch1: deli, ch2: delj
  /// both are CV_16SC2
  Mat interpolate(Mat points, Mat values, int blockSize) {
    assert(points.size() == values.size());
    static int neighbours[][2] = { {-1,-1},
              {-1, 0},
              {-1, 1},
              {0, -1},
              {0, +1},
              {1, -1},
              {+1, 0},
              {+1, 1}};
    Mat result = Mat::zeros(Size(points.cols*blockSize, points.rows*blockSize), CV_16SC2);
    for (int i = 0; i < points.rows; ++i)
    {
      for (int j = 0; j < points.cols; ++j)
      {
        DisparityElemType centroid = points.at<DisparityElemType>(i,j);
        DisparityElemType value = values.at<DisparityElemType>(i,j);
        vector<DisparityElemType> nbCentroids;
        vector<DisparityElemType> nbValues;
        nbCentroids.push_back(centroid);
        nbValues.push_back(value);
        for(int k=0; k<8; k++)
        {
          int ni = i+neighbours[k][0];
          int nj = j+neighbours[k][1];
          if(ni < 0 || ni >= points.rows || nj < 0 || nj >= points.cols)
            continue;
          nbCentroids.push_back(points.at<DisparityElemType>(ni,nj));
          nbValues.push_back(values.at<DisparityElemType>(ni,nj));
        }
        for (int ii = i*blockSize; ii < blockSize+i*blockSize; ++ii)
        {
          for (int jj = j*blockSize; jj < blockSize+j*blockSize; ++jj)
          {
            if(ii == centroid[0] && jj == centroid[1]) {
              result.at<DisparityElemType>(ii, jj) = value;
              continue;
            }
            double wtSum = 0;
            double vali = 0, valj=0;
            DisparityElemType currP(ii, jj);
            for (int l = 0; l < nbCentroids.size(); ++l)
            {
              double wt = distSq(currP, nbCentroids[l]);
              if(wt == 0.0) {
                printf("ii jj: %d %d, centroid: %d %d, nbCentroids[%d]= %d %d\n", (int)ii, (int)jj, (int)centroid[0], (int)centroid[1], (int)l, (int)nbCentroids[l][0], (int)nbCentroids[l][1]);
                assert(0);
              }
              assert(wt != 0.0);
              wt = (1/wt);
              wtSum += wt;
              vali += wt * nbValues[l][0]; 
              valj += wt * nbValues[l][1];
            }
            assert(wtSum != 0);
            vali /= wtSum;
            valj /= wtSum;
            result.at<DisparityElemType>(ii, jj) = DisparityElemType(vali, valj);
          }
        }
      }
    }
    return result;
  }
  /// uses cubic spline interpolation from csa.
  /// very shitty code sorry
  Mat interpolate2(Mat centroids, Mat values, int blockSize) {
    assert(centroids.size() == values.size());
    point *inputI = new point[centroids.cols*centroids.rows];
    point *inputJ = new point[centroids.cols*centroids.rows];
    for (int i = 0; i < centroids.rows; ++i)
    {
      for (int j = 0; j < centroids.cols; ++j)
      {
        point tp;
        tp.x = centroids.at<DisparityElemType>(i,j)[1];
        tp.y = centroids.at<DisparityElemType>(i,j)[0];
        tp.z = values.at<DisparityElemType>(i,j)[0];
        inputI[i*centroids.cols+j] = tp;
        tp.z = values.at<DisparityElemType>(i,j)[1];
        inputJ[i*centroids.cols+j] = tp;
      }
    }
    point *outputI = new point[centroids.cols*centroids.rows*blockSize*blockSize];
    point *outputJ = new point[centroids.cols*centroids.rows*blockSize*blockSize];
    for (int i = 0; i < centroids.rows*blockSize; ++i)
    {
      for (int j = 0; j < centroids.cols*blockSize; ++j)
      {
        point tp;
        tp.x = j;
        tp.y = i;
        tp.z = 0;
        outputI[i*centroids.cols*blockSize+j] = tp;
        outputJ[i*centroids.cols*blockSize+j] = tp;
      }
    }
    csa *c = csa_create();
    csa_addpoints(c, centroids.cols*centroids.rows, inputI);
    csa_setnpmin(c, 3);
    csa_setnpmax(c, 20);
    csa_setk(c, 200);
    // csa_setnppc(c, 10);
    // printf("calculating spline...\n");
    csa_calculatespline(c);
    // printf("done.\n");
    csa_approximatepoints(c, centroids.cols*centroids.rows*blockSize*blockSize, outputI);
    csa_destroy(c);

    csa *d = csa_create();
    csa_addpoints(d, centroids.cols*centroids.rows, inputJ);
    csa_setnpmin(d, 3);
    csa_setnpmax(d, 20);
    csa_setk(d, 200);
    // csa_setnppc(d, 10);
    // printf("calculating spline...\n");
    csa_calculatespline(d);
    // printf("done.\n");
    csa_approximatepoints(d, centroids.cols*centroids.rows*blockSize*blockSize, outputJ);
    csa_destroy(d);

    Mat result = Mat::zeros(Size(centroids.cols*blockSize, centroids.rows*blockSize), CV_16SC2);
    int bigSize = centroids.cols*centroids.rows*blockSize*blockSize;
    for(int idx = 0; idx < bigSize; idx++) {
      int i = outputI[idx].y;
      int j = outputI[idx].x;
      short valI = outputI[idx].z;
      short valJ = outputJ[idx].z;
      result.at<DisparityElemType>(i, j) = DisparityElemType(valI, valJ);
    }
    delete[] inputI;
    delete[] inputJ;
    delete[] outputJ;
    delete[] outputI;
    return result;
  }
  /// applies getModalPoint, getWrongPoints and interpolation on src1 and src2
  /// input images should not be blocky!
  /// blockSize is for region to consider for modal evaluation
  /// most prob(!) src1 is I, src2 is J.
  Mat applyModalFilter(Mat mask, Mat src1, Mat src2, int blockSize) {
    assert(src1.size() == src2.size() && mask.size() == src1.size());
    assert(src1.rows >= blockSize && src1.cols >= blockSize);
    // Mat res = Mat(Size(src1.cols/blockSize, src1.rows/blockSize), CV_16SC2);
    Mat centroids = Mat(Size(src1.cols/blockSize, src1.rows/blockSize), CV_16SC2);
    Mat values = Mat(centroids.size(), CV_16SC2);
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
        Mat roi_mask = mask.rowRange(rowStart, rowEnd).colRange(colStart, colEnd);
        Mat b1, b2;
        roi_src1.convertTo(b1, CV_32FC1);
        roi_src2.convertTo(b2, CV_32FC1);
        Point p = getModalPoint(b1, b2, roi_mask, NULL);
        Point cent = getCentroid(roi_src1, roi_src2, roi_mask, p, 5, 2);
        centroids.at<DisparityElemType>(i/blockSize,j/blockSize) = DisparityElemType((short)cent.y, (short)cent.x);
        values.at<DisparityElemType>(i/blockSize,j/blockSize) = DisparityElemType((short)p.y, (short)p.x);
        // int &dispI = p.y;
        // int &dispJ = p.x;
        // /// Right now not finding centroid. Using the centre of the block itself for interpolation.
        // res.at<DisparityElemType>(i/blockSize, j/blockSize)[0] = dispI;
        // res.at<DisparityElemType>(i/blockSize, j/blockSize)[1] = dispJ;
      }
    }
    // cout << centroids.size() << endl;
    // cout << centroids << endl;
    // cout << values.size() << endl;
    // cout << values << endl;
    // waitKey(0);
    Mat res = interpolate(centroids, values, blockSize);
    // resize(res, res, src1.size(), 0, 0, INTER_LINEAR);
    return res;
  }
}
#endif