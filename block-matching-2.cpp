/* Arpit
*/
#include <stdio.h>
#include <highgui/highgui.hpp>
#include <imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
using namespace cv;
using namespace std;
#define INF 99999999
#define WINDOWNAME1 "Display1"
#define WINDOWNAME2 "DIsplay2"


typedef Vec2s DisparityElemType;
// All blocksize/windowsize definitions here global.
static const int BLOCKSIZE = 40;
static const int BLOCK_MULT_Y = 2;
static const int BLOCK_MULT_X = 2;
static const int WINDOWSIZE_Y = BLOCKSIZE * BLOCK_MULT_Y;
static const int WINDOWSIZE_X = BLOCKSIZE * BLOCK_MULT_X;

namespace BlockMatching
{
	/*[1]*/
	double sumOfSquaresDiffScaled(const Mat m1, const Mat m2)
	{
		/* Better would be to normalize submatrices HERE rather than globally (possibly more accurate, but more computation1)
		*/
		/*Mat m1 = m1_orig.clone();		
		Mat m2 = m2_orig.clone();
		cv::normalize(m1, m1, 0, 255, CV_MINMAX);
		cv::normalize(m2, m2, 0, 255, CV_MINMAX);*/
		assert(m1.size() == m2.size());
		assert(m1.type() == CV_8UC1 && m2.type() == CV_8UC1);
		float result = 0, sum = 0;
		for (int i = 0; i < m1.rows; ++i)
		{
			for (int j = 0; j < m1.cols; ++j)
			{
				int v1 = m1.at<unsigned char>(i,j);
				int v2 = m2.at<unsigned char>(i,j);
				result += (v1-v2)*(v1-v2);
				sum += v1;
			}
		}
		return result/(sum*sum);
	}
	/* implements [1] */
	Rect matchABlock1(Mat block1, Mat dst, Mat = Mat(), int = 0)//block1 must be square and < dst size. rect returned from parent image of dst
	{
		assert(block1.rows == block1.cols);
		int blockSize = block1.rows;
		int minDelI=INF, minDelJ=INF;
		float minDiff=INF;
		for (int i2 = 0; i2 < dst.rows - blockSize; ++i2)
		{
			for (int j2 = 0; j2 < dst.cols-blockSize; ++j2)
			{
				Mat block2 = dst.rowRange(i2, i2+blockSize).colRange(j2, j2+blockSize);
				//calc diff, using [1]
				double diff = sumOfSquaresDiffScaled(block1, block2);
				if(diff < minDiff)
				{
					minDiff = diff;
					minDelI = i2-0;
					minDelJ = j2-0;
				}
			}
		}
		Size size; Point ofs;
		dst.locateROI(size, ofs);
		return Rect(ofs.x+minDelJ, ofs.y+minDelI, blockSize, blockSize);
	}
	/* implements opencv template matching */
	Rect matchABlock2(Mat block1, Mat dst, Mat result, int match_method = CV_TM_CCOEFF_NORMED)
	{
		assert(block1.rows == block1.cols);
		int blockSize = block1.rows;
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
		return Rect(ofs.x+matchLoc.x, ofs.y+matchLoc.y, blockSize, blockSize);
	}
	void sanitize(int &x, int min, int max) {
		assert(min <= max);
		if(x<min)
			x = min;
		else if(x > max)
			x = max;
	}
	Mat blockMatching(Mat src1, Mat src2, int shiftI = 0, int shiftJ = 0)
	{
		assert(src1.size() == src2.size());
		///need to resize image to add border if BLOCKSIZE doesnt exactly match.		
		Mat src1_new, src2_new;
		copyMakeBorder(src1, src1_new, 0,  -((-src1.size().height)%BLOCKSIZE), 0, -((-src1.size().width)%BLOCKSIZE), BORDER_REPLICATE);
		copyMakeBorder(src2, src2_new, 0,  -((-src1.size().height)%BLOCKSIZE), 0, -((-src1.size().width)%BLOCKSIZE), BORDER_REPLICATE);
		Mat result(src1_new.size(), CV_16SC2); //channel1  = delI, channel2 = delJ
		///Initializing mat used for storage by matchABlock2.
		int match_cols = 2*WINDOWSIZE_X + BLOCKSIZE - BLOCKSIZE + 1;
		int match_rows = 2*WINDOWSIZE_Y + BLOCKSIZE - BLOCKSIZE + 1;		
		Mat matchingStorage(match_cols, match_rows, CV_32FC1);
		for (int i1 = 0; i1 < src1_new.rows-BLOCKSIZE+1; i1+=BLOCKSIZE)
		{
			for (int j1 = 0; j1 < src1_new.cols-BLOCKSIZE+1; j1+=BLOCKSIZE)
			{
				//created block from image 1
				Mat block1 = src1_new.rowRange(i1, i1+BLOCKSIZE).colRange(j1, j1+BLOCKSIZE);
				//defining roi for search in src2_new
				int cntrI = i1+shiftI;
				int cntrJ = j1+shiftJ;
				sanitize(cntrI, 0, src1_new.rows-1);
				sanitize(cntrJ, 0, src1_new.cols-1);
				//the start rows/cols and end rows/cols
				int rowStart = std::max(0, cntrI-WINDOWSIZE_Y);
				int rowEnd = std::min(src2_new.rows, cntrI+WINDOWSIZE_Y);
				int colStart = std::max(0, cntrJ-WINDOWSIZE_X);
				int colEnd = std::min(src2_new.cols, cntrJ+WINDOWSIZE_X);
				Mat roi_src2 = src2_new.rowRange(rowStart, rowEnd).colRange(colStart, colEnd);
				//finding matched rect
				Rect rect = matchABlock2(block1, roi_src2, matchingStorage);
				//using only shift... else is pretty useless since right now rect width and height are fixed at BLOCKSIZE
				int delJ = abs(rect.x - j1);
				int delI = abs(rect.y - i1); 
				//basic outlier removal
				// if(delI > 30)
				// 	delI = 0;
				// if(delJ > 30)
				// 	delJ = 0;
				for (int i = i1; i < i1+BLOCKSIZE; ++i)
				{
					for (int j = j1; j < j1+BLOCKSIZE; ++j)
					{
						result.at<DisparityElemType >(i,j)[0] = delI;
						result.at<DisparityElemType >(i,j)[1] = delJ;
					}
				}				
			}
		}
		return result;
	}
	void outlierRemoval(Mat &result) {
		
	}
	Mat blockMatchingLeveled(Mat &disparityPrev, Mat src1, Mat src2)
	{
		/// assuming resolution of prev  is EXACTLY half of current;
		assert(src1.size() == src2.size());
		/// not implemented
	}
}

