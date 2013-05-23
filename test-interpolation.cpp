#include "interpolation.hpp"

int main(int argc, char const *argv[])
{
	Mat img = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	Mat invalids = imread(argv[2], CV_LOAD_IMAGE_GRAYSCALE);
	Interpolation::filterThreshold<uchar>(invalids, 4, 13);
	// Mat invalids = Mat::zeros(img.rows, img.cols, CV_8UC1);
	Mat inter = Interpolation::interpolate<uchar>(img, invalids, 40);
	Mat smoothed = Interpolation::smooth<uchar>(inter, 40);
	imshow("1", inter);
	imshow("2", smoothed);
	// imshow("3")
	waitKey(0);
	return 0;
}