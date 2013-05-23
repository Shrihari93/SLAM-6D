/* Arpit
*/
#include <stdio.h>
#include <highgui/highgui.hpp>
#include <imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
using namespace cv;
using namespace std;
namespace Interpolation {
	int neighbours[][2] = { {-1,-1},
							{-1, 0},
							{-1, 1},
							{0, -1},
							{0, +1},
							{1, -1},
							{+1, 0},
							{+1, 1}};
	struct IPoint {
		int i, j, val;
		IPoint(int i, int j, int val):i(i), j(j), val(val) {}
		//reverse sort. not using val at all ><
		bool operator <(const IPoint &b) const {
			if(i < b.i)
				return true;
			else if(i > b.i)
				return false;
			else return j < b.j;
		}
	};
	template <class T>
	void filterThreshold(Mat &src, T mean, T sd) {
		for (int i = 0; i < src.rows; ++i)	{
			for (int j = 0; j < src.cols; ++j)	{
				double val = (double)src.at<T>(i,j) - (double)mean; //assuming T may be typecast from/to val!
				src.at<T>(i,j) = val > 0? val:-val;
				if((val)*(val) < (double)sd*(double)sd) {
					src.at<T>(i,j) = 0;
				}
			}
		}
	}
	/// Elmt of src is of type T, invalids is CV_8UC1
	/// invalids is non-zero for all (i,j) that are invalid in src
	/// the larger the non-zero value, the more invalid it is.
	/// interpolates for all invalids.
	template <class T>
	Mat interpolate(Mat src, Mat invalids, int blockSize) {
		assert(invalids.size() == src.size());
		assert(blockSize > 0);
		Mat dst = Mat(src.rows/blockSize, src.cols/blockSize, src.type());
		set<IPoint> invalidList;
		for (int i = 0; i < dst.rows; ++i)
		{
			for (int j = 0; j < dst.cols; ++j)
			{
				if(!invalids.at<uchar>(i*blockSize,j*blockSize))
					dst.at<T>(i,j) = src.at<T>(i*blockSize, j*blockSize);
				else
					invalidList.insert(IPoint(i,j,invalids.at<uchar>(i*blockSize,j*blockSize)));
			}
		}
		int iter = 0;
		while(invalidList.size()) {
			++iter;
			for (set<IPoint>::iterator k = invalidList.begin(); k != invalidList.end(); ++k)
			{
				int i = k->i;
				int j = k->j;
				double sum = 0; /// assuming T may be typecast from/to double!
				int count = 0;
				for (int l = 0; l < 8; ++l)
				{
					int ni = (i+neighbours[l][0]);
					int nj = (j+neighbours[l][1]);
					if(ni < 0 || ni >= dst.rows || nj < 0 || nj >= dst.cols)
						continue;
					if(invalidList.find(IPoint(ni,nj,0)) == invalidList.end()) {
						count++;
						sum += dst.at<T>(ni, nj);
					}
				}
				if(count > 2) {
					/// Some neighbour(s) valid. removing from invalid list.
					invalidList.erase(k);
					dst.at<T>(i,j) = (T)(sum/count);
				}
			}
		}
		printf("num iters = %d\n", iter);
		resize(dst, dst, src.size(), 0, 0, INTER_NEAREST);
		return dst;
	}
	/// uses sobel to find peaks on surface, applies interpolation.
	template <class T>
	Mat smooth(Mat &src, int blockSize) {
		assert(blockSize > 0);
		cv::Size origSize = src.size();
		resize(src, src, cv::Size(src.cols/blockSize, src.rows/blockSize), 0, 0, INTER_NEAREST);
		Mat invalids;// = Mat::zeros(src.rows, src.cols, CV_8UC1);
		cornerHarris(src, invalids, 2, 3, 0.04);
		normalize( invalids, invalids, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
		convertScaleAbs(invalids, invalids);

		Mat invalidsx, invalidsy;
		Sobel(invalids, invalidsx, CV_16S, 1, 0, 3);
		Sobel(invalids, invalidsy, CV_16S, 0, 1, 3);
		invalids = invalidsx+invalidsy;
		convertScaleAbs(invalids, invalids);
		
		filterThreshold<uchar>(invalids, 0, 10);
		Mat dst = interpolate<T>(src, invalids, 1);
		resize(dst, dst, origSize, 0, 0, INTER_NEAREST);
		resize(src, src, origSize, 0, 0, INTER_NEAREST);
		resize(invalids, invalids, origSize, 0, 0, INTER_NEAREST);
		imshow("sobel result, filtered", invalids);
		waitKey(0);
		return dst;
	}
}

