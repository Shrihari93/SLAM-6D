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

typedef Vec2f DisparityElemType;
static const int BLOCKSIZE = 30;
static const int BLOCK_MULT = 5;
static const int WINDOWSIZE = BLOCKSIZE * BLOCK_MULT;

struct MouseParam //For Debugging purposes
{
	Mat src, disparity, display1, display2;
	int blockSize;
	std::string windowName1, windowName2;
};
struct MouseParam2 //For Debugging purposes
{
	Mat src1, src2;
	int blockSize;
	std::string windowName1, windowName2;
};
namespace BlockMatching
{
	/*[1]*/
	double sumOfSquaresDiffScaled(const Mat m1, const Mat m2)
	{
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
	/*[2]*/
	double correlation(const Mat m1, const Mat m2)
	{
		assert(m1.size() == m2.size());
		assert(m1.type() == CV_8UC1 && m2.type() == CV_8UC1);
		//variance 1
	}
	cv::Rect getBlockWindow(Mat src, int i, int j, int blockSize)
	{
		assert(blockSize > 0);
		int i1 = i- i%blockSize;
		int j1 = j- j%blockSize;
		assert(i1 < src.rows && j1 < src.cols);
		return cv::Rect(j1, i1, blockSize, blockSize);
	}
	Mat blockMatching(Mat src1, Mat src2, int blockSize)
	{
		assert(src1.size() == src2.size());
		//need to resize image to add border if blockSize doesnt exactly match.		
		Mat src1_new, src2_new;
		copyMakeBorder(src1, src1_new, 0, blockSize - src1.size().height%blockSize, 0, blockSize - src1.size().width%blockSize, BORDER_REPLICATE);
		copyMakeBorder(src2, src2_new, 0, blockSize - src1.size().height%blockSize, 0, blockSize - src1.size().width%blockSize, BORDER_REPLICATE);
		std::cout << src1_new.size();
		Mat result(src1_new.size(), CV_32FC2); //channel1  = delI, channel2 = delJ
		// char ch1 = result.at<cv::Vec<unsigned char, 2> >(0,0)[0];
		for (int i1 = 0; i1 < src1_new.rows; i1+=blockSize)
		{
			for (int j1 = 0; j1 < src1_new.cols; j1+=blockSize)
			{
				Mat block1 = src1_new.rowRange(i1, i1+blockSize).colRange(j1, j1+blockSize);
				//block in 2nd image searched throughout: may take time...
				int minDelI=INF, minDelJ=INF;
				float minDiff=INF;
				for (int i2 = 0; i2 < src1_new.rows-blockSize; ++i2)
				{
					for (int j2 = 0; j2 < src1_new.cols-blockSize; ++j2)
					{
						Mat block2 = src2_new.rowRange(i2, i2+blockSize).colRange(j2, j2+blockSize);
						//calc diff, using [1]
						double diff = sumOfSquaresDiffScaled(block1, block2);
						if(diff < minDiff)
						{
							minDiff = diff;
							minDelI = i2-i1;
							minDelJ = j2-j1;
						}
					}
				}
				//assuming atleast 1 minDiff was found < INF...
				for (int i = i1; i < i1+blockSize; ++i)
				{
					for (int j = j1; j < j1+blockSize; ++j)
					{
						result.at<DisparityElemType >(i,j)[0] = (minDelI);
						result.at<DisparityElemType >(i,j)[1] = (minDelJ);
					}
				}
			}
		}
		return result;
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
	Rect matchABlock2(Mat block1, Mat dst, int match_method = CV_TM_CCOEFF_NORMED)
	{
		static int match_cols = 2*WINDOWSIZE + BLOCKSIZE - BLOCKSIZE + 1;
		static int match_rows = 2*WINDOWSIZE + BLOCKSIZE - BLOCKSIZE + 1;	
		static Mat result(match_cols, match_rows, CV_32FC1);
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
}
void mouseCallback(int evt, int x, int y, int flags, void *param)//Correct orr not?
{
	if(evt == CV_EVENT_LBUTTONDOWN)
	{		
		MouseParam *mp = static_cast<MouseParam*>(param);
		if(!mp)
			return;		
		cv::Rect rect = BlockMatching::getBlockWindow(mp->src, y, x, mp->blockSize);
		Mat display1 = mp->display1.clone();
		Mat display2 = mp->display2.clone();
		cout << rect;
		cv::rectangle(display1, rect, Scalar(0,0,255));
		rect.x += mp->disparity.at<DisparityElemType>(y, x)[1];
		rect.y += mp->disparity.at<DisparityElemType >(y, x)[0];
		cv::rectangle(display2, rect, Scalar(255, 0, 0));
		cout << rect << endl;
		imshow(mp->windowName1.c_str(), display1);
		imshow(mp->windowName2.c_str(), display2);
	}
}

void mouseCallback2(int evt, int x, int y, int flags, void *param)//Correct orr not?
{
	//assuming src1  and src2 already padded at boundaries!
	if(evt == CV_EVENT_LBUTTONDOWN)
	{		
		MouseParam2 *mp = static_cast<MouseParam2*>(param);
		if(!mp)
			return;		
		
		cv::Rect rect(x, y, mp->blockSize, mp->blockSize);
		Mat display1 = mp->src1.clone();
		Mat display2 = mp->src2.clone();
		cout << rect;
		cv::rectangle(display1, rect, Scalar(0,0,255));
		Mat block = mp->src1.rowRange(y, y+mp->blockSize).colRange(x, x+mp->blockSize);
		//defining roi for search in src2
		int rowStart = std::max(0, y-WINDOWSIZE);
		int rowEnd = std::min(mp->src2.rows, y+WINDOWSIZE);
		int colStart = std::max(0, x-WINDOWSIZE);
		int colEnd = std::min(mp->src2.cols, x+WINDOWSIZE);
		Rect roi(colStart, rowStart,  colEnd-colStart, rowEnd-rowStart);
		cv::rectangle(display2, roi, Scalar(128));
		Mat roi_src2 = mp->src2.rowRange(rowStart, rowEnd).colRange(colStart, colEnd);
		//searching only in roi
		Rect rect2 = BlockMatching::matchABlock2(block, roi_src2);
		cv::rectangle(display2, rect2, Scalar(255, 0, 0));
		cout << rect2 << endl;
		imshow(mp->windowName1.c_str(), display1);
		imshow(mp->windowName2.c_str(), display2);
	}
}

int main(int argc, char const *argv[])
{
	Mat img1 = imread(argv[1],CV_LOAD_IMAGE_GRAYSCALE);
	Mat img2 = imread(argv[2],CV_LOAD_IMAGE_GRAYSCALE);
	/*Mat result = BlockMatching::blockMatching(img1, img2, BLOCKSIZE);
	vector<Mat> planes;
	split(result, planes);
	assert(planes.size() == 2);
	imshow("i", planes[0]);
	imshow("j", planes[1]);
	cv::namedWindow(WINDOWNAME1);
	cv::namedWindow(WINDOWNAME2);
	MouseParam mp;
	mp.src = img1;
	mp.disparity = result;
	mp.blockSize = BLOCKSIZE;
	mp.windowName1 = WINDOWNAME1;
	mp.windowName2 = WINDOWNAME2;
	mp.display1 = img1;
	mp.display2 = img2;
	cv::setMouseCallback(WINDOWNAME1, mouseCallback, &mp);
	imshow(WINDOWNAME1, img1);
	imshow(WINDOWNAME2, img2);*/
	Mat img1_new, img2_new;
	copyMakeBorder(img1, img1_new, 0, BLOCKSIZE - img1.rows%BLOCKSIZE, 0, BLOCKSIZE - img1.cols%BLOCKSIZE, BORDER_REPLICATE);
	copyMakeBorder(img2, img2_new, 0, BLOCKSIZE - img2.rows%BLOCKSIZE, 0, BLOCKSIZE - img2.cols%BLOCKSIZE, BORDER_REPLICATE);
	cv::namedWindow(WINDOWNAME1);
	cv::namedWindow(WINDOWNAME2);
	MouseParam2 mp;
	mp.src1 = img1_new;
	mp.src2 = img2_new;
	mp.blockSize = BLOCKSIZE;
	mp.windowName1 = WINDOWNAME1;
	mp.windowName2 = WINDOWNAME2;
	cv::setMouseCallback(WINDOWNAME1, mouseCallback2, &mp);
	imshow(WINDOWNAME1, img1);
	imshow(WINDOWNAME2, img2);
	while(waitKey(0) != 'q'){
		cout << "HERE\n";
	}
	return 0;
}