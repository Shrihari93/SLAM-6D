#ifndef BLOCKMATCHING_HPP
#define BLOCKMATCHING_HPP
/* Arpit
*/
#include <stdio.h>
#include <highgui/highgui.hpp>
#include <imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
using namespace cv;
using namespace std;
#define INF 99999999
#define WINDOWNAME1 "Display1"
#define WINDOWNAME2 "DIsplay2"

typedef Vec2s DisparityElemType;
static const float BLOCK_MULT_Y = 1.9;
static const float BLOCK_MULT_X = 1.0;
namespace BlockMatching
{
  /* implements opencv template matching */
  Rect matchABlock2(Mat block1, Mat dst, Mat result, int match_method = CV_TM_CCOEFF_NORMED)
  {
    assert(block1.rows <= dst.rows && block1.cols <= dst.cols);
    assert(result.cols >= dst.cols - block1.cols+1 && dst.rows >= dst.rows - block1.rows+1);
    Size blockSize = block1.size();
    //Assuming result already exists! and size is ok!
    // Do the Matching and Normalize
    matchTemplate( dst, block1, result, match_method );
    normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );
    double minVal; double maxVal; Point minLoc; Point maxLoc;
      Point matchLoc;
      minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
      /// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
      if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED )
      { matchLoc = minLoc; }
      else
      { matchLoc = maxLoc; }
      Size size; Point ofs;
    dst.locateROI(size, ofs);
    return Rect(ofs.x+matchLoc.x, ofs.y+matchLoc.y, blockSize.width, blockSize.height);
  }
  void sanitize(int &x, int min, int max) {
    assert(min <= max);
    if(x<min)
      x = min;
    else if(x > max)
      x = max;
  }
  /// Given a mask, input1, input2, prevStage, blockSize, isStartStage : returns an output which has disparity only for the mask points.
  /// The blockSIze block is centered around the mask point.
  /// Mask: CV_8UC1
  /// current: DisparityElemType
  /// src1, src2: assumed CV_8UC1

  /// result: DisparityElemType
  Mat blockMatching(Mat mask, Mat src1, Mat src2, Mat& current, int blockSize, bool isStartStage, int shift)
  {
    assert(src1.size() == src2.size() && mask.size() == src1.size() && current.size() == src1.size());
    assert(src1.type() == CV_8UC1 && src2.type() == CV_8UC1);
    int BLOCKSIZE = blockSize;
    int WINDOWSIZE_Y = (BLOCKSIZE * BLOCK_MULT_Y);
    int WINDOWSIZE_X = (BLOCKSIZE * BLOCK_MULT_X);
    if(isStartStage) {
      // WINDOWSIZE_Y *= 1.5;
    }
    Mat result = Mat::zeros(src1.size(), CV_16SC2);
    ///Initializing mat used for storage by matchABlock2.
    int match_cols = 2*WINDOWSIZE_X + 1;
    int match_rows = 2*WINDOWSIZE_Y + 1;
    Mat matchingStorage(match_cols, match_rows, CV_32FC1);
    for (int i1 = 0; i1 < src1.rows; ++i1)
    {
      for (int j1 = 0; j1 < src1.cols; ++j1)
      {
        if(!mask.at<uchar>(i1, j1))
          continue;
        //created block from image 1
        int rowStart = std::max(0, i1-BLOCKSIZE/2);
        int rowEnd = std::min(src1.rows, i1+BLOCKSIZE/2);
        int colStart = std::max(0, j1-BLOCKSIZE/2);
        int colEnd = std::min(src1.cols, j1+BLOCKSIZE/2);
        Mat block1 = src1.rowRange(rowStart, rowEnd).colRange(colStart, colEnd);
        //assuming shiftI and shiftJ from previous stage
        int shiftI = current.at<DisparityElemType>(i1,j1)[0];
        int shiftJ = current.at<DisparityElemType>(i1,j1)[1];
        //defining roi for search in src2
        int cntrI = i1+shiftI;
        int cntrJ = j1+shiftJ;
        if(isStartStage)
          cntrJ += shift;
        sanitize(cntrI, 0, src1.rows-BLOCKSIZE);
        sanitize(cntrJ, 0, src1.cols-1);
        //the start rows/cols and end rows/cols
        rowStart = std::max(0, cntrI-BLOCKSIZE);
        rowEnd = std::min(src2.rows, cntrI+WINDOWSIZE_Y);
        colStart = std::max(0, cntrJ-WINDOWSIZE_X);
        colEnd = std::min(src2.cols, cntrJ+WINDOWSIZE_X);
        Mat roi_src2 = src2.rowRange(rowStart, rowEnd).colRange(colStart, colEnd);
        //finding matched rect
        Rect rect = matchABlock2(block1, roi_src2, matchingStorage);
        /// disparity
        float delJ = (rect.x+rect.width/2 - j1);
        float delI = (rect.y+rect.height/2 - i1);
        result.at<DisparityElemType >(i1,j1)[0] = (delI);
        result.at<DisparityElemType >(i1,j1)[1] = (delJ);
      }
    }
    return result;
  }
 
}


#endif