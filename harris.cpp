/*
#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#define IMGDATA(image,i,j,k) ((char*)&image->imageData[(i)*(image->widthStep) + (j)*(image->nChannels) + (k)])
typedef struct _nonzeropoints{
    int X;
    int Y;
    int G;
}non_zero_points;
int main()
{

    
    IplImage* aft1  =cvLoadImage("aft1.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* fore1 =cvLoadImage("fore1.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* aft2 =cvLoadImage("aft2.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* fore2 =cvLoadImage("fore2.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	
	IplImage* aft3 =cvLoadImage("aft3.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* fore3 =cvLoadImage("fore3.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* aft4 =cvLoadImage("aft4.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* fore4 =cvLoadImage("fore4.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	
	IplImage* harris_aft1 =cvCreateImage(cvSize(aft1->width,aft1->height),32,1);
	IplImage* harris_fore1 =cvCreateImage(cvSize(fore1->width,fore1->height),32,1);
	IplImage* harris_aft2 =cvCreateImage(cvSize(aft2->width,aft2->height),32,1);
	IplImage* harris_fore2 =cvCreateImage(cvSize(fore2->width,fore2->height),32,1);
	
	IplImage* harris_aft3 =cvCreateImage(cvSize(aft3->width,aft3->height),32,1);
	IplImage* harris_fore3 =cvCreateImage(cvSize(fore3->width,fore3->height),32,1);
	IplImage* harris_aft4 =cvCreateImage(cvSize(aft4->width,aft4->height),32,1);
	IplImage* harris_fore4 =cvCreateImage(cvSize(fore4->width,fore4->height),32,1);
	//CvArr* harris_aft4=;
	//CvArr* harris_fore4=;
	cvCornerHarris(aft3,harris_aft3,3,3,0.04);
	cvCornerHarris(fore3,harris_fore3,3,3,0.04);
	cvSaveImage("Harris_Afterward3.jpg",harris_aft3);
	cvSaveImage("Harris_Foreward3.jpg",harris_fore3);
	cvNamedWindow("Harris Afterward");
	cvNamedWindow("Harris Foreward");
	cvShowImage("Harris Afterward",harris_aft3);
	cvShowImage("Harris Foreward",harris_fore3);
	cvWaitKey(0);
	cvDestroyAllWindows();
	
	return 0;
}
*/
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <cstdio>
#include <cstdlib>

using namespace cv;
using namespace std;

/// Global variables
Mat src, src_gray;
int thresh = 200;
int max_thresh = 255;

char* source_window = "Source image";
char* corners_window = "Corners detected";

/// Function header
void cornerHarris_demo( int, void* );

/** @function main */
int main(  )
{
  /// Load source image and convert it to gray.
  src=imread("aft4.jpg");
  //src = imread( argv[1], 1 );
  cvtColor( src, src_gray, CV_BGR2GRAY );
  /// Create a window and a trackbar
  namedWindow( source_window, CV_WINDOW_NORMAL);
  createTrackbar( "Threshold: ", source_window, &thresh, max_thresh, cornerHarris_demo );
  imshow( source_window, src );

  //cornerHarris_demo( 0, 0 );

  waitKey(0);
  return(0);
}

/** @function cornerHarris_demo */
void cornerHarris_demo( int, void* )
{

  Mat dst, dst_norm, dst_norm_scaled;
  dst = Mat::zeros( src.size(), CV_32FC1 );

  /// Detector parameters
  int blockSize = 2;
  int apertureSize = 3;
  double k = 0.04;

  /// Detecting corners
  cornerHarris( src_gray, dst, blockSize, apertureSize, k, BORDER_DEFAULT );

  /// Normalizing
  normalize( dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
  convertScaleAbs( dst_norm, dst_norm_scaled );

  /// Drawing a circle around corners
  for( int j = 0; j < dst_norm.rows ; j++ )
     { for( int i = 0; i < dst_norm.cols; i++ )
          {
            if( (int) dst_norm.at<float>(j,i) > thresh )
              {
               circle( dst_norm_scaled, Point( i, j ),5,  Scalar(0), 1, 8, 0 );
              }
          }
     }
  /// Showing the result
  namedWindow( corners_window, CV_WINDOW_AUTOSIZE);
  imshow( corners_window, dst_norm_scaled );
}
