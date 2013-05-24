#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

/** @function main */
int main ( int argc, char** argv )
{
  /// Declare variables
  Mat src, dst;

  Mat kernel;
  Point anchor;
  double delta;
  int ddepth;
  int kernel_size;
  char* window_name = "filter2D Demo";

  int c;

  /// Load an image
  src = imread( argv[1] );

  if( !src.data )
  { return -1; }

  /// Create window
  namedWindow( window_name, CV_WINDOW_AUTOSIZE );

  /// Initialize arguments for the filter
  anchor = Point( -1, -1 );
  delta = 0;
  ddepth = -1;

  /// Loop - Will filter the image with different kernel sizes each 0.5 seconds
  int ind = 0;
  float data[][3] = { {1,1,1},
                    {1,-8,1},
                    {1,1,1}};
  /// Update kernel size for a normalized box filter
  kernel_size = 3;
  kernel = Mat( kernel_size, kernel_size, CV_32FC1, data)/ (float)(kernel_size*kernel_size);

  /// Apply filter
  filter2D(src, dst, ddepth , kernel, anchor, delta, BORDER_DEFAULT );
  imshow( window_name, dst );
  waitKey(0);
  return 0;
}