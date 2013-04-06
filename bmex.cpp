#include <iostream>
#include <cstring>
#include<cstdio>
#include <cstdlib>

#include<cv.h>
#include<highgui.h>


using namespace std;

struct meanvar{
	double mean;
	double var;
};
typedef struct meanvar meanvar;

struct hcorners{
    double x;
    double y;
};
typedef struct hcorners hcorners;

struct disp{
    double x_disp;
    double y_disp;
};

typedef struct disp disp;
struct point{
	double x;
	double y;
};
typedef struct point point;

int main(){

}

void computeBasicDisparity(IplImage *aft,IplImage *fore,IplImage *hc,meanvar *stat1,meanvar* stat2,int row,int col){
	int i=j=0;
	int k,l,m,n;
	CVScalar s,s1,s2;
	int cnt=0;
	d

	for(i=0;i<row;i++){

		for(j=0;j<col;j++){
			s=cvGet2D(hc,i,j);
			if(s.val[0]>0)
				cnt++;
		}

	}
    cnt=0;
	hcorners arr[cnt];
	point arr2[cnt];
	disp mat[row*col];
	for(i=0;i<row;i++){

		for(j=0;j<col;j++){
			s=cvGet2D(hc,i,j);
			if(s.val[0]>0){
				arr[cnt].x=i;
				arr[cnt].y=j;
				cnt++;



			}
				
		}

	}
    int x,y,start_row,start_col,no_row,no_col,max_row,max_col;
    double mean1,mean2,var1,var2,cov,corr,max_corr,sum_xy;
	for(i=0;i<cnt;i++){
         x=arr[i].x;
         y=arr[i].y;
         max_corr=-1;
         start_row=(x-2>=0)?x-2:0
         start_col=(y-2<=0)?y-2:0;
         no_row=(start_row+4>=row)?row-start_row:5;
         no_row=(start_col+4>=col)?col-start_col:5;
         mean1=stat1[nrows*start_row+start_col].mean;         
         var1=stat1[nrows*start_row+start_col].var;
         
         cov=0;
         sum_xy=0;
         for(k=0;k<row;k++){
         	for(l=0;l<col;l++){
         		mean2=stat2[nrows*k+l].mean;         
                var2=stat2[nrows*k+l].var;
                if(k+no_row>=row|| l+no_col>=col)
                	continue;
                sum_xy=0;
                mean2=stat2[nrows*k+l].mean;         
                var2=stat2[nrows*k+l].var;


                for(m=0;m<no_row;m++){
                	for(n=0;n<no_col;n++){

                		s1=cvGet2D(aft,start_row+m,start_row+n);
                		s2=cvGet2D(fore,k+m,l+n);
                		sum_xy+=s1.val[0]*s2.val[0];
                	}
                }
                sum_xy=sum_xy/(no_row*no_col);
                corr=(sum_xy-mean1*mean2)/(sqrt(var1*var2));
                if(corr>max_corr){
                	max_corr=corr;
                	mat[x*row+y].x_disp=x-k;
                	mat[]
                }




         	}

         }


	}
}

