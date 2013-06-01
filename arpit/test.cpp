#include "filtering.hpp"
#include <initializer_list>
int main(int argc, char const *argv[])
{
  // Mat img1 = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
  // Mat img2 = imread(argv[2], CV_LOAD_IMAGE_GRAYSCALE);
  float data1[] = {-2,-2,3,
                   -1,0,4,
                   -1,2,2};
  float data2[] = {3,4,5,
                   3,3,6,
                   3,2,1};
  Mat img1(3,3,CV_32F, data1);
  Mat img2(3,3,CV_32F, data2);
  cout << img1 << endl;
  cout << img2 << endl;
  Point p = Filter::getModalPoint(img1, img2, NULL);
  cout << p << endl;
  vector<Mat> mArr;
  mArr.push_back(img1); mArr.push_back(img2);
  Mat dst;
  merge(mArr, dst);
  std::vector<Point> wrongPoints = Filter::getWrongPoints<Vec2f>(p.y, p.x, dst, 2, 2);
  cout << "Wrong Points: ";
  for (int i = 0; i < wrongPoints.size(); ++i)
  {
    cout << wrongPoints[i] << " ";
  }
  cout << endl;
  mArr.clear();
  split(dst, mArr);
  Filter::nnInterpolation<float>(wrongPoints, mArr[0]);
  Filter::nnInterpolation<float>(wrongPoints, mArr[1]);
  cout << mArr[0] << endl;
  cout << mArr[1] << endl;
  return 0;
}