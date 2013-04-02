/* Arpit
*/
#include <stdio.h>
#include <highgui/highgui.hpp>
#include <imgproc/imgproc.hpp>
#include <iostream>
using namespace cv;
#define INF 99999999
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

	Mat blockMatching(Mat src1, Mat src2, int blockSize)
	{
		assert(src1.size() == src2.size());
		//need to resize image to add border if blockSize doesnt exactly match.		
		Mat src1_new, src2_new;
		copyMakeBorder(src1, src1_new, 0, blockSize - src1.size().height%blockSize, 0, blockSize - src1.size().width%blockSize, BORDER_REPLICATE);
		copyMakeBorder(src2, src2_new, 0, blockSize - src1.size().height%blockSize, 0, blockSize - src1.size().width%blockSize, BORDER_REPLICATE);
		std::cout << src1_new.size();
		Mat result(src1_new.size(), CV_8UC2); //channel1  = delI, channel2 = delJ
		// char ch1 = result.at<cv::Vec<unsigned char, 2> >(0,0)[0];
		for (int i1 = 0; i1 < src1_new.rows; i1+=blockSize)
		{
			for (int j1 = 0; j1 < src1_new.cols; j1+=blockSize)
			{
				Mat block1 = src1_new.rowRange(i1, i1+blockSize-1).colRange(j1, j1+blockSize-1);
				//block in 2nd image searched throughout: may take time...
				int minDelI=INF, minDelJ=INF;
				float minDiff=INF;
				for (int i2 = 0; i2 < src1_new.rows-blockSize; ++i2)
				{
					for (int j2 = 0; j2 < src1_new.cols-blockSize; ++j2)
					{
						Mat block2 = src2_new.rowRange(i2, i2+blockSize-1).colRange(j2, j2+blockSize-1);
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
						result.at<cv::Vec<unsigned char, 2> >(i,j)[0] = abs(minDelI);
						result.at<cv::Vec<unsigned char, 2> >(i,j)[1] = abs(minDelJ);
						// std::cout << 
					}
				}
			}
		}
		return result;
	}
}

int main(int argc, char const *argv[])
{
	Mat img1 = imread("resized-images/fore4.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	Mat img2 = imread("resized-images/aft4.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	Mat result = BlockMatching::blockMatching(img1, img2, 5);
	vector<Mat> planes;
	split(result, planes);
	assert(planes.size() == 2);
	imshow("i", planes[0]);
	imshow("j", planes[1]);
	waitKey(0);
	return 0;
}