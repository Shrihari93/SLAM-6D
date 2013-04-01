#include <stdio.h>
#include <cv.h>
#include <highgui.h>
IplImage * createImage(int w, int h)
{
   IplImage * img = NULL;
   img = cvCreateImage(cvSize(w,h),IPL_DEPTH_8U,3);
   return img;
}
int main()
{
    IplImage* aft1  =cvLoadImage("aft.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* fore1 =cvLoadImage("fore.jpg",CV_LOAD_IMAGE_GRAYSCALE);

	IplImage* aft2 =cvCreateImage(cvSize(aft1->width/5,aft1->height/5),IPL_DEPTH_8U,1);
	IplImage* fore2 =cvCreateImage(cvSize(fore1->width/5,fore1->height/5),IPL_DEPTH_8U,1);
	IplImage* aft3 =cvCreateImage(cvSize(aft2->width/5,aft2->height/5),IPL_DEPTH_8U,1);
	IplImage* fore3 =cvCreateImage(cvSize(fore2->width/5,fore2->height/5),IPL_DEPTH_8U,1);
	IplImage* aft4 =cvCreateImage(cvSize(aft3->width/5,aft3->height/5),IPL_DEPTH_8U,1);
	IplImage* fore4 =cvCreateImage(cvSize(fore3->width/5,fore3->height/5),IPL_DEPTH_8U,1);

	cvResize(aft1,aft2,CV_INTER_LINEAR);
	cvResize(aft2,aft3,CV_INTER_LINEAR);
	cvResize(aft3,aft4,CV_INTER_LINEAR);

	cvResize(fore1,fore2,CV_INTER_LINEAR);
	cvResize(fore2,fore3,CV_INTER_LINEAR);
	cvResize(fore3,fore4,CV_INTER_LINEAR);

	cvSaveImage("aft1.jpg",aft1);
	cvSaveImage("aft2.jpg",aft2);
	cvSaveImage("aft3.jpg",aft3);
	cvSaveImage("aft4.jpg",aft4);
	cvSaveImage("fore1.jpg",fore1);
	cvSaveImage("fore2.jpg",fore2);
	cvSaveImage("fore3.jpg",fore3);
	cvSaveImage("fore4.jpg",fore4);
	
	/*
	cvNamedWindow("Level 1::Aft",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Level 1::Fore",CV_WINDOW_AUTOSIZE);
	cvShowImage("Level 1::Aft",aft1);
	cvShowImage("Level 1::Fore",fore1);
	cvWaitKey(0);
	cvDestroyAllWindows();
	*/
	return 0;
}
