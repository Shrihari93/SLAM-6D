#include <iostream>
#include <cstring>
#include<cstdio>
#include <cstdlib>

#include<cv.h>
#include<highgui.h>

void weightedInterpolate(IplImage *input,IplImage *output,int height,int width){

	cvScalar a,b;
	double x;
	int i,j;
	for(i=0;i<=height-1;i++){
		for(j=0;j<=width-1;j++){
			a=cvGet2D(input,i,j);
			b=cvGet2D(output,i,j);
			if(a.val[0]!=0){
				b.val[0]=a.val[0];
			}else{
				x=interpolate(input,height,width,i,j,5);
				b.val[0]=(int)x;
			}
			cvSet2D(output,i,j,b);
		}
	}

}
double distance2(int a,int b,int x,int y){
	return (a-x)*(a-x)+(b-y)*(b-y);
}
double interpolate(IplImage *input,int height,int width,int i,int j,int ratio){
	int bheight,bwidth;
	bheight=height/ratio;
	bwidth=width/ratio;
	int x,y,m,n;
	double sum=0,sumd=0,dist2;
	cvScalar a;
	for(x=-((bheight-1)/2);x<=((bheight-1)/2);x++){
		for(y=-(bwidth-1)/2;y<=(bwidth-1)/2;y++){
			m=i+x;
			n=j+y;
			if(!((m>=height)||(m<0)||(n>=width)||(n<0)||((m=i)&&(n=j))){
				//point is in bound and not (i,j)
				dist2=distance2(m,n,i,j);
				if(dist2!=0){
					a=cvGet2D(input,m,n);
					sum=sum+(a.val[0])/dist2;
					sumd=sumd+(1.0/dist2);
				}
			}
		}
	}
	return sum/sumd;
}

int main(){

}