#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <math.h>
#define IMGDATA(image,i,j,k) ((char*)&image->imageData[(i)*(image->widthStep) + (j)*(image->nChannels) + (k)])
void initImage(IplImage* in,int k)
{
    int w=in->width;
    int h=in->height;
    int i,j,l;
    printf("\nKILL");
    for(i=0;i<w;i++)
    {
        for(j=0;j<h;j++)
        {
        	for(l=0;l<in->nChannels;l++)
        	{
            	*IMGDATA(in,i,j,l)=k;
            }
        }
    }
}
IplImage * createImage(int w, int h)
{
   IplImage * img = NULL;
   img = cvCreateImage(cvSize(w,h),IPL_DEPTH_8U,1);
   initImage(img,0);
   return img;
}
int sumofsquaredifference(IplImage* left,IplImage* right,int il,int jl,int ir,int jr,int blocksize)
{
	int i,j;
	int sosd=0;
	int sd;
	for(i=0;i<blocksize;i++)
	{
		for(j=0;j<blocksize;j++)
		{
			sd=abs(*IMGDATA(left,il+i,jl+j,0))-abs(*IMGDATA(right,ir+i,jr+j,0));
			sosd=sosd+sd*sd;
		}
	}
	return sosd;
}
void assignDisparity(IplImage* in,int il,int jl,int blocksize,int value,int channel)
{
	int i,j;
	for(i=0;i<blocksize;i++)
	{
		for(j=0;j<blocksize;j++)
		{
			//*IMGDATA(in,il+i,jl+j,channel)=value*255/in->width;
			*IMGDATA(in,il+i,jl+j,channel)=value;
		}
	}
}
void StereoExhaustiveBM(IplImage* left,IplImage* right,IplImage* dst,int blocksize,int threshold)
{
	IplImage* xdst;
	IplImage* ydst; 
	int il,jl;
	int ir,jr;
	int xshift,yshift;
	unsigned int msosd;
	unsigned int fsosd;
	//cvNamedWindow("Disp",CV_WINDOW_NORMAL);
	xdst=createImage(left->width,left->height);
	ydst=createImage(left->width,left->height);
	cvNamedWindow("XDisp",CV_WINDOW_NORMAL);
	cvNamedWindow("YDisp",CV_WINDOW_NORMAL);
	for(il=0;il<=left->width-blocksize;il+=blocksize)
	{	
		for(jl=0;jl<=left->height-blocksize;jl+=blocksize)
		{
			msosd=10000000;
			printf("\n %d %d",il,jl);
			for(ir=0;ir<right->width-blocksize;ir++)
			{
				printf("\n Block In Right Image %d %d",ir,jr);
				for(jr=0;jr<right->height-blocksize;jr++)
				{
					fsosd=sumofsquaredifference(left,right,il,jl,ir,jr,blocksize);
					if(fsosd<msosd)
					{
						printf("\n MSOSD %d",msosd);
						printf("\n Mininimum msosd %d",msosd);
						msosd=fsosd;
						xshift=abs(ir-il);
						yshift=abs(jr-jl);
						printf("\n %d %d",xshift,yshift);
					}
				}
			}
			if(msosd<100)
			{
				assignDisparity(xdst,il,jl,blocksize,xshift,0);
				assignDisparity(ydst,il,jl,blocksize,yshift,0);
			}
		}
		//assignDisparity(dst,il,jl,blocksize,xshift,0);//B
		//assignDisparity(dst,il,jl,blocksize,yshift,2);//R
		cvWaitKey(500);
		cvShowImage("XDisp",xdst);
		cvShowImage("YDisp",ydst);
	}
	cvWaitKey(0);

}
int main()
{
	int blocksize=4;
	int threshold=0;
	IplImage* aft=cvLoadImage("aft4.jpg",CV_LOAD_IMAGE_GRAYSCALE);
    IplImage* fore=cvLoadImage("fore4.jpg",CV_LOAD_IMAGE_GRAYSCALE);
    IplImage* dst;

    cvSmooth(aft,aft,CV_GAUSSIAN,3,0,0,0);
    cvSmooth(fore,fore,CV_GAUSSIAN,3,0,0,0);
    StereoExhaustiveBM(fore,aft,dst,blocksize,threshold);
    cvNamedWindow("Disparity",CV_WINDOW_AUTOSIZE);
    cvShowImage("Disparity",dst);
    cvWaitKey(0);
    cvDestroyWindow("Disparity");
	return 0;
}