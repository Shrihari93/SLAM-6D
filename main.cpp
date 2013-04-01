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

    /*
    IplImage* aft1  =cvLoadImage("aft1.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* fore1 =cvLoadImage("fore1.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* aft2 =cvLoadImage("aft2.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* fore2 =cvLoadImage("fore2.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	*/
	IplImage* aft3 =cvLoadImage("aft3.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* fore3 =cvLoadImage("fore3.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	

	IplImage* aft4 =cvLoadImage("aft4.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* fore4 =cvLoadImage("fore4.jpg",CV_LOAD_IMAGE_GRAYSCALE);

	/*
	IplImage* harris_aft1 =cvCreateImage(cvSize(aft1->width,aft1->height),32,1);
	IplImage* harris_fore1 =cvCreateImage(cvSize(fore1->width,fore1->height),32,1);
	IplImage* harris_aft2 =cvCreateImage(cvSize(aft2->width,aft2->height),32,1);
	IplImage* harris_fore2 =cvCreateImage(cvSize(fore2->width,fore2->height),32,1);
	*/
	IplImage* harris_aft3 =cvCreateImage(cvSize(aft3->width,aft3->height),32,1);
	IplImage* harris_fore3 =cvCreateImage(cvSize(fore3->width,fore3->height),32,1);
	
	IplImage* harris_aft4 =cvCreateImage(cvSize(aft4->width,aft4->height),32,1);
	IplImage* harris_fore4 =cvCreateImage(cvSize(fore4->width,fore4->height),32,1);
	
	//CvArr* harris_aft4=;
	//CvArr* harris_fore4=;
	cvCornerHarris(aft3,harris_aft3,3, 3, 0.04);
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