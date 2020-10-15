#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#include <assert.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "cnn.h"
#define FLT_MAX 3.402823466e+38F
//#include "yolov3.h"


//
#define SIZE 13
#define PAGE 8

int main(void){
    int i,j,k;
    short upsamplein[SIZE*SIZE*PAGE];
    int*  memin = (int*)calloc(SIZE*SIZE*PAGE,sizeof(int));
    int*  memout = (int*)calloc(SIZE*SIZE*PAGE*4,sizeof(int));
    for (i = 0; i < SIZE*SIZE*PAGE;i++){
        upsamplein[i] = i%(2*SIZE);
        printf("%hd,",upsamplein[i]);
        if((i+1)%SIZE == 0) printf("\n");
        if((i+1)%(SIZE*SIZE) == 0) printf("\n");
    }
    //>>inputsize 13 x 13 x 8    >>>>>>  26 x 26 x 8
    int tmp_in;
	short current_in,next_in;
	bool NextPixelInFlag = true;
	int InputPixelOffset = 0;
	
	
	//13*13*8 = 1352
	//2^15      = 65536
	//65536/1352 = 31.9<32>>16>>2^4
	int mulsize = 5;
	for(int x=0;x<SIZE*SIZE*PAGE;x++)//1st Layer input Q14
	{
		if(NextPixelInFlag)
		{
			current_in = (short)(upsamplein[x]*pow(2.0,mulsize));
			NextPixelInFlag = false;
		}
		else
		{
			next_in = (short)(upsamplein[x]*pow(2.0,mulsize));
			tmp_in = (next_in<<16) + (current_in);
			memin[InputPixelOffset] = tmp_in;
			InputPixelOffset++;
			NextPixelInFlag = true;
		}
	}

				int output_w = 2*SIZE;
				int output_h = 2*SIZE;

				int TR = SIZE;
				int TC = SIZE;
				int TN = 4;
				int TM = TN;

				int rLoops = (int)ceil(((float)SIZE/TR));
				int cLoops = (int)ceil(((float)SIZE/TC));
				//mLoops = (int)ceil(((float)l.n)/TM);
			    int nLoops = (int)ceil(((float)PAGE/TN));
				int mLoops = nLoops;
                int T2Rate = 1;
				//TRow = (TR-1)*l.stride+l.size;
				int TRow = 13;
				int trow_loops = (int)ceil(((float)TRow/T2Rate));
	

	printf("input end\n");

	/*
	 *				YOLO2_FPGA(in_ptr[i],in_ptr[i],in_ptr[i],in_ptr[i],out_ptr[i],out_ptr[i],NULL,NULL,l.c,l.c,
					1,l.stride,l.w,l.h,output_w,output_h,l.pad,0,0,TM,TN,TR,TC,mLoops,1,rLoops,cLoops,2,
					temp_inputQ[i],temp_inputQ[i+1],INTERWIDTH,INTERWIDTH,trow_loops);
	 * */

	YOLO2_FPGA(memin,memin,memin,memin,memout,memout,NULL,NULL,PAGE,PAGE,
	                1,2,SIZE,SIZE,2*SIZE,2*SIZE,0,0,0,TM,TN,TR,TC,mLoops,nLoops,rLoops,cLoops,2,
					mulsize,mulsize,20,20,trow_loops);
	
	printf("end\n\n\n\n");
	
	double Para = pow(2.0,-mulsize);
	bool NextPixelFlag = true;
	int OutputPixelOffset = 0;
	short current_p,next_p,output_p;
	int *Output_ptr = (int *)memout;
	//int j =0;
	for(j=0;j<SIZE*SIZE*PAGE*4;j++)
	{
		if(NextPixelFlag)
		{
			int tmp_p = Output_ptr[OutputPixelOffset];
			OutputPixelOffset++;
			current_p = tmp_p;
			next_p = tmp_p >> 16;
			output_p = current_p;
			NextPixelFlag = false;
		}else
		{
			output_p = next_p;
			NextPixelFlag = true;
		}
		//printf("%f,",output_p*Para);

			printf("%2f\t",output_p*Para);
			if((j+1)%26==0){
				printf("\n");
			}
			if((j+1)%676 == 0){
				printf("\n\n");
			}
		//yolo_buf[j]=((j+1)%4);
	}


	return 0;
    
}
