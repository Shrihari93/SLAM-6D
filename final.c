#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <math.h>
#define IMGDATA(image,i,j,k) ((char*)&image->imageData[(j)*(image->widthStep) + (i)*(image->nChannels) + (k)])
void initImage(IplImage* in,int k)
{
    int w=in->width;
    int h=in->height;
    int i,j,l;
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
IplImage * createImage(int w, int h,int channels)
{
   IplImage * img = NULL;
   img = cvCreateImage(cvSize(w,h),IPL_DEPTH_8U,channels);
   initImage(img,0);
   return img;
}
float newcorr(const IplImage *left,const IplImage* right,const int il,const int jl,const int ir,const int jr ,const int blocksize)
{
	float corr=0;
	int i,j;
	float mx,my;
	float vx,vy;
	mx=mean(left,il,jl,blocksize);
	my=mean(right,ir,jr,blocksize);
	vx=var(left,il,jl,blocksize);
	vy=var(right,ir,jr,blocksize);
	for(i=0;i<blocksize;i++)
	{
		for(j=0;j<blocksize;j++)
		{
			corr=corr+(abs(*IMGDATA(left,il+i,jl+j,0))-mx)*(abs(*IMGDATA(right,ir+i,jr+j,0))-my);
		}
	}
	//printf("\n CORRELATON ~ %f, VARIANCE ~ %f ,VARIANCE ~ %f",corr,vx,vy );
	corr=corr/(sqrt(vx)*sqrt(vy));
	corr=corr/(blocksize*blocksize);
	//printf("\n CORRELATON ~ %f",corr );
	return corr;
}
void
int main()
{
	IplImage* aft2=cvLoadImage("aft3.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* fore2=cvLoadImage("fore2.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* xdst3=cvLoadImage("XDisparity.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* ydst3=cvLoadImage("YDisparity.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* xdst2=createImage(aft2->width,aft2->height,1);
	IplImage* ydst2=createImage(fore2->width,fore2->height,1);

	
	IplImage* aft3 =cvCreateImage(cvSize(aft2->width/5,aft2->height/5),IPL_DEPTH_8U,1);
	IplImage* fore3 =cvCreateImage(cvSize(fore2->width/5,fore2->height/5),IPL_DEPTH_8U,1);
	IplImage* aft4 =cvCreateImage(cvSize(aft3->width/5,aft3->height/5),IPL_DEPTH_8U,1);
	IplImage* fore4 =cvCreateImage(cvSize(fore3->width/5,fore3->height/5),IPL_DEPTH_8U,1);
	return 0;
}