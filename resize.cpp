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

	if(!aft1 || !fore1)
	{
		printf("ERROR: images couldn't be loaded. Abort\n");
		return 0;
	}

	cvNormalize(aft1, aft1, 0, 255, CV_MINMAX);
	cvNormalize(fore1, fore1, 0, 255, CV_MINMAX);
	IplImage* aft2 =cvCreateImage(cvSize(aft1->width/5,aft1->height/5),IPL_DEPTH_8U,1);
	IplImage* fore2 =cvCreateImage(cvSize(fore1->width/5,fore1->height/5),IPL_DEPTH_8U,1);
	IplImage* aft3 =cvCreateImage(cvSize(aft2->width/5,aft2->height/5),IPL_DEPTH_8U,1);
	IplImage* fore3 =cvCreateImage(cvSize(fore2->width/5,fore2->height/5),IPL_DEPTH_8U,1);
	IplImage* aft4 =cvCreateImage(cvSize(aft3->width/5,aft3->height/5),IPL_DEPTH_8U,1);
	IplImage* fore4 =cvCreateImage(cvSize(fore3->width/5,fore3->height/5),IPL_DEPTH_8U,1);

	//processing and saving aft images:
	cvSaveImage("resized-images/aft1.jpg",aft1);
	cvSmooth(aft1, aft1, CV_GAUSSIAN);
	cvResize(aft1,aft2,CV_INTER_LINEAR);

	cvSaveImage("resized-images/aft2.jpg",aft2);
	cvSmooth(aft2, aft2, CV_GAUSSIAN);
	cvResize(aft2,aft3,CV_INTER_LINEAR);

	cvSaveImage("resized-images/aft3.jpg",aft3);
	cvSmooth(aft3, aft3, CV_GAUSSIAN);
	cvResize(aft3,aft4,CV_INTER_LINEAR);

	cvSaveImage("resized-images/aft4.jpg",aft4);

	//processing and saving for images:
	cvSaveImage("resized-images/fore1.jpg",fore1);
	cvSmooth(fore1, fore1, CV_GAUSSIAN);
	cvResize(fore1,fore2,CV_INTER_LINEAR);

	cvSaveImage("resized-images/fore2.jpg",fore2);
	cvSmooth(fore2, fore2, CV_GAUSSIAN);
	cvResize(fore2,fore3,CV_INTER_LINEAR);


	cvSaveImage("resized-images/fore3.jpg",fore3);
	cvSmooth(fore3, fore3, CV_GAUSSIAN);
	cvResize(fore3,fore4,CV_INTER_LINEAR);

	cvSaveImage("resized-images/fore4.jpg",fore4);
	
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
