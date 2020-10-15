#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
//#include "yolo3.h"

#define MIN_VALUE (-1024*1024*1024)
#define MAX_VALUE (1024*1024*1024)

//#define MAX_TEST
#define MAX_TEST_VALUE 8

/*
这一步是为了做什么？好像是找到误差最小的可利用的位数吧
*/
int quantize_short16(int type,float *in,short *out,int *offset,int layer_num,float *ap16_range,int *maxQ_array)
{
	int i;
	int offset_index = 0;
	int woffset = 0;
	for(i=0;i<layer_num;i++)
	{
		if(offset[offset_index]==0)
			return i;
		printf("Layer %2d;weight num=%12d\n",i,offset[offset_index]);
		int j;
		float min,max;
		min = MAX_VALUE;
		max = MIN_VALUE;
		for(j=0;j<offset[offset_index];j++)
		{
			float tmp_in_float = in[woffset+j];
			if(tmp_in_float<min)
				min = tmp_in_float;
			if(tmp_in_float>max)
				max = tmp_in_float;
		}
		printf("float min=%.7lf,max=%.7lf\n",min,max);//find float min max

		int k;
		int maxQ = -1;
		for(k=0;k<16;k++)//find maxQ
		{
			if(min>ap16_range[2*k]&&max<ap16_range[2*k+1])
			{
				maxQ = k;
			}
			else if(k==0)
			{
				printf("beyond Q0 min=%.7lf,max=%.7lf ",min,max);
				break;
			}
		}
		#ifdef MAX_TEST
		maxQ = MAX_TEST_VALUE;
		#endif
		
		if(offset_index == 29){
			maxQ=15;
		} 
		/*else if(offset_index == 30){
			maxQ = 12;
		}*/
		/*
		if(maxQ>13){
			maxQ=13;
		}*/
		/*for(k = 0;k<200;k++){
			if(type==1){
				printf("%f,")
			}
		}*/
		printf("offset_index=%d,maxQ=%d\n",offset_index,maxQ);
		maxQ_array[i] = maxQ;
		

		double max_error,min_error,sum_error;
		sum_error = 0;
		max_error = MIN_VALUE;
		min_error = MAX_VALUE;
		for(j=0;j<offset[offset_index];j++)
		{
			float tmp_in_float = in[woffset+j];
			short tmp_fixed = (short)(tmp_in_float*pow(2.0,maxQ));
			/*
			if( j<10){
				printf("tmp_in_float = %f,para = %f,tmp_fixed = %hd\n",tmp_in_float,pow(2.0,maxQ),tmp_fixed);
			}
			*/
			if(type==1 && j<200){
				printf("%f,%hd\t",tmp_in_float,tmp_fixed);
				if((j+1)%10==0){printf("\n");}
			}
			float tmp_out_float = (float)tmp_fixed*pow(2.0,-maxQ);
			double error = (tmp_out_float - tmp_in_float)*(tmp_out_float - tmp_in_float);
			error = sqrt(error);
			sum_error += error;
			if(error<min_error)
				min_error = error;
			if(error>max_error)
				max_error = error;
/*
			if(type==0){
				tmp_fixed = 0;
			}
*/
			out[woffset+j] = tmp_fixed;
			
		}
		printf("sum2_error = %.7lf,min_error=%.7lf,max_error=%.7lf",sum_error,min_error,max_error);
		printf("\n\n");

		woffset += offset[offset_index];
		offset_index++;
	}

	return 0;
}


