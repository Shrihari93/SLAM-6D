#include "resize.hpp"

int main(int argc, char const *argv[])
{
  Mat img = imread(argv[1], 0);
  normalize(img, img, 0, 255, CV_MINMAX);
  resize(img, img, Size(12000, 12000), 0, 0, INTER_CUBIC);
  imwrite("big-resized.png", img);
  return 0;
}