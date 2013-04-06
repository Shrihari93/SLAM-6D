/* Arpit
*/
#include <stdio.h>
#include <highgui/highgui.hpp>
#include <imgproc/imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;
#define INF 99999999
#define WINDOWNAME1 "Display1"
#define WINDOWNAME2 "DIsplay2"

typedef Vec2b DisparityElemType;
// All blocksize/windowsize definitions here global.
static const int BLOCKSIZE = 20;
static const int BLOCK_MULT_Y = 4;
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

	Rect matchABlock(Mat block1, Mat dst)//block1 must be square and < dst size. rect returned from parent image of dst
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
	Mat blockMatching(Mat src1, Mat src2)
	{
		assert(src1.size() == src2.size());
		//need to resize image to add border if BLOCKSIZE doesnt exactly match.		
		Mat src1_new, src2_new;
		copyMakeBorder(src1, src1_new, 0,  (-src1.size().height)%BLOCKSIZE, 0, (-src1.size().width)%BLOCKSIZE, BORDER_REPLICATE);
		copyMakeBorder(src2, src2_new, 0,  (-src1.size().height)%BLOCKSIZE, 0, (-src1.size().width)%BLOCKSIZE, BORDER_REPLICATE);
		Mat result(src1_new.size(), CV_8UC2); //channel1  = delI, channel2 = delJ
		for (int i1 = 0; i1 < src1_new.rows; i1+=BLOCKSIZE)
		{
			for (int j1 = 0; j1 < src1_new.cols; j1+=BLOCKSIZE)
			{
				//created block from image 1
				Mat block1 = src1_new.rowRange(i1, i1+BLOCKSIZE).colRange(j1, j1+BLOCKSIZE);
				//defining roi for search in src2_new
				int rowStart = std::max(0, i1-WINDOWSIZE_Y);
				int rowEnd = std::min(src2_new.rows, i1+WINDOWSIZE_Y);
				int colStart = std::max(0, j1-WINDOWSIZE_X);
				int colEnd = std::min(src2_new.cols, j1+WINDOWSIZE_X);
				Mat roi_src2 = src2_new.rowRange(rowStart, rowEnd).colRange(colStart, colEnd);
				//finding matched rect
				Rect rect = matchABlock(block1, roi_src2);
				//using only shift... else is pretty useless since right now rect width and height are fixed at BLOCKSIZE
				int delJ = abs(rect.x - j1);
				int delI = abs(rect.y - i1); 
				if(delI > 70)
					delI = 0;
				if(delJ > 70)
					delJ = 0;
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
}

int main(int argc, char const *argv[])
{
	Mat img1 = imread("bandaac-scaled.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	Mat img2 = imread("bandffc-scaled.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	cv::normalize(img1, img1, 0, 255, CV_MINMAX);	
	cv::normalize(img2, img2, 0, 255, CV_MINMAX);
	Mat result = BlockMatching::blockMatching(img1, img2);
	vector<Mat> planes;
	split(result, planes);
	assert(planes.size() == 2);
	imshow("i", planes[0]);
	imshow("j", planes[1]);
	cv::waitKey();
	return 0;
}