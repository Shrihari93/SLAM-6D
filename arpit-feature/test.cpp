#include "corners.hpp"
#include "resize.hpp"
#include "block-matching.hpp"
#include "filtering.hpp"
#include "pyramid.hpp"
#include <assert.h>
int main(int argc, char const *argv[])
{
  Mat img1 = imread(argv[1], 0);
  Mat img2 = imread(argv[2], 0);
  normalize(img1, img1, 0, 255, CV_MINMAX);
  normalize(img2, img2, 0, 255, CV_MINMAX);
  int shiftI = 0, shiftJ = 0;
  assert(img1.size() == img2.size());
  // {
  //   string txtFile = argv[3];
  //   std::ifstream f(txtFile.c_str(), ios::in);
  //   int numMatches = 0;
  //   f >> numMatches;
  //   float x1, x2, y1, y2, sumX = 0, sumY = 0;
  //   for (int i = 0; i < numMatches; ++i)
  //   {
  //     f >> x1 >> y1 >> x2 >> y2 >> ws; //assuming order of images in orsa run is same as here.
  //     sumX += x2 - x1;
  //     sumY += y2 - y1;
  //   }
  //   if(numMatches > 0) {
  //     shiftI = sumY/numMatches;
  //     shiftJ = sumX/numMatches;
  //   }
  //   f.close();
  // }
  // cout << "shiftJ = " << shiftJ << endl; 
  // Mat result = Resize::resizeImage(img, Size(img.cols/3, img.rows/3));
  // Mat mask = Corners::harrisCorner(img1, 0.20*img1.rows*img1.cols);
  // Mat current = Mat::zeros(img1.size(), CV_16SC2);
  // Mat scattered = BlockMatching::blockMatching(mask, img1, img2, current, 25, false);
  Mat result = BlockMatching::pyramidalMatching(img1, img2, shiftJ);
  vector<Mat> planes;
  // split(scattered, planes);
  // assert(planes.size() == 2);
  // Mat result = Filter::applyModalFilter(mask, planes[0], planes[1], 25);
  // planes.clear();
  split(result, planes);
  assert(planes.size() == 2);
  convertScaleAbs(planes[0], planes[0]);
  convertScaleAbs(planes[1], planes[1]);
  imshow("disp", planes[0]);
  imshow("90", planes[1]);
  imwrite("disp.png", planes[0]);
  imwrite("90.png", planes[1]);
  // imshow("1", mask);
  waitKey(0);
  return 0;
}