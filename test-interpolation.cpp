#include "interpolation.hpp"

int main(int argc, char const *argv[])
{
	if(argc != 4) {
		printf("Usage: ./test-interpolation <src> <invalids> <block-size>\n");
		return 0;
	}
	Mat img = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	Mat invalids = imread(argv[2], CV_LOAD_IMAGE_GRAYSCALE);
	int blockSize = atoi(argv[3]);
	Interpolation::filterThreshold<uchar>(invalids, 4, 13);
	// Mat invalids = Mat::zeros(img.rows, img.cols, CV_8UC1);

	// Mat inter = Interpolation::interpolate<uchar>(img, invalids, blockSize);
	// Mat smoothed = Interpolation::smooth<uchar>(inter, blockSize);
	/// instead of ^ two lines, doing just smooth here:
	Mat smoothed = Interpolation::smooth<uchar>(img, blockSize);

	// imshow("1", inter);
	imshow("2", smoothed);
	imwrite("final.png", smoothed);
	// imshow("3")
	waitKey(0);
	return 0;
}