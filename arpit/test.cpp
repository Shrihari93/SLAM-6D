#include "filtering.hpp"
#include "pyramid.hpp"
#include <initializer_list>
/// WOrks! xD
/*int main(int argc, char const *argv[])
{
  Mat img1 = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
  Mat img2 = imread(argv[2], CV_LOAD_IMAGE_GRAYSCALE);
  resize(img1, img1, Size(img1.cols/20, img1.cols/20), 0, 0);
  resize(img2, img2, Size(img2.cols/20, img2.cols/20), 0, 0);
  // float data1[] = {-2,-2,3,-1, 2,1,
  //                  -1, 0,4, 3, 4,3,
  //                  -1, 2,2,-1,-1,1};
  // float data2[] = {3,4,5,3,4,5,
  //                  3,3,6,4,5,3,
  //                  3,2,1,3,3,4};
  // Mat img1(3,6,CV_32F, data1);
  // Mat img2(3,6,CV_32F, data2);
  // cout << img1 << endl;
  // cout << img2 << endl;
  // Point p = Filter::getModalPoint(img1, img2, NULL);
  // cout << p << endl;
  // vector<Mat> mArr;
  // mArr.push_back(img1); mArr.push_back(img2);
  // Mat dst;
  // merge(mArr, dst);
  // std::vector<Point> wrongPoints = Filter::getWrongPoints<Vec2f>(p.y, p.x, dst, 2, 2);
  // cout << "Wrong Points: ";
  // for (int i = 0; i < wrongPoints.size(); ++i)
  // {
  //   cout << wrongPoints[i] << " ";
  // }
  // cout << endl;
  // mArr.clear();
  // split(dst, mArr);
  // Filter::nnInterpolation<float>(wrongPoints, mArr[0]);
  // Filter::nnInterpolation<float>(wrongPoints, mArr[1]);
  // cout << mArr[0] << endl;
  // cout << mArr[1] << endl;
  Filter::applyModalFilter<uchar>(img1, img2, 10, 40.0, 40.0);
  resize(img1, img1, Size(img1.cols*20, img1.cols*20), 0, 0, INTER_NEAREST);
  resize(img2, img2, Size(img2.cols*20, img2.cols*20), 0, 0, INTER_NEAREST);
  imshow("1", img1);
  imshow("2", img2);
  waitKey();
  // cout << img1 << endl;
  // cout << img2 << endl;
  return 0;
}*/

int main(int argc, char const *argv[])
{
  Mat img1 = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
  Mat img2 = imread(argv[2], CV_LOAD_IMAGE_GRAYSCALE);
  Mat res = BlockMatching::pyramidalMatching(img1, img2, 0);
  vector<Mat> planes;
  split(res, planes);
  assert(planes.size() == 2);
  imwrite("outi-new.png", planes[0]);
  imwrite("outj-new.png", planes[1]);
  convertScaleAbs(planes[0], planes[0]);
  convertScaleAbs(planes[1], planes[1]);
  imshow("1", planes[0]);
  imshow("2", planes[1]);
  waitKey();
  return 0;
}