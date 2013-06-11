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
using namespace cv;
using namespace std;
#define INF 99999999
#define WINDOWNAME1 "Display1"
#define WINDOWNAME2 "DIsplay2"


typedef Vec2s DisparityElemType;
// All blocksize/windowsize definitions here global.
static const int STAGES = 6;
static const int BLOCKSIZES[STAGES] = {320, 160, 80, 40, 20, 10};
static const float BLOCK_MULT_Y = 2;
static const float BLOCK_MULT_X = 1.0;
// #define WINDOWSIZE_Y(i)  (BLOCKSIZES[i] * BLOCK_MULT_Y)
// #define WINDOWSIZE_X(i)  (BLOCKSIZES[i] * BLOCK_MULT_X)

namespace BlockMatching
{
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
	void blockMatching(Mat& src1_new, Mat& src2_new, Mat& current, int stage)
	{
		assert(src1_new.size() == src2_new.size());
		assert(stage >= 0 && stage < STAGES);
		int BLOCKSIZE = BLOCKSIZES[stage];
		int WINDOWSIZE_Y = (BLOCKSIZE * BLOCK_MULT_Y);
		int WINDOWSIZE_X = (BLOCKSIZE * BLOCK_MULT_X);
		///need to resize image to add border if BLOCKSIZE doesnt exactly match.		
		Mat &result = current;
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
				//assuming shiftI and shiftJ from previous stage
				int shiftI = current.at<DisparityElemType>(i1,j1)[0];
				int shiftJ = current.at<DisparityElemType>(i1,j1)[1];
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
				//calculating shift along theta and theta+90deg
				float delJ = (rect.x - j1);
				float delI = (rect.y - i1);

				// float dispTheta = delJ * sin(theta) + delI * cos(theta);
				// float disp90 = delJ * cos(theta) + delI * sin(theta);
				//basic outlier removal
				// if(delI > 30)
				// 	delI = 0;
				// if(delJ > 30)
				// 	delJ = 0;
				for (int i = i1; i < i1+BLOCKSIZE; ++i)
				{
					for (int j = j1; j < j1+BLOCKSIZE; ++j)
					{
						result.at<DisparityElemType >(i,j)[0] = abs(delI);
						result.at<DisparityElemType >(i,j)[1] = abs(delJ);
					}
				}
			}
		}
	}
	/// converts i,j disp to theta,theta+90 disp.
	void convertAlongTheta(Mat &disp, float theta)
	{
		for (int i = 0; i < disp.rows; ++i)
		{
			for (int j = 0; j < disp.cols; ++j)
			{
				float delI = disp.at<DisparityElemType>(i,j)[0];
				float delJ = disp.at<DisparityElemType>(i,j)[1];
				float dispTheta = delJ * sin(theta) + delI * cos(theta);
				float disp90 = delJ * cos(theta) + delI * sin(theta);
				disp.at<DisparityElemType>(i,j)[0] = abs(dispTheta);
				disp.at<DisparityElemType>(i,j)[1] = abs(disp90);
			}
		}
	}
	Mat pyramidalMatching(Mat src1, Mat src2, float theta)
	{
		assert(src1.size() == src2.size());
		Mat result;
		int stage = 0;
		while(BLOCKSIZES[stage] > min(src1.rows, src1.cols))
			stage++;
		Mat src1_new, src2_new;
		assert(stage < STAGES);
		{
			int BLOCKSIZE = BLOCKSIZES[stage];
			int WINDOWSIZE_Y = (BLOCKSIZE * BLOCK_MULT_Y);
			int WINDOWSIZE_X = (BLOCKSIZE * BLOCK_MULT_X);
			///need to resize image to add border if BLOCKSIZE doesnt exactly match.		
			copyMakeBorder(src1, src1_new, 0,  -((-src1.size().height)%BLOCKSIZE), 0, -((-src1.size().width)%BLOCKSIZE), BORDER_REPLICATE);
			copyMakeBorder(src2, src2_new, 0,  -((-src1.size().height)%BLOCKSIZE), 0, -((-src1.size().width)%BLOCKSIZE), BORDER_REPLICATE);
			result = Mat::zeros(src1_new.size(), CV_16SC2); //channel1  = disparity, channel2 = perpendicular
		}
		while(stage < STAGES)
		{
			blockMatching(src1_new, src2_new, result, stage);
			printf("stage %d done...\n", stage);

			vector<Mat> planes;
			// split(result, planes);
			// convertScaleAbs(planes[0], planes[0]);
			// convertScaleAbs(planes[1], planes[1]);
			// imshow("i", planes[0]);
			// imshow("j", planes[1]);
			// waitKey(0);
			// planes[0] = Interpolation::smooth<int>(planes[0], BLOCKSIZES[stage]);
			// planes[1] = Interpolation::smooth<int>(planes[1], BLOCKSIZES[stage]);
			stage++;
		}
		convertAlongTheta(result, theta);
		return result;
	}
}

int main(int argc, char const *argv[])
{
	srand(time(NULL));
	if(argc < 3 || argc > 5) {
		printf("Usage: ./block-matching-pyramid <image1> <image2>\n");
		printf("Optionally: ./block-matching-pyramid <image1> <image2> <orsa-match-file> <angle>\n");
		exit(0);
	}
	Mat img1 = imread(argv[1],CV_LOAD_IMAGE_GRAYSCALE);
	Mat img2 = imread(argv[2],CV_LOAD_IMAGE_GRAYSCALE);
	int shiftI = 0, shiftJ = 0;
	//Loading orsa matches
	if(argc >= 4) {
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
	float angle = 0;
	if(argc == 5) {
		angle = atof(argv[4]);
	}
	printf("shiftI, shiftJ = %d %d\n", shiftI, shiftJ);
	{
		std::ofstream f("shiftLateral.txt", ios::out);
		int lateral = shiftJ * cos(angle) + shiftI * sin(angle);
		f << lateral << endl;
		f.close();
	}
	cv::normalize(img1, img1, 0, 255, CV_MINMAX);	
	cv::normalize(img2, img2, 0, 255, CV_MINMAX);
	Mat result = BlockMatching::pyramidalMatching(img1, img2, angle);
	vector<Mat> planes;
	split(result, planes);
	assert(planes.size() == 2);
	convertScaleAbs(planes[0], planes[0]);
	convertScaleAbs(planes[1], planes[1]);
	// cv::normalize(planes[0], planes[0], 0, 255, CV_MINMAX);
	// cv::normalize(planes[1], planes[1], 0, 255, CV_MINMAX);
	imshow("disp", planes[0]);
	imshow("90", planes[1]);
	imwrite("outi.png", planes[0]);
	imwrite("outj.png", planes[1]);
	cv::waitKey();
	return 0;
}
