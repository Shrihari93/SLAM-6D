#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <math.h>
int main()
{
	IplImage* ydst=cvLoadImage("YDisparity.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	int k;
	for(k=0;k<12;k++)
    {
        printf("\nGAUSSSIAN %d",k);
        cvSmooth(ydst,ydst,CV_GAUSSIAN,2*k+1,0,0,0);
        cvNormalize(ydst,ydst,0,255,CV_MINMAX);
        cvShowImage("DISPARITY",ydst);
        cvWaitKey(10);
    }
    cvSaveImage("Smooth_Disp.jpg",ydst);
    printf("\nImages Are Saved");
    return 0;
}