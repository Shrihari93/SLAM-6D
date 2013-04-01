#include <stdlib.h>
#include <stdio.h>
struct point{
	double x;
	double y;
};
typedef struct point point;


int main(){

	/***************CODE to find best fitting str line with method of least square technique***************/

	point plist[100];
	int i,pcount;
	/**************code for random generation*********************/
	/**/
	pcount=100
	for(i=0;i<=99;i++){
		plist[i].x=rand()%(i+1);
		plist[i].y=rand()%(i+1)-50;
		printf("\n \tX = %lf\tY = %lf ",plist[i].x,plist[i].y);
	}
	/*/
	/*
	pcount=3;
	plist[0].x=1;
	plist[0].y=1;
	plist[1].x=2;
	plist[1].y=3;
	plist[2].x=3;
	plist[2].y=4;
	*/
	/****************************************************/

	double sumx=0,sumy=0,sumx2=0,sumxy=0;
	double xmean=0,ymean=0;
	for(i=0;i<=pcount-1;i++){
		sumx=sumx+plist[i].x;
		sumy=sumy+plist[i].y;
		sumx2=sumx2+(plist[i].x)*(plist[i].x);
		sumxy=sumxy+plist[i].x*plist[i].y;
	}
	xmean=sumx/pcount;
	ymean=sumy/pcount;
	double yint=0,slope=0;
	if((sumx2-sumx*xmean)==0){
		printf("\nBAD INPUT !!");
		return 0;
	}
	slope=(sumxy-sumx*ymean)/(sumx2-sumx*xmean);
	yint=ymean-slope*xmean;
	printf("\n\n slope = %lf yint = %lf \n\n",slope,yint);
	return 0;
}