int main(int argc,char *argv[])
{
	int i;
	int fixed  = MAX_TEST_VALUE;
	printf("Test fixed-point\n");
	
	//这部分是偏移量，根据文件进行了测定后修改
/*------------------------------------------------------------------------------*/
	int yolo3_weight_offset[128] = {864,18432,2048,18432,
		73728,8192,73728,
		8192,73728,
		294912,32768,294912,
		32768,294912,
		32768,294912,
		32768,294912,
		32768,294912,
		32768,294912,
		32768,294912,
		32768,294912,
		1179648,131072,1179648,
		131072,1179648,
		131072,1179648,
		131072,1179648,
		131072,1179648,
		131072,1179648,
		131072,1179648,
		131072,1179648,
		4718592,524288,4718592,
		524288,4718592,
		524288,4718592,
		524288,4718592,
		524288,4718592,524288,4718592,524288,4718592,261120,
		131072,
		196608,1179648,131072,1179648,131072,1179648,130560,
		32768,
		49152,294912,32768,294912,32768,294912,65280,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0};
	
	int yolo3_beta_offset[128] = {32,64,32,64,
		128,64,128,
		64,128,
		256,128,256,
		128,256,
		128,256,
		128,256,
		128,256,
		128,256,
		128,256,
		128,256,
		512,256,512,
		256,512,
		256,512,
		256,512,
		256,512,
		256,512,
		256,512,
		256,512,
		1024,512,1024,
		512,1024,
		512,1024,
		512,1024,
		512,1024,512,1024,512,1024,255,
		256,
		256,512,256,512,256,512,255,
		128,
		128,256,128,256,128,256,255,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
		0,0,0};
	
	short* yolo3_weight_fixed_buf = (short *)calloc(247583104/4,sizeof(short));
	float* yolo3_weight_buf = (float *)calloc(247583104/4,sizeof(float));
	float* yolo3_beta_buf   = (float *)calloc(108276/4,sizeof(float));
	
	FILE *fp_yl3_w = fopen("weightsv3_comb_reorg.bin", "rb");
    if(!fp_yl3_w) printf("fopen weightsv3_comb_reorg.bin error\n");
	FILE *fp_yl3_b = fopen("bias.bin", "rb");
    if(!fp_yl3_b) printf("fopen bias.bin error\n");

	fread(yolo3_weight_buf, sizeof(float), 247583104/4, fp_yl3_w);
	fread(yolo3_beta_buf, sizeof(float), 108276/4, fp_yl3_b);
	printf("yolo3_weight_buf[0] = %f\n",yolo3_weight_buf[0]);

	fclose(fp_yl3_w);
	fclose(fp_yl3_b);
/*------------------------------------------------------------------------------*/
	
	//这部分似乎是和数据类型有关系，32bit，所以不做修改
	short ap16_min = 0x8000;
	short ap16_max = 0x7fff;
	printf("ap16_min = %d \nap16_max = %d\n",ap16_min,ap16_max);
	float ap16_range[16*2];
	for(i=0;i<16;i++)
	{
		printf("Q%2d:",i);
		ap16_range[2*i]   = (float)ap16_min*pow((float)2,-i);//min
		ap16_range[2*i+1] = (float)ap16_max*pow((float)2,-i);//max
		printf("min=%.7lf,max=%.7lf\n",ap16_range[2*i],ap16_range[2*i+1]);
	}
	
	
	//
	int maxQ_array[128];
	int layer_num;
	FILE* fout;
	char layer_num_string[256];
	char s[256];

	printf("weight quantize begin\n");
	layer_num = quantize_short16(1,yolo3_weight_buf,yolo3_weight_fixed_buf,yolo3_weight_offset,128,ap16_range,maxQ_array);
	
	for(i=0;i<layer_num;i++)
	{
		#ifdef MAX_TEST
		maxQ_array[i] = fixed;
		#endif
		printf("maxQ_array[%d]=%d\n",i,maxQ_array[i]);
	}
	sprintf(s,"weightsv3_comb_reorg_ap16_maxQ_%d.bin", layer_num);
	printf("%s\n",s);

	fout = fopen(s,"wb");
    if(!fout) printf("fopen %s error\n",s);
	fwrite(maxQ_array,sizeof(int), layer_num,fout);
	fclose(fout);

	fout = fopen("weightsv3_comb_reorg_ap16.bin","wb");
    if(!fout) printf("fopen weightsv3_comb_reorg_ap16.bin error\n");
	fwrite(yolo3_weight_fixed_buf,sizeof(short), 247583104/4,fout);
	fclose(fout);
	printf("weight quantize end\n");

	printf("beta quantize begin\n");
	layer_num = quantize_short16(0,yolo3_beta_buf,yolo3_weight_fixed_buf,yolo3_beta_offset,128,ap16_range,maxQ_array);
	for(i=0;i<layer_num;i++)
	{
		#ifdef MAX_TEST
		maxQ_array[i] = fixed;
		#endif
		printf("MaxQ_array[%d]=%d\n",i,maxQ_array[i]);
	}
	sprintf(s,"biasv3_comb_ap16_maxQ_%d.bin", layer_num);
	printf("%s\n",s);

	fout = fopen(s,"wb");
    if(!fout) printf("fopen %s error\n",s);
	fwrite(maxQ_array,sizeof(int), layer_num,fout);
	fclose(fout);

	fout = fopen("biasv3_comb_ap16.bin","wb");
    if(!fout) printf("fopen biasv3_comb_ap16.bin error\n");
	fwrite(yolo3_weight_fixed_buf,sizeof(short), 108276/4+1,fout);
	fclose(fout);
	printf("beta quantize end\n");

	free(yolo3_weight_fixed_buf);
	free(yolo3_weight_buf);
	free(yolo3_beta_buf);
	
	printf("0.1=%.10lf\n",((short)(0.1*pow(2.0,15)))*pow(2.0,-15));
	printf("0.1=%x\n",(short)(0.1*pow(2.0,15)));

	return 0;
}