int main(int argc, char const *argv[])
{
	if(argc < 3 || argc > 4) {
		printf("Usage: ./block-matching-2 <image1> <image2>\n");
		printf("Optionally: ./block-matching-2 <image1> <image2> <orsa-match-file>\n");
		exit(0);
	}
	Mat img1 = imread(argv[1],CV_LOAD_IMAGE_GRAYSCALE);
	Mat img2 = imread(argv[2],CV_LOAD_IMAGE_GRAYSCALE);
	int shiftI = 0, shiftJ = 0;
	//Loading orsa matches
	if(argc == 4) {
		string txtFile = argv[3];
		std::ifstream f(txtFile.c_str(), ios::in);
		int numMatches = 0;
		f >> numMatches;
		float x1, x2, y1, y2, sumX = 0, sumY = 0;
		for (int i = 0; i < numMatches; ++i)
		{
			f >> x1 >> y1 >> x2 >> y2 >> ws; //assuming order of images in orsa run is same as here.
			sumX += x2 - x1;
			sumY += y2 - y1;
		}
		if(numMatches > 0) {
			shiftI = sumY/numMatches;
			shiftJ = sumX/numMatches;
		}
		f.close();
	}
	printf("shiftI, shiftJ = %d %d\n", shiftI, shiftJ);
	cv::normalize(img1, img1, 0, 255, CV_MINMAX);	
	cv::normalize(img2, img2, 0, 255, CV_MINMAX);
	Mat result = BlockMatching::blockMatching(img1, img2, shiftI, shiftJ);
	vector<Mat> planes;
	split(result, planes);
	assert(planes.size() == 2);
	// cv::normalize(planes[0], planes[0], 0, 255, CV_MINMAX);
	// cv::normalize(planes[1], planes[1], 0, 255, CV_MINMAX);
	imshow("i", planes[0]);
	imshow("j", planes[1]);
	imwrite("outi.png", planes[0]);
	imwrite("outj.png", planes[1]);
	cv::waitKey();
	return 0;
}