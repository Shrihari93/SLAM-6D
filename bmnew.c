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
void cvDrawRectangle(const IplImage* in,IplImage* out,int window,int xpos,int ypos,int color)
{
    cvCopyImage(in,out);
    CvPoint p1;
    CvPoint p2;
    p1.x=xpos;
    p1.y=ypos;
    p2.x=xpos+window;
    p2.y=ypos+window;
    if(color==1)
    {
        cvRectangle(out,p1,p2,CV_RGB(255,0,0),2,8,0);
    }
    if(color==2)
    {
        cvRectangle(out,p1,p2,CV_RGB(0,0,255),2,8,0);
    }
}
float sumofsquaredifference(const IplImage* left,const IplImage* right,int il,int jl,int ir,int jr,const int blocksize)
{
	int i,j;
	float sosd=0;
	float sd;
	float sum=0;
	for(i=0;i<blocksize;i++)
	{
		for(j=0;j<blocksize;j++)
		{
			sd=abs(*IMGDATA(left,il+i,jl+j,0))-abs(*IMGDATA(right,ir+i,jr+j,0));
			sosd=sosd+sd*sd;
			sum=sum+abs(*IMGDATA(left,il+i,jl+j,0));
		}
	}
	return sosd/sum*sum;
}
float covariance(const IplImage* left,const IplImage* right,int il,int jl,int ir,int jr,const int blocksize)
{
	float cov;
	float exy=0,ex=0,ey=0;
	int i,j;
	for(i=il;i<il+blocksize;i++)
	{
		for(j=jl;j<jl+blocksize;j++)
		{
			ex=ex+abs(*IMGDATA(left,i,j,0));
		}
	}
	ex=ex/(blocksize*blocksize);
	for(i=ir;i<ir+blocksize;i++)
	{
		for(j=jr;j<jr+blocksize;j++)
		{
			ey=ey+abs(*IMGDATA(right,i,j,0));
		}
	}
	ey=ey/(blocksize*blocksize);
	for(i=0;i<blocksize;i++)
	{
		for(j=0;j<blocksize;j++)
		{
			exy=exy+abs(*IMGDATA(left,il+i,jl+j,0))*abs(*IMGDATA(right,ir+i,jr+j,0));
		}
	}
	exy=exy/(blocksize*blocksize);
	cov=exy-ex*ey;
	//printf("\n COVARIANCE ~ %f",cov);
	return cov;
}
float variance(const IplImage* in,int il,int jl,const int blocksize)
{
	float exx=0;
	float ex=0;
	float var=0;
	int i,j;
	for(i=il;i<il+blocksize;i++)
	{
		for(j=jl;j<jl+blocksize;j++)
		{
			exx=exx+abs(*IMGDATA(in,il,jl,0))*abs(*IMGDATA(in,il,jl,0));
			ex=ex+abs(*IMGDATA(in,il,jl,0));
		}
	}
	var=(exx-ex*ex)/(blocksize*blocksize);
	var=abs(var);
	//printf("\n VARIANCE ~ %f",var);
	return var;
}
float correlation(const IplImage *left,const IplImage* right,int il,int jl,int ir,int jr ,const int blocksize)
{
	float corr;
	corr=covariance(left,right,il,jl,ir,jr,blocksize)/(sqrt(variance(left,il,jl,blocksize))*sqrt(variance(right,ir,jr,blocksize)));
	//printf("\nCORRELATON~ %f",corr);
	return corr;
}
int sumofsquaredaddition(IplImage* left,IplImage* right,int il,int jl,int ir,int jr,int blocksize)
{
	int i,j;
	int sosa=0;
	int sd;
	for(i=0;i<blocksize;i++)
	{
		for(j=0;j<blocksize;j++)
		{
			sd=abs(*IMGDATA(left,il+i,jl+j,0))+abs(*IMGDATA(right,ir+i,jr+j,0));
			sosa=sosa+abs(sd);
		}
	}
	return sosa;	
}
void assignDisparity(IplImage* in,int il,int jl,int blocksize,int value,int channel)
{
	int i,j;
	
	value=value*255/in->width;
	//printf("\nPassed ij~%d jl~%d Value ~ %d",il,jl,value);
	for(i=il;i<il+blocksize;i++)
	{
		for(j=jl;j<jl+blocksize;j++)
		{
			//*IMGDATA(in,il+i,jl+j,channel)=value*255/in->width;
			*IMGDATA(in,i,j,channel)=value;

		}
	}
}
void StereoExhaustiveBM(const IplImage* left,const IplImage* right,IplImage* dst,const int blocksize,const int threshold,const int xlimit,const int ylimit)
{
	IplImage* xdst;
	IplImage* ydst; 
	int il,jl;
	int ir,jr;
	int irtempf,jrtempf;
	int irtempt,jrtempt;
	int ircorr,jrcorr;
	int xshift,yshift;
	float msosd;
	float fsosd;
	IplImage *leftm;
    IplImage *rightm;

    leftm=createImage(left->width,left->height,3);
    rightm=createImage(right->width,right->height,3);

	//cvNamedWindow("Disp",CV_WINDOW_NORMAL);
	xdst=createImage(left->width,left->height,1);
	ydst=createImage(right->width,right->height,1);
	printf("******done\n\n\n");
	cvNamedWindow("XDisp",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("YDisp",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Position on Left",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Position on Right",CV_WINDOW_AUTOSIZE);
	printf("done");
	for(il=0;il<=left->width-blocksize;il+=blocksize)
	{	
		for(jl=0;jl<=left->height-blocksize;jl+=blocksize)
		{
			//msosd=100000000;
			msosd=-5;
			//printf("\n %d %d",il,jl);
			//for(ir=0;ir<right->width-blocksize;ir++)
			
			irtempf=il-xlimit;
			jrtempf=jl-ylimit;
			irtempt=il+xlimit;
			jrtempt=jl+ylimit;
			if(irtempf<0)
			{
				irtempf=0;
			}
			if(jrtempf<0)
			{
				jrtempf=0;
			}
			if(irtempt>left->width-blocksize)
			{
				irtempt=left->width-blocksize;
			}
			if(jrtempt>left->height-blocksize)
			{
				jrtempt=left->height-blocksize;
			}
			
			for(ir=irtempf;ir<=irtempt;ir++)
			//for(ir=0;ir<=right->width-blocksize;ir++)
			{
				//printf("\n Block In Right Image %d %d",ir,jr);
				for(jr=jrtempf;jr<=jrtempt;jr++)
				//for(jr=0;jr<right->height-blocksize;jr++)
				{
					fsosd=correlation(left,right,il,jl,ir,jr,blocksize);
					//fsosd=sumofsquaredifference(left,right,il,jl,ir,jr,blocksize);
					//printf("\n CORRELATON ~ %f",fsosd );
					if(fsosd>msosd)
					{
						ircorr=ir;
						jrcorr=jr;
						//printf("\n FSOSD %f",fsosd);
						//printf("\n Mininimum msosd %d",msosd);
						msosd=fsosd;
						xshift=abs(ir-il);
						yshift=abs(jr-jl);
						//printf("\nSHIFT~ %d %d  ICORR~ %d JCORR~ %d",xshift,yshift,ircorr,jrcorr);
						//cvWaitKey(100);
						//assignDisparity(xdst,il,jl,blocksize,xshift,0);
						//assignDisparity(ydst,il,jl,blocksize,yshift,0);
					}
				}
			}
			cvWaitKey(5);
			cvDrawRectangle(left,leftm,blocksize,il,jl,1);
            cvDrawRectangle(right,rightm,blocksize,ircorr,jrcorr,2);
            cvShowImage("Position on Left",leftm);
			cvShowImage("Position on Right",rightm);
			//if(msosd<threshold)
			//{
				
			assignDisparity(xdst,il,jl,blocksize,xshift,0);
			assignDisparity(ydst,il,jl,blocksize,yshift,0);
			//printf("\n Now at ~ %d %d",il,jl);
			//cvWaitKey(100);
			cvShowImage("XDisp",xdst);
			cvShowImage("YDisp",ydst);
			//}
		}
		//assignDisparity(dst,il,jl,blocksize,xshift,0);//B
		//assignDisparity(dst,il,jl,blocksize,yshift,2);//R
		//cvWaitKey(500);
		
	}
	cvWaitKey(0);
    cvNormalize(xdst,xdst,0,255, CV_MINMAX );
    cvNormalize(ydst,ydst,0,255, CV_MINMAX );	
	cvShowImage("XDisp",xdst);
	cvShowImage("YDisp",ydst);
	cvWaitKey(0);

}
int main()
{
	int blocksize=100;
	int threshold=10000;
	int xlimit=100;
	int ylimit=100;
	//IplImage* aft=cvLoadImage("aft3.jpg",CV_LOAD_IMAGE_COLOR);
    //IplImage* fore=cvLoadImage("fore3.jpg",CV_LOAD_IMAGE_COLOR);
    IplImage* aft=cvLoadImage("scene_l.pgm",CV_LOAD_IMAGE_COLOR);
    IplImage* fore=cvLoadImage("scene_r.pgm",CV_LOAD_IMAGE_COLOR);
    IplImage* dst;
    cvNormalize(aft,aft,0,255, CV_MINMAX );
    cvNormalize(fore,fore,0,255, CV_MINMAX );
    //cvSmooth(aft,aft,CV_GAUSSIAN,3,0,0,0);
    //cvSmooth(fore,fore,CV_GAUSSIAN,3,0,0,0);
    StereoExhaustiveBM(aft,fore,dst,blocksize,threshold,xlimit,ylimit);
    //cvNamedWindow("Disparity",CV_WINDOW_AUTOSIZE);
    //cvShowImage("Disparity",dst);
    cvWaitKey(0);
    //cvDestroyWindow("Disparity");
	return 0;
}