
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ap_int.h>
#include "cnn.h"


/*
 *mei gai sha a za conv dou bu dui jin le?
 *mei
 *mei   xian yong zhi qian de ce shi conv
 *mei   ran hou zai ce shi qi ta de
 *
 *yan zheng fa xian shi conv bei gai dong le
 *
 *   chang shi ba PARA she zhi cheng 1
 *   >> gai cheng 1 hai shi bu xing
 *
 *	suo yi ying gai shi bie de di fang chu le wen ti
 *	dao di shi na li chu le wen ti???
 *
 *	xian kan yi xia quan zhong wen jian
 
 yuan ben de ce shi shi mei you wen ti de
 >>gai bian ip he
 >>gaibian IP he ye mei you wen ti 
    >> ye jiu shi shuo conv de wen ti bu zai IP he li 
 >>xia mian ying gai gai bian zhu kong han shu zuo ce shi le 
 >> you qu bie de jiu shi xin de nei cun kong zhi de di zhi mei wen ti de shi yong de
  	  jiu ban ben de di zhi zhe ge hui shi cha ju ma ?
 
>> quan xin jiu you wen ti
>> quan jiu jiu mei wen ti

>>zhi xie yi ge zhen dui upsample de testbench


?????

 * */
////////////////////////////////////////////20181229 n4m32  v2 without input and reorg opt ok input opt ok combine input relu comb ok // input opt ok // output opt ok //weight opt ok (5)n4m32i4o2 ok start
#define MAX(x,y) ((x)>(y)?(x):(y))
#define MIN(x,y) ((x)<(y)?(x):(y))
#define S 2
#define K 3

//Tn he Tm hai bu zhi dao shen me yi si
#define Tn 4
#define Tm 32

//Tr,Tc yao gen ju wang luo da xiao she zhi  
#define Tr 26
#define Tc 26

#define SIZE 13
#define PARA 1

#define OnChipIB_Width  ((Tc-1)*S+K)
#define OnChipIB_Height ((Tr-1)*S+K)
//MAX_BETA_LENGTH yao gen ju ce ding de shu ju she zhi
#define MAX_BETA_LENGTH (1024)
#define INTERWIDTH 20
/*
方案1：shortcut分别加载两次，每次四个
	尝试ing
方案2：重写加载函数只包含两个端口
	等方案一的结果
*/

typedef unsigned char UCHAR;

void mmcpy_inputport(int *input,int input_memcpy_buffer[(OnChipIB_Width+3)/2],ap_uint<3> TN_MIN,int RowOffset,UCHAR RowIntNum)
{
	bool enable = TN_MIN > 0;
	if(!enable)
		return;

	memcpy(input_memcpy_buffer,(int *)(input + RowOffset),RowIntNum*sizeof(int));

}

void mmcpy_inputport1(int *input,int input_memcpy_buffer[(OnChipIB_Width+3)/2],ap_uint<3> TN_MIN,int RowOffset,UCHAR RowIntNum)
{
	bool enable = TN_MIN > 1;
	if(!enable)
		return;

	memcpy(input_memcpy_buffer,(int *)(input + RowOffset),RowIntNum*sizeof(int));

}

void mmcpy_inputport2(int *input,int input_memcpy_buffer[(OnChipIB_Width+3)/2],ap_uint<3> TN_MIN,int RowOffset,UCHAR RowIntNum)
{
	bool enable = TN_MIN > 2;
	if(!enable)
		return;

	memcpy(input_memcpy_buffer,(int *)(input + RowOffset),RowIntNum*sizeof(int));


}

void mmcpy_inputport3(int *input,int input_memcpy_buffer[(OnChipIB_Width+3)/2],ap_uint<3> TN_MIN,int RowOffset,UCHAR RowIntNum)
{
	bool enable = TN_MIN > 3;
	if(!enable)
		return;

	memcpy(input_memcpy_buffer,(int *)(input + RowOffset),RowIntNum*sizeof(int));

}

/*
是从四个端口加入的
主要是看大小
*/
void mmcpy_inputpixel_m2b_comb(int *input,int *input1,int *input2,int *input3,
						  int input_memcpy_buffer[(OnChipIB_Width+3)/2],int input_memcpy_buffer1[(OnChipIB_Width+3)/2],
						  int input_memcpy_buffer2[(OnChipIB_Width+3)/2],int input_memcpy_buffer3[(OnChipIB_Width+3)/2],
						  ap_uint<1>  RowBeginByte[Tn],ap_uint<3> TN_MIN_3b,ap_uint<6> t2,ap_uint<1> RowSub,int IN_OFFSET,ap_uint<9> RowIncreaseLength,ap_uint<18> IHxIW_18b,ap_uint<6> ColIncreaseLength,ap_uint<6> next_t2[1],bool next_IsRowPixel[1],bool IsRowPixel,bool enable)
{
	static int tmp_inoffset;

	next_t2[0] = t2;
	next_IsRowPixel[0] = IsRowPixel;

	if(!enable)
		return;

	bool init = (t2==0);
	if(init)
	{
		tmp_inoffset = IN_OFFSET;
	}else
	{
		tmp_inoffset += RowIncreaseLength;
	}

	int InOffset[Tn];
#pragma HLS ARRAY_PARTITION variable=InOffset complete dim=1
	int RowOffset[Tn];
#pragma HLS ARRAY_PARTITION variable=RowOffset complete dim=1
	ap_uint<1>  LowBit[Tn];
#pragma HLS ARRAY_PARTITION variable=LowBit complete dim=1
	UCHAR BeginByteNum[Tn];
#pragma HLS ARRAY_PARTITION variable=BeginByteNum complete dim=1
	UCHAR RowIntNum[Tn];
#pragma HLS ARRAY_PARTITION variable=RowIntNum complete dim=1

	int t1;
	for(t1 = 0;t1 < Tn;t1++)
	{
#pragma HLS UNROLL
		InOffset[t1] = tmp_inoffset + t1*IHxIW_18b;
		RowOffset[t1] = InOffset[t1] >> 1;
		LowBit[t1] = InOffset[t1]&0x1;
		RowBeginByte[t1] = LowBit[t1];
		BeginByteNum[t1] = ColIncreaseLength + LowBit[t1];

//		assert((BeginByteNum[t1] > 0)&&(BeginByteNum[t1] < 256));

		RowIntNum[t1] = BeginByteNum[t1] >> 1;
		if(BeginByteNum[t1]&0x1)
			RowIntNum[t1]++;

//		assert((RowIntNum[t1] > 0)&&(RowIntNum[t1] < 256));
	}

	mmcpy_inputport(input,input_memcpy_buffer, TN_MIN_3b,RowOffset[0],RowIntNum[0]);
	mmcpy_inputport1(input1,input_memcpy_buffer1, TN_MIN_3b,RowOffset[1],RowIntNum[1]);
	mmcpy_inputport2(input2,input_memcpy_buffer2, TN_MIN_3b,RowOffset[2],RowIntNum[2]);
	mmcpy_inputport3(input3,input_memcpy_buffer3, TN_MIN_3b,RowOffset[3],RowIntNum[3]);
}

void copy_input2buf_row(short input_buffer[Tn][OnChipIB_Height][OnChipIB_Width],ap_uint<6> row_len,ap_uint<6> col_len,ap_uint<1> RowSub,ap_uint<1> ColSub,
					int input_memcpy_buffer[(OnChipIB_Width+3)/2],int input_memcpy_buffer1[(OnChipIB_Width+3)/2],
					int input_memcpy_buffer2[(OnChipIB_Width+3)/2],int input_memcpy_buffer3[(OnChipIB_Width+3)/2],
					ap_uint<1>  RowBeginByte[Tn],UCHAR TRow,UCHAR TCol,int LayerType,ap_uint<6> next_t2[1],bool next_enable[1],bool enable,ap_uint<3> T2Rate)
{

	if(!enable)
		return;

	static ap_uint<6> t2_local = 0;
	ap_uint<6> t2 = next_t2[0];
	bool IsRowPixel = next_enable[0];
	int t1,t3;
	ap_uint<6> t2r;
	ap_uint<3> T2R;

	bool initial = (t2==0);
	if(initial)
	{
		t2_local = 0;
	}

	short pad_value = 0;
	/*
	if(LayerType==1)
		pad_value = 0x8001;
	*/
	int input_mmcpy_offset[Tn];
#pragma HLS ARRAY_PARTITION variable=input_mmcpy_offset complete dim=1
	bool NextInputFlag[Tn];
#pragma HLS ARRAY_PARTITION variable=NextInputFlag complete dim=1
	ap_uint<1>  cnt[Tn];
#pragma HLS ARRAY_PARTITION variable=cnt complete dim=1
	short input_array[Tn][2];
#pragma HLS ARRAY_PARTITION variable=input_array complete dim=1

	for(t1 = 0;t1 < Tn; t1++)
	{
#pragma HLS UNROLL
		input_mmcpy_offset[t1] = 0;
	}

	if(!IsRowPixel)
	{
		T2R = T2Rate + 1;
	}else
	{
		T2R = T2Rate;
	}
//	ap_uint<6> T2R_bound = MIN(t2_local + T2R,OnChipIB_Height);
	unsigned char tmp_min = t2_local + T2R;
	ap_uint<6> T2R_bound = MIN(tmp_min, OnChipIB_Height);

	bool IsRowInit_flag = true;

	for(t2r = t2_local;t2r < T2R_bound; t2r++)
		for(t3 = 0;t3 < TCol; t3++)
		{
#pragma HLS PIPELINE
			bool IsRowPixel_t2r = (t2r >= RowSub)&&(t2r < (row_len + RowSub));
			bool IsColPixel = (t3 >= ColSub)&&(t3 < (col_len + ColSub));
			bool IsRowInit = (t3==ColSub)&&IsRowInit_flag;

			if(IsRowPixel_t2r&&IsColPixel)
			{
				if(IsRowInit)
				{
					IsRowInit_flag = false;
					cnt[0] = RowBeginByte[0];
					cnt[1] = RowBeginByte[1];
					cnt[2] = RowBeginByte[2];
					cnt[3] = RowBeginByte[3];
					NextInputFlag[0] = true;
					NextInputFlag[1] = true;
					NextInputFlag[2] = true;
					NextInputFlag[3] = true;
				}

				if(NextInputFlag[0])
				{
					input_array[0][0] = input_memcpy_buffer[input_mmcpy_offset[0]];
					input_array[0][1] = input_memcpy_buffer[input_mmcpy_offset[0]] >> 16;
					input_mmcpy_offset[0]++;
					NextInputFlag[0] = false;
				}

				if(NextInputFlag[1])
				{
					input_array[1][0] = input_memcpy_buffer1[input_mmcpy_offset[1]];
					input_array[1][1] = input_memcpy_buffer1[input_mmcpy_offset[1]] >> 16;
					input_mmcpy_offset[1]++;
					NextInputFlag[1] = false;
				}

				if(NextInputFlag[2])
				{
					input_array[2][0] = input_memcpy_buffer2[input_mmcpy_offset[2]];
					input_array[2][1] = input_memcpy_buffer2[input_mmcpy_offset[2]] >> 16;
					input_mmcpy_offset[2]++;
					NextInputFlag[2] = false;
				}

				if(NextInputFlag[3])
				{
					input_array[3][0] = input_memcpy_buffer3[input_mmcpy_offset[3]];
					input_array[3][1] = input_memcpy_buffer3[input_mmcpy_offset[3]] >> 16;
					input_mmcpy_offset[3]++;
					NextInputFlag[3] = false;
				}

				input_buffer[0][t2r][t3] = input_array[0][cnt[0]];
				input_buffer[1][t2r][t3] = input_array[1][cnt[1]];
				input_buffer[2][t2r][t3] = input_array[2][cnt[2]];
				input_buffer[3][t2r][t3] = input_array[3][cnt[3]];

				if(cnt[0]==1)
				{
					NextInputFlag[0] = true;
					cnt[0] = 0;
				}else
				{
					cnt[0] = 1;
				}

				if(cnt[1]==1)
				{
					NextInputFlag[1] = true;
					cnt[1] = 0;
				}else
				{
					cnt[1] = 1;
				}

				if(cnt[2]==1)
				{
					NextInputFlag[2] = true;
					cnt[2] = 0;
				}else
				{
					cnt[2] = 1;
				}

				if(cnt[3]==1)
				{
					NextInputFlag[3] = true;
					cnt[3] = 0;
				}else
				{
					cnt[3] = 1;
				}
			}else
			{
				input_buffer[0][t2r][t3] = pad_value;
				input_buffer[1][t2r][t3] = pad_value;
				input_buffer[2][t2r][t3] = pad_value;
				input_buffer[3][t2r][t3] = pad_value;
			}

		}

		t2_local += T2R;
}


//到底影响不影响啊？喵喵喵？
/*
mmcpy_inputpixel_m2b_comb pingpong了这个函数
*/
void input_load(int *input,int *input1,int *input2,int *input3,
				short input_buffer[Tn][OnChipIB_Height][OnChipIB_Width],int r,int c,int n,int Kernel_stride,int Padding,UCHAR TRow,UCHAR TCol,int Input_w,int Input_h,int TN_MIN,int IHxIW,int LayerType,ap_uint<6> trow_loops)
{
	static int input_memcpy_buffer0[(OnChipIB_Width+3)/2];
	static int input_memcpy_buffer1[(OnChipIB_Width+3)/2];
	static int input_memcpy_buffer2[(OnChipIB_Width+3)/2];
	static int input_memcpy_buffer3[(OnChipIB_Width+3)/2];
	ap_uint<1> RowBeginByte[Tn];
#pragma HLS ARRAY_PARTITION variable=RowBeginByte complete dim=1//0 ro 1

	static int input_memcpy_buffer02[(OnChipIB_Width+3)/2];
	static int input_memcpy_buffer12[(OnChipIB_Width+3)/2];
	static int input_memcpy_buffer22[(OnChipIB_Width+3)/2];
	static int input_memcpy_buffer32[(OnChipIB_Width+3)/2];
	ap_uint<1> RowBeginByte2[Tn];//0 ro 1
#pragma HLS ARRAY_PARTITION variable=RowBeginByte2 complete dim=1//0 ro 1

	ap_uint<1> RowSub,ColSub;

	ap_uint<6> t2;

	ap_uint<9> r_9b = r;
//	assert(r < 512);
	ap_uint<9> c_9b = c;
//	assert(c < 512);
//	assert(n < 2048);
	ap_uint<11> n_11b = n;
//	assert(Kernel_stride < 4);
	ap_uint<2> Kernel_stride_2b = Kernel_stride;
//	assert(Padding < 2);
	ap_uint<1> Padding_1b = Padding;
//	assert(Input_w < 512);
	ap_uint<9> Input_w_9b = Input_w;
	ap_uint<10> Input_h_10b = Input_h;
//	assert(Input_h < 1024);
//	assert(TN_MIN < 8);//xx8
	ap_uint<3> TN_MIN_3b = TN_MIN;
	ap_uint<18> IHxIW_18b = IHxIW;
//	assert(IHxIW < 512*512);

	ap_int<12> Coffset;
	ap_int<12> Roffset;
	//   Coffset = c_9b*Kernel_stride_2b - Padding_1b;
	//   Roffset = r_9b*Kernel_stride_2b - Padding_1b;
	if(LayerType == 2){
	    Coffset = c_9b;
	    Roffset = r_9b;
	} else {
	   Coffset = c_9b*Kernel_stride_2b - Padding_1b;
	   Roffset = r_9b*Kernel_stride_2b - Padding_1b;
	}
	 
	ap_uint<12> TCol_right,TRow_bottom;
	ap_uint<10> TRow_top,TCol_left;
	ap_uint<6> row_len,col_len;

	if(Coffset > 0)
		TCol_left = Coffset;
	else
		TCol_left = 0;

	if((Coffset + TCol-1)<Input_w_9b)
		TCol_right = Coffset + TCol;
	else
		TCol_right = Input_w_9b;

	col_len = TCol_right - TCol_left;

	if(Roffset > 0)
		TRow_top = Roffset;
	else
		TRow_top = 0;

	if((Roffset + TRow-1)<Input_h_10b)
		TRow_bottom = Roffset + TRow;
	else
		TRow_bottom = Input_h_10b;

	row_len = TRow_bottom - TRow_top;

	int IN_OFFSET = n_11b*IHxIW_18b + TRow_top*Input_w_9b +TCol_left;

	ap_uint<9> RowIncreaseLength;
	ap_uint<6> ColIncreaseLength;
	ap_uint<3> T2Rate;
	switch(Input_w_9b)
	{
		case Tr:
			RowIncreaseLength = 2*Tr;
			ColIncreaseLength = 2*Tr;
			T2Rate = 2;
			break;
		case SIZE:
			RowIncreaseLength = 4*SIZE;
			ColIncreaseLength = 4*SIZE;
			T2Rate = 4;
			break;
		default:
			RowIncreaseLength = Input_w_9b;
			ColIncreaseLength = col_len;
			T2Rate = 1;
			break;
	}

	//assert(ColNum < 64*64);
	//assert(RowNum < 64);
	RowSub = TRow_top - Roffset;
	ColSub = TCol_left - Coffset;

	bool pingpong = 1;
	ap_uint<6> next_t2[1];
	bool next_IsRowPixel[1];
	ap_uint<6> next_t22[1];
	bool next_IsRowPixel2[1];

//	ap_uint<6> trow_loops = (int)ceil(((float)TRow/T2Rate));
	ap_uint<6> TMP_t2;
	for(TMP_t2 = 0,t2 = 0;TMP_t2 < trow_loops + 1; t2 += T2Rate,TMP_t2++)
	{
		bool IsRowPixel = (t2 >= RowSub)&&(t2 < (row_len + RowSub));

		if(pingpong == 1)
		{
			mmcpy_inputpixel_m2b_comb(input,input1,input2,input3,
							   input_memcpy_buffer0, input_memcpy_buffer1,
							   input_memcpy_buffer2, input_memcpy_buffer3,
							   RowBeginByte, TN_MIN_3b, t2, RowSub, IN_OFFSET, RowIncreaseLength, IHxIW_18b, ColIncreaseLength, next_t2,next_IsRowPixel,IsRowPixel,TMP_t2!=trow_loops);

			copy_input2buf_row( input_buffer, row_len, col_len, RowSub, ColSub,
						 input_memcpy_buffer02, input_memcpy_buffer12,input_memcpy_buffer22, input_memcpy_buffer32,
						RowBeginByte2, TRow, TCol,LayerType,next_t22,next_IsRowPixel2,TMP_t2!=0,T2Rate);
			pingpong = 0;
		}else
		{
			mmcpy_inputpixel_m2b_comb(input,input1,input2,input3,
							   input_memcpy_buffer02, input_memcpy_buffer12,
							   input_memcpy_buffer22, input_memcpy_buffer32,
							   RowBeginByte2, TN_MIN_3b, t2, RowSub, IN_OFFSET, RowIncreaseLength, IHxIW_18b, ColIncreaseLength, next_t22,next_IsRowPixel2,IsRowPixel,TMP_t2!=trow_loops);

			copy_input2buf_row( input_buffer, row_len, col_len, RowSub, ColSub,
						 input_memcpy_buffer0, input_memcpy_buffer1,input_memcpy_buffer2, input_memcpy_buffer3,
						RowBeginByte, TRow, TCol,LayerType,next_t2,next_IsRowPixel,TMP_t2!=0,T2Rate);
			pingpong = 1;
		}
	}

//	assert(TRow_top < 1024);
//	assert(TCol_left < 1024);

}

void weight_mmcpy_everyKxK(int *Weight,int weight_memcpy_buffer[Tm*Tn/2],ap_uint<3> t3,ap_uint<3> t4,ap_uint<3> next_t3[1],ap_uint<3> next_t4[1],unsigned int ReadLength,bool init_enable,bool enable)
{
	if(!enable)
		return;

	static int Woffset;
	next_t3[0] = t3;
	next_t4[0] = t4;

	if(init_enable)
	{
		Woffset = 0;
	}

	memcpy(weight_memcpy_buffer,(int *)(Weight + Woffset),ReadLength*sizeof(int));
	Woffset += ReadLength;
}

void load_weight2buf_everyKxK(int weight_memcpy_buffer[Tm*Tn/2],short weight_buffer[Tm][Tn][K][K],ap_uint<3> t3,ap_uint<3> t4,ap_uint<6> TM_MIN,ap_uint<3> TN_MIN,bool enable)
{

	if(!enable)
		return;

	ap_uint<5> t1;
	ap_uint<3> t2;
	ap_uint<8> weight_memcpy_offset = 0;
	ap_uint<2> cnt = 0;
	short input_array[2];
#pragma HLS ARRAY_PARTITION variable=input_array complete dim=1
	short input_value;

	for(t1 = 0;t1 < Tm; t1++)
		for(t2 = 0;t2 < Tn; t2++)
		{
#pragma HLS PIPELINE
			bool Enable = (t1 < TM_MIN)&&(t2 < TN_MIN);
			if(Enable)
			{
				if(cnt==0)
				{
					input_array[0] = weight_memcpy_buffer[weight_memcpy_offset];
					input_array[1] = weight_memcpy_buffer[weight_memcpy_offset] >> 16;
					weight_memcpy_offset++;
				}
				input_value = input_array[cnt];

				cnt++;
				if(cnt==2)
					cnt = 0;
			}
			else
				input_value = 0;

			weight_buffer[t1][t2][t3][t4] =  input_value;
			//weight_buffer[0][0][t3][t4] =  input_value;
		}
}

void weight_load_reorg(int *Weight,short weight_buffer[Tm][Tn][K][K],bool weight_load_enable,int m,int n,int IFM_numxKxK,int KxK,int Kernel_size,int TM_MIN,int TN_MIN)
{
	/*int t1,t2,t3,t4;*/
	static int weight_memcpy_buffer[Tm*Tn/2];
	static int weight_memcpy_buffer1[Tm*Tn/2];

	if(!weight_load_enable)
		return;

//	assert(m < 1024);
//	assert(n < 2048);//gg2048
//	assert(IFM_numxKxK < 1024*16);
//	assert(Kernel_size < 4);
//	assert(TM_MIN < 64);
//	assert(TN_MIN < 8);//xx8

	ap_uint<2> Kernel_size_2b = Kernel_size;
	ap_uint<6> TM_MIN_6b = TM_MIN;
	ap_uint<3> TN_MIN_3b = TN_MIN;

	ap_uint<10> m_10b = m;
	ap_uint<11> n_11b = n;

	bool Me0aNe0 = (m_10b==0)&&(n_11b==0);
	unsigned int ReadLength = (TM_MIN_6b*TN_MIN_3b)>>1;

//	if((TM_MIN*TN_MIN)%2)
//		printf("weight % error\n");

	ap_uint<3> t3,t4;
	ap_uint<3> next_t3[1];
	ap_uint<3> next_t4[1];
	ap_uint<3> next_t31[1];
	ap_uint<3> next_t41[1];

	bool pingpong = true;

	for(t3 = 0;t3 < Kernel_size_2b;t3++)
		for(t4 = 0;t4 < Kernel_size_2b + 1;t4++)
		{
			if(pingpong)
			{
				weight_mmcpy_everyKxK(Weight, weight_memcpy_buffer, t3, t4,next_t3,next_t4, ReadLength,Me0aNe0&&(t3==0)&&(t4==0),t4!=Kernel_size_2b);
				load_weight2buf_everyKxK(weight_memcpy_buffer1, weight_buffer, next_t31[0], next_t41[0], TM_MIN, TN_MIN,t4!=0);
				pingpong = false;
			}else
			{
				weight_mmcpy_everyKxK(Weight, weight_memcpy_buffer1, t3, t4,next_t31,next_t41, ReadLength,Me0aNe0&&(t3==0)&&(t4==0),t4!=Kernel_size_2b);
				load_weight2buf_everyKxK(weight_memcpy_buffer, weight_buffer, next_t3[0], next_t4[0], TM_MIN, TN_MIN,t4!=0);
				pingpong = true;
			}
		}
}


void copy_input_weight(int *input,int *input1,int *input2,int *input3,int *Weight,int InFM_num,int Input_w,int Input_h,int Kernel_size,int Kernel_stride,int r,int c,int m,int n,
		int TM_MIN,int TN,UCHAR TRow,UCHAR TCol,int Padding,short input_buffer[Tn][OnChipIB_Height][OnChipIB_Width],short weight_buffer[Tm][Tn][K][K],int TMP_N_next[1],
		bool enable,bool weight_load_enable,bool initialize,const int IHxIW,const int KxK,const int IFM_numxKxK,const int LayerType,ap_uint<6> trow_loops)
{
	if(!enable)
		return ;

	const int TN_MIN = MIN(TN,InFM_num - n);
	TMP_N_next[0] = n;

	input_load(input,input1,input2,input3, input_buffer, r, c, n, Kernel_stride, Padding, TRow, TCol, Input_w, Input_h, TN_MIN, IHxIW, LayerType,trow_loops);
	weight_load_reorg(Weight,weight_buffer,weight_load_enable,m,n,IFM_numxKxK,KxK,Kernel_size,TM_MIN,TN_MIN);

}

//////////////////////////////////////////////////T3 end

void copy_local_beta(short beta_buffer[MAX_BETA_LENGTH],int local_beta_buffer[MAX_BETA_LENGTH],const int TM_MIN,int m,UCHAR InterSubBeta)
{
	ap_uint<4> InterSubBeta_4b = InterSubBeta;
	int offset;
	int tm;
	for(tm = 0,offset = m;tm < TM_MIN;tm++)
	{
#pragma HLS PIPELINE
		local_beta_buffer[tm] = beta_buffer[offset] << InterSubBeta_4b;
		offset++;
	}
}

///#######################################################################################################################
//现在缺少位数对齐
/*
输入大小是没有改动的所以不用管，但是输出是要改动的
*/
//buffer1需要和buffer0对其
//In1Sub和buff1对应， In0Sub和buffer0对应
//In1Sub 是要处理的值
void shortcut(short input_buffer0[Tn][OnChipIB_Height][OnChipIB_Width],short input_buffer1[Tn][OnChipIB_Height][OnChipIB_Width],
        int output_buffer[Tm][Tr*PARA][Tc*PARA],const int TM_MIN,const int TR_MIN,
        const int TC_MIN,UCHAR In0Sub,UCHAR In1Sub,bool enable){
//对于shortcut来说，只有其中的一个需要和输出对其，那么问题来了，究竟是左移还是右移？
    ap_uint<5> tr,tc,tm;
    if(!enable){
		return;
    }
	
	ap_uint<5> TM_MIN_5b = TM_MIN;
	ap_uint<5> TR_MIN_5b = TR_MIN;
	ap_uint<5> TC_MIN_5b = TC_MIN;
	
	ap_uint<4> In0Sub_4b = In0Sub;
	ap_uint<4> In1Sub_4b = INTERWIDTH - In1Sub;
	//由于不确定大小关系，所以不能直接减法求出差值然后计算，只能先同步到相同的位置，那就是 INTERWIDTH的位数，然后加法以后再处理回去

    for(tm = 0; tm < TM_MIN_5b;tm++){
        for(tr = 0; tr < TR_MIN_5b;tr++){
            for(tc = 0; tc < TC_MIN_5b; tc++){
#pragma HLS PIPELINE
                int tempt_add0 = input_buffer0[tm][tr][tc] << In0Sub_4b;
                int tempt_add1 = input_buffer1[tm][tr][tc] << In1Sub_4b;
				//用32位容纳一个＜20位的数值，不会有精度损失，然后做加法
				int tempt_out = (tempt_add0 + tempt_add1) >> In0Sub_4b;
				//output_buffer[tm][tr][tc] = input_buffer0[tm][tr][tc] + input_buffer1[tm][tr][tc];
				output_buffer[tm][tr][tc] = tempt_out;
				//int 可以给short吗？
				//所以数值上是int
				//那么现在就存在一个问题了，究竟是需不需要右移呢？
				//先试一下需要右移的吧
            }
        }
    }

}
///#######################################################################################################################
void upsample(short input_bufferInput[Tn][OnChipIB_Height][OnChipIB_Width],int output_buffer[Tm*PARA][Tr*PARA][Tc*PARA],
        const int upsample_size,const int TM_MIN,const int TR_MIN,
        const int TC_MIN,bool enable){
    ap_uint<5> tr,tc,tm,i,j;
    if(!enable){
		return;
    }
	
	ap_uint<2> upsample_size_2b = upsample_size;

	ap_uint<5> TM_MIN_5b = TM_MIN;
	ap_uint<5> TR_MIN_5b = TR_MIN;
	ap_uint<5> TC_MIN_5b = TC_MIN;

    for(tm = 0; tm < TM_MIN_5b;tm++){
        for(tr = 0; tr < TR_MIN_5b;tr++){
            for(tc = 0; tc < TC_MIN_5b; tc++){
//#pragma HLS PIPELINE
                for(i = 0; i < upsample_size_2b;i++){
#pragma HLS UNROLL
                    for(j = 0;j < upsample_size_2b;j++){
#pragma HLS UNROLL
                            output_buffer[tm][tr*upsample_size_2b+i][tc*upsample_size_2b+j] = input_bufferInput[tm][tr][tc];
                            //printf("value = %hd\n",input_bufferInput[tm][tr][tc]);
                        }
                }
            }
        }
    }
}

///#######################################################################################################################
void compute(short input_buffer[Tn][OnChipIB_Height][OnChipIB_Width],int output_buffer[Tm][Tr*PARA][Tc*PARA],
		short weight_buffer[Tm][Tn][K][K],short beta_buffer[MAX_BETA_LENGTH],int TMP_N_next[1],
		const int Kernel_size,const int Kernel_stride,int TMP_M,
		const int TM_MIN,const int TR_MIN,const int TC_MIN,bool enable,const bool IsNL,const bool reluenable,
		UCHAR InterSubBeta,UCHAR WeightAddInputSubInter,UCHAR InterSubOutput)
{
//输出完全没用到啊？InterSubOutput
//计算以后直接移位我服了，为什么是右移这个位数？
//因为两个运算了以后会超过位数
	static int local_beta_buffer[Tm];
#pragma HLS ARRAY_PARTITION variable=local_beta_buffer complete dim=1

//	static int compute_buffer[Tm][Tr][Tc];
//#pragma HLS ARRAY_PARTITION variable=compute_buffer complete dim=1

	if(!enable)
	{
		copy_local_beta(beta_buffer,local_beta_buffer,TM_MIN,TMP_M,InterSubBeta);
		return;
	}

	int partial_mul[Tm][Tn];
#pragma HLS ARRAY_PARTITION variable=partial_mul complete dim=1
#pragma HLS ARRAY_PARTITION variable=partial_mul complete dim=2

	ap_uint<2> i,j;
	UCHAR tm,tn;
	ap_uint<5> tr,tc;
	ap_uint<2> Kernel_size_2b = Kernel_size;
	ap_uint<2> Kernel_stride_2b = Kernel_stride;

	ap_uint<5> TR_MIN_5b = TR_MIN;
	ap_uint<5> TC_MIN_5b = TC_MIN;

//	ap_uint<4> InterSubBeta_4b = InterSubBeta;
	ap_uint<4> WeightAddInputSubInter_4b = WeightAddInputSubInter;

//	assert(InterSubBeta < 16);
//	assert(WeightAddInputSubInter < 16);
//	assert(InterSubOutput < 16);

//	assert(Kernel_size < 4);
//	assert(TR_MIN < 32);
//	assert(TC_MIN < 32);

	ap_uint<11> n = TMP_N_next[0];
//	assert(n < 2048);

	for(i = 0;i < Kernel_size_2b; i++)
		for(j = 0;j < Kernel_size_2b; j++)
			for(tr = 0;tr < TR_MIN_5b;tr++)
				for(tc = 0;tc < TC_MIN_5b;tc++)
				{
#pragma HLS PIPELINE
					for(tm = 0;tm < Tm;tm++)
					{
#pragma HLS DEPENDENCE variable=output_buffer inter false
						int tmp_add_result;
						if(i==0&&j==0&&n==0)
						{
							tmp_add_result = local_beta_buffer[tm];
						}
						else
							tmp_add_result = output_buffer[tm][tr][tc];

						partial_mul[tm][0] = (weight_buffer[tm][0][i][j]*input_buffer[0][tr*Kernel_stride_2b+i][tc*Kernel_stride_2b+j]) >> WeightAddInputSubInter_4b;//Q1+Q2-Q3
						partial_mul[tm][1] = (weight_buffer[tm][1][i][j]*input_buffer[1][tr*Kernel_stride_2b+i][tc*Kernel_stride_2b+j]) >> WeightAddInputSubInter_4b;//Q1+Q2-Q3
						partial_mul[tm][2] = (weight_buffer[tm][2][i][j]*input_buffer[2][tr*Kernel_stride_2b+i][tc*Kernel_stride_2b+j]) >> WeightAddInputSubInter_4b;//Q1+Q2-Q3
						partial_mul[tm][3] = (weight_buffer[tm][3][i][j]*input_buffer[3][tr*Kernel_stride_2b+i][tc*Kernel_stride_2b+j]) >> WeightAddInputSubInter_4b;//Q1+Q2-Q3

						int tmp_add1 = partial_mul[tm][0] + partial_mul[tm][1];
						int tmp_add2 = partial_mul[tm][2] + partial_mul[tm][3];
						int tmp_add12 = tmp_add1 + tmp_add2;
						output_buffer[tm][tr][tc] = tmp_add_result + tmp_add12;

//						partial_mul[tm][0] = (weight_buffer[tm][0][i][j]*input_buffer[0][tr+i][tc+j]) >> WeightAddInputSubInter_4b;//Q1+Q2-Q3
//						partial_mul[tm][1] = (weight_buffer[tm][1][i][j]*input_buffer[1][tr+i][tc+j]) >> WeightAddInputSubInter_4b;//Q1+Q2-Q3
//
//						compute_buffer[tm][tr][tc] = tmp_add_result + partial_mul[tm][0] + partial_mul[tm][1];
					}
				}
}

//////////////version-0.2 start
void mmcpy_outputport(int *Output,int output_tmp[Tr*PARA*Tc*PARA/4],ap_uint<6> tm,ap_uint<6> mLoop,int OutputOffset,int OutputLength)
{
	bool enable = tm < mLoop;
	if(!enable)
		return;

	memcpy((int *)(Output + OutputOffset),(int *)(output_tmp),OutputLength*sizeof(int));
}

void mmcpy_outputport1(int *Output,int output_tmp[Tr*PARA*Tc*PARA/4],ap_uint<6> tm,ap_uint<6> mLoop,int OutputOffset,int OutputLength)
{
	bool enable = tm < mLoop;
	if(!enable)
		return;

	memcpy((int *)(Output + OutputOffset),(int *)(output_tmp),OutputLength*sizeof(int));
}


/*
并行调用拷贝进端口的函数
*/
void mmcpy_outputpixel(int *Output,int *Output1,int output_tmp[Tr*PARA*Tc*PARA/4],int output_tmp1[Tr*PARA*Tc*PARA/4],ap_uint<6> tm,ap_uint<6> mLoop1,ap_uint<6> mLoop2,int outputoffsetarray[2],int OutputLength,int OutputLength1,bool enable)
{
	if(!enable)
	{
		return;
	}
	mmcpy_outputport(Output ,output_tmp ,tm,mLoop1,outputoffsetarray[0],OutputLength );
	mmcpy_outputport1(Output1,output_tmp1,tm,mLoop2,outputoffsetarray[1],OutputLength1);
}

/*
不知道是干嘛的
*/
void outputpixel2buf(int output_buffer[Tm][Tr*PARA][Tc*PARA],int output_tmp[Tr*PARA*Tc*PARA/4],int output_tmp1[Tr*PARA*Tc*PARA/4],bool IsNL,int InterSubOutput,int LayerType,bool TC_MINe26,int TR_MIN,int TC_MIN,int mLoop,int rLoop, bool init,
					 int outputoffsetarray[2],int OutputOffset1_sum,int OutputOffset1_sum1,int OutputOffset2_sum,ap_uint<6> tm_next[1],bool enable)
{
	if(!enable)
	{
		return;
	}

	tm_next[0] =  mLoop;

	ap_uint<4> InterSubOutput_4b = InterSubOutput;
	int tmp_output;
	int tmp_output_1;
	short tmp_output2;
	short tmp_output2_1;
	int tmp_output3;
	int tmp_output3_1;
	ap_uint<2> cnt = 0;
	short ouput_array[2];
#pragma HLS ARRAY_PARTITION variable=ouput_array complete dim=1
	short ouput_array1[2];
#pragma HLS ARRAY_PARTITION variable=ouput_array1 complete dim=1
	ap_uint<5> tr;
	static ap_uint<6> tm;

	ap_uint<5> TC_MIN_5b = TC_MIN;
	ap_uint<5> tc;
	ap_uint<2> TM_LOOP,tm_count;
	ap_uint<4> TR_LOOP,tr_count;
	
	
	//const int para = (LayerType==2) ? 2:1;
	if(init)
	{
		tm = 0;
	}

	if(TC_MINe26)
	{
		tm = mLoop;
		tr = rLoop;
		TM_LOOP = 1;
		TR_LOOP = 1;
	}else
	{
		tr = 0;
		TM_LOOP = 2;
		TR_LOOP = SIZE;
	}

	ap_uint<8> outputoffset = 0;
	ap_uint<8> outputoffset1 = 0;
	
	for(tm_count = 0;tm_count < TM_LOOP;tm_count++,tm++,tr = 0)
		for(tr_count = 0;tr_count < TR_LOOP;tr_count++,tr++)
			for(tc = 0;tc < TC_MIN_5b;tc++)
			{
#pragma HLS PIPELINE
				int tmp = output_buffer[tm][tr][tc];
				int tmp1 = output_buffer[tm + Tm/2][tr][tc];
				if(IsNL&&tmp<0)
				{
					tmp_output = ((long long)tmp*0xccc)>>15;
				}else
				{
					tmp_output = tmp;
				}

				if(IsNL&&tmp1<0)
				{
					tmp_output_1 = ((long long)tmp1*0xccc)>>15;
				}else
				{
					tmp_output_1 = tmp1;
				}

				if(LayerType==0)
				{
					tmp_output2 = tmp_output >> InterSubOutput_4b;
					tmp_output2_1 = tmp_output_1 >> InterSubOutput_4b;
				}
				else
				{
					tmp_output2 = tmp_output;
					tmp_output2_1 = tmp_output_1;
				}
				ouput_array[cnt] = tmp_output2;
				ouput_array1[cnt] = tmp_output2_1;
				cnt++;
				if(cnt==2)
				{
					tmp_output3 = (ouput_array[0]       &0x0000FFFF) |
							((ouput_array[1] << 16 )&0xFFFF0000);
					tmp_output3_1 = (ouput_array1[0]       &0x0000FFFF) |
							((ouput_array1[1] << 16 )&0xFFFF0000);

					output_tmp[outputoffset] = tmp_output3;
					outputoffset++;

					output_tmp1[outputoffset1] = tmp_output3_1;
					outputoffset1++;
					cnt = 0;
				}
			}

	outputoffsetarray[0] = (OutputOffset1_sum  + OutputOffset2_sum)>>1;
	outputoffsetarray[1] = (OutputOffset1_sum1 + OutputOffset2_sum)>>1;

}


//para 还没有设置好
void write_back_output_reorg(int output_buffer[Tm][Tr*PARA][Tc*PARA],int *Output,int *Output1,int r,int c,int m,const int Output_w,const int Output_h,
					   const int TM_MIN,const int TR_MIN,const int TC_MIN,const int OHxOW,bool write_flag,const int OutputQ,bool IsNL,int InterSubOutput,int LayerType)
{
	static int output_tmp00[Tr*PARA*Tc*PARA/4];
	static int output_tmp01[Tr*PARA*Tc*PARA/4];

	static int output_tmp10[Tr*PARA*Tc*PARA/4];
	static int output_tmp11[Tr*PARA*Tc*PARA/4];
	//NOTICE: si ge jia qi lai zhi you output_buffer de 1/Tm
	
	//const int para = (LayerType==2) ? 2:1;

	int tr,tm,tc;
	int OutputLength,OutputLength1;
	int mLoopc,mLoop,rLoop;
	ap_uint<6> mLoop1,mLoop2;

	if(!write_flag)
		return;

//	assert(TM_MIN < 64);
	assert(TR_MIN < 32);
	assert(TC_MIN < 32);

	ap_uint<6> TM_MIN_6b = TM_MIN;
	ap_uint<18> OHxOW_18b = OHxOW;
	ap_uint<9> Output_w_9b = Output_w;
	ap_uint<10> m_10b = m;
	ap_uint<9> r_9b = r;
	ap_uint<9> c_9b = c;

//	assert(m < 1024);
//	assert(r < 512);
//	assert(c < 512);
//	assert(OHxOW < 512*512);
//	assert(Output_w < 512);

	ap_uint<6> TM_MIN_g;
	//??????==9 shi sha cao zuo ?
	if(TM_MIN_6b==9)
		TM_MIN_g = 12;
	else
		TM_MIN_g = TM_MIN_6b;

	
	int tempoff;
	if(LayerType == 2){
	    tempoff = m*OHxOW_18b + r*Output_w_9b*2 + c_9b*2;
	} else {
        tempoff = m*OHxOW_18b + r*Output_w_9b + c_9b;
	}
	//const int offset = m_10b*OHxOW_18b + r_9b*Output_w_9b + c_9b;
	const int offset = tempoff;

	bool TM_MINaboveTmdiv2 = TM_MIN_g > Tm/2;
	bool TC_MINe26 = TC_MIN == Tr;

	if(TM_MINaboveTmdiv2)
	{
		mLoop = Tm/2;
		mLoop1 = Tm/2;
		mLoop2 = TM_MIN_g - Tm/2;
	}else
	{
		mLoop = TM_MIN_g;
		mLoop1 = TM_MIN_g;
		mLoop2 = 0;
	}
	mLoopc = mLoop;

	int offset1 = offset + mLoop1*OHxOW_18b;

	int OutputOffset1,OutputOffset2;
	int OutputOffset1_sum,OutputOffset1_sum1;
	int OutputOffset2_sum;

	// when TC_MIN==26,burstlength = 13*2/2=13,else 13*13*2/2=169
	if(TC_MINe26)
	{
		OutputLength = Tr/2;
		OutputLength1 = Tr/2;
		OutputOffset1 = OHxOW_18b;
		OutputOffset2 = Output_w_9b;
		rLoop = 26;
	}else//TMxTRxTC TMx13x13 continues
	{
		OutputLength = SIZE*SIZE;
		OutputLength1 = SIZE*SIZE;
		rLoop = 1;
		mLoop = mLoop/2;
		OutputOffset1 = SIZE*SIZE*2;
		OutputOffset2 = 0;
	}

	bool pingpong = true;
	int outputoffsetarray[2];
#pragma HLS ARRAY_PARTITION variable=outputoffsetarray complete dim=1
	int outputoffsetarray1[2];
#pragma HLS ARRAY_PARTITION variable=outputoffsetarray1 complete dim=1
	ap_uint<6> tm_next[1];
	ap_uint<6> tm_next1[1];
	bool wb_start_flag = true;
	for(tm = 0,OutputOffset1_sum = offset,OutputOffset1_sum1 = offset1;tm < mLoop;tm++,OutputOffset1_sum += OutputOffset1,OutputOffset1_sum1 += OutputOffset1)
		for(tr = 0,OutputOffset2_sum = 0;tr < rLoop + 1;tr++,OutputOffset2_sum += OutputOffset2,wb_start_flag = false)
		{
			if(pingpong)
			{
				outputpixel2buf( output_buffer, output_tmp00, output_tmp01, IsNL, InterSubOutput, LayerType, TC_MINe26, TR_MIN, TC_MIN, tm, tr,wb_start_flag,
					  outputoffsetarray, OutputOffset1_sum, OutputOffset1_sum1, OutputOffset2_sum,tm_next,tr != rLoop);
				mmcpy_outputpixel(Output,Output1, output_tmp10, output_tmp11, tm_next1[0], mLoop1, mLoop2, outputoffsetarray1, OutputLength, OutputLength1,tr != 0);
				pingpong = false;
			}else
			{
				outputpixel2buf( output_buffer, output_tmp10, output_tmp11, IsNL, InterSubOutput, LayerType, TC_MINe26, TR_MIN, TC_MIN, tm, tr,wb_start_flag,
					  outputoffsetarray1, OutputOffset1_sum, OutputOffset1_sum1, OutputOffset2_sum,tm_next1,tr != rLoop);
				mmcpy_outputpixel(Output,Output1, output_tmp00, output_tmp01, tm_next[0], mLoop1, mLoop2, outputoffsetarray, OutputLength, OutputLength1,tr != 0);
				pingpong = true;
			}
		}

}
/*
void pool_yolo2(short Input[Tn][OnChipIB_Height][OnChipIB_Width],int Output[Tm][Tr][Tc],
		  const int Kernel_size,const int Kernel_stride,
		  const int TM_MIN,const int TR_MIN,const int TC_MIN,bool enable)
{

	if(!enable)
		return;

	ap_uint<5> TR_MIN_5b = TR_MIN;
	ap_uint<5> TC_MIN_5b = TC_MIN;
	ap_uint<2> Kernel_stride_2b = Kernel_stride;

//	assert(TR_MIN < 32);
//	assert(TC_MIN < 32);
//	assert(Kernel_stride < 4);

	ap_uint<2> i,j;
	ap_uint<5> tr,tc;
//	ap_uint<8> i,j,tr,tc;
	int of;
	short tmp[Tn];
#pragma HLS ARRAY_PARTITION variable=tmp complete dim=1
	short input_short[Tn];
#pragma HLS ARRAY_PARTITION variable=input_short complete dim=1

	for(tr = 0;tr < TR_MIN_5b;tr++)
		for(tc = 0;tc < TC_MIN_5b;tc++)
			for(i =0;i < 2; i++)
				for(j = 0;j < 2; j++)
				{
#pragma HLS PIPELINE
					for( of = 0; of < Tn; of++)
					{
						if(i==0&&j==0)
							tmp[of] = 0x8001;
						input_short[of] = Input[of][tr*Kernel_stride_2b+i][tc*Kernel_stride_2b+j];
						if(input_short[of] > tmp[of])
							tmp[of] = input_short[of];

						if(i==1&&j==1)
							Output[of][tr][tc] = tmp[of];
					}
				}
}

void reorg_yolo2(short Input[Tn][OnChipIB_Height][OnChipIB_Width],int Output[Tm][Tr][Tc],
		  const int Kernel_size,const int Kernel_stride,
		  const int TM_MIN,const int TR_MIN,const int TC_MIN,bool enable)
{
	int x, y,kx,ky;
	unsigned char Yoffset;
	unsigned char Xoffset;

	if(!enable)
		return;

//	ap_uint<5> TR_MIN_5b = TR_MIN;
//	ap_uint<5> TC_MIN_5b = TC_MIN;

	assert(TR_MIN < 32);
	assert(TC_MIN < 32);

    for( y = 0; y < TR_MIN; y++)
    	for( x = 0; x < TC_MIN; x++)
			for(ky= 0;ky < 2; ky++)
    			for(kx = 0;kx < 2; kx++)
				{
#pragma HLS PIPELINE
						Yoffset = (y << 1) + ky;
						Xoffset = (x << 1) + kx;

						int in_index  = (ky << 1) + kx;
						Output[in_index][y][x] = Input[0][Yoffset][Xoffset];
    			}
}
*/
void intra_pingpong_wrapper(int *Input,int *Input1,int *Input2,int *Input3,int *Weight, int output_buffer[Tm][Tr*PARA][Tc*PARA],short beta_buffer[MAX_BETA_LENGTH],
								 short input_buffer0[Tn][OnChipIB_Height][OnChipIB_Width],short input_buffer1[Tn][OnChipIB_Height][OnChipIB_Width],
								 short input_buffer00[Tn][OnChipIB_Height][OnChipIB_Width],short input_buffer10[Tn][OnChipIB_Height][OnChipIB_Width],
								 int InFM_num,int Input_w,int Input_h,int Kernel_size,int Kernel_stride,
								 int TMP_R,int TMP_C,int TMP_M,int m,int TM_MIN,int TR_MIN,int TC_MIN,int TN,UCHAR TRow,UCHAR TCol,int Padding,
								 int IHxIW,int KxK,int IFM_numxKxK,int nLoops,bool IsNL,int LayerType,int TM,int TMP_X_next[1],int TX_MIN_next[1],bool pingpongx,bool input_flag,bool process_flag,
								 UCHAR InterSubBeta,UCHAR WeightAddInputSubInter,UCHAR InterSubOutput,ap_uint<6> trow_loops)
{

	static short weight_buffer0[Tm][Tn][K][K];
#pragma HLS ARRAY_PARTITION variable=weight_buffer0 complete dim=1
#pragma HLS ARRAY_PARTITION variable=weight_buffer0 complete dim=2

	static short weight_buffer1[Tm][Tn][K][K];
#pragma HLS ARRAY_PARTITION variable=weight_buffer1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=weight_buffer1 complete dim=2

	static int NOP[1];
	static int tmp_x;
	static int tmp_tx_min;


	if(LayerType==0)
	{
	//conv不用动，之前改好了的
		if(!input_flag)
			return;
		TMP_X_next[0] = TMP_M;//consider by the inner-out loop
		TX_MIN_next[0] = TM_MIN;// like above

		bool pingpong = 0;
		int TMP_N_next0[1];
		int TMP_N_next1[1];
		int n;
		int TMP_N;
		for(TMP_N = 0,n = 0;n < nLoops+1; n++,TMP_N += TN)
		{
			if(pingpong == 1)
			{
				copy_input_weight(Input,Input1,Input2,Input3,Weight,InFM_num,Input_w,Input_h,Kernel_size,Kernel_stride,TMP_R,TMP_C,TMP_M,TMP_N,
					TM_MIN,TN,TRow,TCol,Padding,input_buffer1,weight_buffer1,TMP_N_next1,n!=nLoops,1,(m==0)&&(n==0),IHxIW,KxK,IFM_numxKxK,LayerType,trow_loops);
				compute(input_buffer0,output_buffer,weight_buffer0,beta_buffer,TMP_N_next0,Kernel_size,Kernel_stride,TMP_M,TM_MIN,TR_MIN,TC_MIN,n!=0,IsNL,n==nLoops,
					   InterSubBeta,WeightAddInputSubInter,InterSubOutput);
				pingpong = 0;
			}else
			{
				copy_input_weight(Input,Input1,Input2,Input3,Weight,InFM_num,Input_w,Input_h,Kernel_size,Kernel_stride,TMP_R,TMP_C,TMP_M,TMP_N,
					TM_MIN,TN,TRow,TCol,Padding,input_buffer0,weight_buffer0,TMP_N_next0,n!=nLoops,1,(m==0)&&(n==0),IHxIW,KxK,IFM_numxKxK,LayerType,trow_loops);
				compute(input_buffer1,output_buffer,weight_buffer1,beta_buffer,TMP_N_next1,Kernel_size,Kernel_stride,TMP_M,TM_MIN,TR_MIN,TC_MIN,n!=0,IsNL,n==nLoops,
					   InterSubBeta,WeightAddInputSubInter,InterSubOutput);
				pingpong = 1;
			}
		}
	}
	else if(LayerType==1)
	{
		//shortcut，注意要设置stride和kernelsize为1，这是在pc上实验好的结果

		//对于shortcut的位数对齐有两个方案，第一个是在录入文件的时候就对齐，这一步就不管，但是这个也太low了
		//所以方案二就是在函数里新加内容，不过幸亏的是这个函数里有足够的接口可以用，所以选择方案二
		//	方案二：那么要如何使用这些值呢？

		if(pingpongx==0)
		{
			TMP_X_next[0] = tmp_x;
			TX_MIN_next[0] = tmp_tx_min;
			tmp_x = TMP_M;
			tmp_tx_min = TM_MIN;

			//copy_input_weight(Input,Input1,Input2,Input3,Weight,InFM_num,Input_w,Input_h,Kernel_size,Kernel_stride,TMP_R,TMP_C,TMP_M,TMP_M,
			//	TM_MIN,TM,TRow,TCol,0,input_buffer0,weight_buffer0,NOP,input_flag,0,0,IHxIW,KxK,IFM_numxKxK,LayerType,trow_loops);
			copy_input_weight(Input,Input1,Input,Input1,Weight,InFM_num,Input_w,Input_h,Kernel_size,Kernel_stride,TMP_R,TMP_C,TMP_M,TMP_M,
				TM_MIN,TM,TRow,TCol,0,input_buffer0,NULL,NOP,input_flag,0,0,IHxIW,KxK,IFM_numxKxK,LayerType,trow_loops);
			copy_input_weight(Input2,Input3,Input2,Input3,Weight,InFM_num,Input_w,Input_h,Kernel_size,Kernel_stride,TMP_R,TMP_C,TMP_M,TMP_M,
				TM_MIN,TM,TRow,TCol,0,input_buffer00,NULL,NOP,input_flag,0,0,IHxIW,KxK,IFM_numxKxK,LayerType,trow_loops);
			//pool_yolo2(input_buffer1,output_buffer,Kernel_size,Kernel_stride,TX_MIN_next[0],TR_MIN,TC_MIN,process_flag);
			//可以看出位数还是在计算函数里用到的，先看看那些都是什么含义然后看看他是如何利用的

			 //   void shortcut(short input_buffer[Tm][Tr][Tc],int output_buffer[Tm][Tr][Tc],
		     //       const int TM_MIN,const int TR_MIN,const int TC_MIN,bool enable)

			shortcut(input_buffer1,input_buffer10,output_buffer,TM_MIN,TR_MIN,TC_MIN,InterSubOutput,WeightAddInputSubInter,process_flag);
		}else
		{
			TMP_X_next[0] = tmp_x;
			TX_MIN_next[0] = tmp_tx_min;
			tmp_x = TMP_M;
			tmp_tx_min = TM_MIN;

			//copy_input_weight(Input,Input1,Input2,Input3,Weight,InFM_num,Input_w,Input_h,Kernel_size,Kernel_stride,TMP_R,TMP_C,TMP_M,TMP_M,
			//	TM_MIN,TM,TRow,TCol,0,input_buffer1,weight_buffer0,NOP,input_flag,0,0,IHxIW,KxK,IFM_numxKxK,LayerType,trow_loops);
			copy_input_weight(Input,Input1,Input,Input1,Weight,InFM_num,Input_w,Input_h,Kernel_size,Kernel_stride,TMP_R,TMP_C,TMP_M,TMP_M,
				TM_MIN,TM,TRow,TCol,0,input_buffer1,NULL,NOP,input_flag,0,0,IHxIW,KxK,IFM_numxKxK,LayerType,trow_loops);
			copy_input_weight(Input2,Input3,Input2,Input3,Weight,InFM_num,Input_w,Input_h,Kernel_size,Kernel_stride,TMP_R,TMP_C,TMP_M,TMP_M,
				TM_MIN,TM,TRow,TCol,0,input_buffer10,NULL,NOP,input_flag,0,0,IHxIW,KxK,IFM_numxKxK,LayerType,trow_loops);
			//pool_yolo2(input_buffer0,output_buffer,Kernel_size,Kernel_stride,TX_MIN_next[0],TR_MIN,TC_MIN,process_flag);
			shortcut(input_buffer0,input_buffer00,output_buffer,TM_MIN,TR_MIN,TC_MIN,InterSubOutput,WeightAddInputSubInter,process_flag);
		}

	}
	else
	if(LayerType==2)
	{
		//upsample是不需要做位数处理的
		if(pingpongx==0)
		{
			TMP_X_next[0] = tmp_x;
			TX_MIN_next[0] = tmp_tx_min;
			tmp_x = TMP_M;
			tmp_tx_min = TM_MIN;

			copy_input_weight(Input,Input1,Input2,Input3,Weight,InFM_num,Input_w,Input_h,Kernel_size,Kernel_stride,TMP_R,TMP_C,TMP_M,TMP_M,
				TM_MIN,TM,TRow,TCol,0,input_buffer0,NULL,NOP,input_flag,0,0,IHxIW,KxK,IFM_numxKxK,LayerType,trow_loops);
			//reorg_yolo2(input_buffer1,output_buffer,Kernel_size,Kernel_stride,TX_MIN_next[0],TR_MIN,TC_MIN,process_flag);
			upsample(input_buffer1,output_buffer,Kernel_stride,TM_MIN,TR_MIN,TC_MIN,process_flag);
		}else
		{
			TMP_X_next[0] = tmp_x;
			TX_MIN_next[0] = tmp_tx_min;
			tmp_x = TMP_M;
			tmp_tx_min = TM_MIN;

			copy_input_weight(Input,Input1,Input2,Input3,Weight,InFM_num,Input_w,Input_h,Kernel_size,Kernel_stride,TMP_R,TMP_C,TMP_M,TMP_M,
				TM_MIN,TM,TRow,TCol,0,input_buffer1,NULL,NOP,input_flag,0,0,IHxIW,KxK,IFM_numxKxK,LayerType,trow_loops);
			//reorg_yolo2(input_buffer0,output_buffer,Kernel_size,Kernel_stride,TX_MIN_next[0],TR_MIN,TC_MIN,process_flag);
			upsample(input_buffer0,output_buffer,Kernel_stride,TM_MIN,TR_MIN,TC_MIN,process_flag);
		}
		#ifdef UPSAMPLE_TEST
		    for(int i = 0; i < 4; i++){
			    for(int j = 0; j < 26; j ++){
			        for(int k = 0;k < 26;k++){
			            printf("%d,",output_buffer[i][j][k]);
			        }
			        printf("\n");
			    }
			    printf("\n");
			}
			printf("\n\n");
		#endif

	}

}

void copy_beta(short beta_buffer[MAX_BETA_LENGTH],int *Beta,const int OFM_NUM,const int BetaQ)
{
	static int beta_tmp[MAX_BETA_LENGTH/2];
	int NUM = (OFM_NUM+1)>>1;
	memcpy(beta_tmp,(int *)Beta,NUM*sizeof(int));
	int x;
	for(x = 0;x < NUM;x++)
	{
#pragma HLS PIPELINE
		beta_buffer[2*x] = beta_tmp[x];
		beta_buffer[2*x+1] = beta_tmp[x]>>16;
	}
}

/*
void YOLO2_FPGA(
int *Input,int *Input1,int *Input2,int *Input3,    //Input Address,Using paralized input,four ports
,int *Output,int *Output1, //two out Address paralized
int *Weight,int *Beta //weight and bias Address
const int InFM_num,const int OutFM_num, //Input size and out size
const int Kernel_size,const int Kernel_stride, //kernel size and stride
const int Input_w,const int Input_h,//Input winth and height
const int output_w,const int output_h, //Out width and height
const int Padding,//padding value
,const bool IsNL,const bool IsBN, //is leaky ReLu,is batch normalization
const int TM,const int TN,const int TR,const int TC, //accelerate configuration,TM,TN,TR,TC,is setted,but i do not know why set like this
const int mLoops,const int nLoops,const int rLoops,const int cLoops,//do not know
const int LayerType,//layertype ,defferent layertype do different operate
const int InputQ,const int OutputQ,const int WeightQ,const int BetaQ,//quanti weishu
int trow_loops//do not know,seems not used
){

}
*/

/*
就差shortcut和upsample两层位数对齐了
upsample不需要对其
也就是只差shortcut对其了
*/
//Maybe I need to change the name?
void YOLO2_FPGA(int *Input,int *Input1,int *Input2,int *Input3,int *Output,int *Output1,int *Weight,int *Beta,const int InFM_num,const int OutFM_num,
							  const int Kernel_size,const int Kernel_stride,
							  const int Input_w,const int Input_h,const int output_w,const int output_h,const int Padding,const bool IsNL,const bool IsBN,
							  const int TM,const int TN,const int TR,const int TC,
							  const int mLoops,const int nLoops,const int rLoops,const int cLoops,const int LayerType,
							  const int InputQ,const int OutputQ,const int WeightQ,const int BetaQ,int trow_loops)
{

#pragma HLS INTERFACE m_axi depth=512 port=Input   offset=slave bundle=DATA_BUS1 num_read_outstanding=1 num_write_outstanding=1 max_read_burst_length=64 max_write_burst_length=64
#pragma HLS INTERFACE m_axi depth=512 port=Input1  offset=slave bundle=DATA_BUS2 num_read_outstanding=1 num_write_outstanding=1 max_read_burst_length=64 max_write_burst_length=64
#pragma HLS INTERFACE m_axi depth=512 port=Input2  offset=slave bundle=DATA_BUS3 num_read_outstanding=1 max_read_burst_length=64
#pragma HLS INTERFACE m_axi depth=512 port=Input3  offset=slave bundle=DATA_BUS4 num_read_outstanding=1 max_read_burst_length=64
#pragma HLS INTERFACE m_axi depth=512 port=Output  offset=slave bundle=DATA_BUS1 num_read_outstanding=1 num_write_outstanding=1 max_read_burst_length=64 max_write_burst_length=64
#pragma HLS INTERFACE m_axi depth=512 port=Output1 offset=slave bundle=DATA_BUS2 num_read_outstanding=1 num_write_outstanding=1 max_read_burst_length=64 max_write_burst_length=64
#pragma HLS INTERFACE m_axi depth=512 port=Weight  offset=slave bundle=DATA_BUS5 num_read_outstanding=1 max_read_burst_length=128
#pragma HLS INTERFACE m_axi depth=512 port=Beta    offset=slave bundle=DATA_BUS5 num_read_outstanding=1 max_read_burst_length=128

#pragma HLS INTERFACE s_axilite register port=return bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=InFM_num bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=OutFM_num bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=Kernel_size bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=Kernel_stride bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=Input_w bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=Input_h bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=output_w bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=output_h bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=Padding bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=IsNL bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=IsBN bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=TM bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=TN bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=TR bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=TC bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=mLoops bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=nLoops bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=rLoops bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=cLoops bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=LayerType bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=InputQ bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=OutputQ bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=WeightQ bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=BetaQ bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=trow_loops bundle=CTRL_BUS

#pragma HLS INTERFACE s_axilite register port=Input bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=Output bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=Weight bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite register port=Beta bundle=CTRL_BUS

	assert(Kernel_size < 4);
	assert(Kernel_stride < 4);
	assert(TR < 32);
	assert(TC < 32);
	assert(InFM_num < 2048);
	assert(OutFM_num < 2048);
	assert(output_h < 512);
	assert(output_w < 512);
	assert(Input_h < 1024);//gg///??????
	assert(Input_w < 512);

	assert(WeightQ < 32);
	assert(InputQ < 32);
	assert(OutputQ < 32);
	assert(BetaQ < 32);

	ap_uint<9> output_h_9b = output_h;
	ap_uint<9> output_w_9b = output_w;
	ap_uint<5> TR_5b = TR;
	ap_uint<5> TC_5b = TC;
	ap_uint<2> Kernel_stride_2b = Kernel_stride;
	ap_uint<2> Kernel_size_2b = Kernel_size;
	ap_uint<11> InFM_num_11b = InFM_num;
	ap_uint<10> Input_h_10b = Input_h;
	ap_uint<9> Input_w_9b = Input_w;
	ap_uint<6> trow_loops_6b = trow_loops;


	UCHAR tmprow,tmpcol;
	if(LayerType==2){
	    tmprow = TR_5b;
	    tmpcol = TC_5b;
	} else {
	    tmprow = (TR_5b-1)*Kernel_stride_2b+Kernel_size_2b;
	    tmpcol = (TC_5b-1)*Kernel_stride_2b+Kernel_size_2b;
	}
	const UCHAR TRow = tmprow;
	const UCHAR TCol = tmpcol;


//	const UCHAR TRow = (TR_5b-1)*Kernel_stride_2b+Kernel_size_2b;
//	const UCHAR TCol = (TC_5b-1)*Kernel_stride_2b+Kernel_size_2b;
	
	
	const int OHxOW = output_h_9b*output_w_9b;

	const int IHxIW   = Input_h_10b*Input_w_9b;
	const int KxK = Kernel_size_2b*Kernel_size_2b;
	assert(KxK < 10);
	ap_uint<4> KxK_4b = KxK;
	const int IFM_numxKxK = InFM_num_11b*KxK_4b;
	const int mLoops_add1 = mLoops + 1;
	const int mLoops_add2 = mLoops + 2;
	const int mLoops_bound = LayerType ? mLoops_add2: mLoops_add1;
	//temp_inputQ[l.index+1],	temp_inputQ[i],		INTERWIDTH,			INTERWIDTH,			trow_loops
	//const int InputQ,			const int OutputQ,	const int WeightQ,	const int BetaQ,	int trow_loops
	const UCHAR InterSubBeta = INTERWIDTH - BetaQ;//总的与bias值的偏移量
	const UCHAR WeightAddInputSubInter = WeightQ + InputQ - INTERWIDTH;//输入与权重相乘后大于差值的偏移量
	const UCHAR InterSubOutput = INTERWIDTH - OutputQ;//超过输出的偏移量，虽然处理与写回看起来都用到了，但是实际上只有写回能用到
	/*
在shortcut里面   	InterSubBeta = INTERWIDTH-INTERWIDTH = 	0；
					WeightAddInputSubInter = 				temp_inputQ[l.index+1]；//这个可以直接利用起来，用INTERWIDTH-它  定义成一个值
					InterSubOutput		   = 				INTERWIDTH-temp_inputQ[i];；直接利用
			那么这样子就是两者距离20位的差距，都左移，按放大处理，加法以后再处理成输出值？？？//需要在这里输出吗

在conv内			InterSubBeta > 0 
					WeightAddInputSubInter 较小
					InterSubOutput 与上相等
	*/
	

	//assert(InterSubBeta < 16);
	//assert(WeightAddInputSubInter < 16);
	//assert(InterSubOutput < 16);
	//printf("LayerType = %d\n",LayerType);

	//assert(TRow < 256);
	//assert(TCol < 256);

	//printf("OKOKOK!\n");

	static short input_buffer0[Tn][OnChipIB_Height][OnChipIB_Width];
#pragma HLS ARRAY_PARTITION variable=input_buffer0 complete dim=1

	static short input_buffer1[Tn][OnChipIB_Height][OnChipIB_Width];
#pragma HLS ARRAY_PARTITION variable=input_buffer1 complete dim=1

//mycode
	static short input_buffer00[Tn][OnChipIB_Height][OnChipIB_Width];
#pragma HLS ARRAY_PARTITION variable=input_buffer0 complete dim=1

	static short input_buffer10[Tn][OnChipIB_Height][OnChipIB_Width];
#pragma HLS ARRAY_PARTITION variable=input_buffer1 complete dim=1
//end
	static int output_buffer0[Tm][Tr*PARA][Tc*PARA];
#pragma HLS ARRAY_PARTITION variable=output_buffer0 complete dim=1

	static int output_buffer1[Tm][Tr*PARA][Tc*PARA];
#pragma HLS ARRAY_PARTITION variable=output_buffer1 complete dim=1

	static short beta_buffer[MAX_BETA_LENGTH];

	int r,c,m;
/////////////////////////////////param
	int TMP_R,TMP_C,TMP_M;
	int TM_MIN,TR_MIN,TC_MIN;
///////////////////////////////////////

	int TMP_M_next0[1];
	int TMP_M_next1[1];
	int TM_MIN_next0[1];
	int TM_MIN_next1[1];
	bool pingpongm;
	const int para = (LayerType==2) ? 2 : 1;

	if(LayerType==0)
		copy_beta(beta_buffer,Beta,OutFM_num,BetaQ);

	for(TMP_R = 0,r = 0; r < rLoops; r++, TMP_R += TR)
	{
		TR_MIN = MIN(TR,output_h -TMP_R);
		for(TMP_C = 0,c = 0; c < cLoops; c++,TMP_C += TC)
		{
			TC_MIN = MIN(TC,output_w -TMP_C);
			pingpongm = 0;
			for(TMP_M = 0, m = 0; m < mLoops_bound; m++,TMP_M += TM)
			{
				TM_MIN = MIN(TM,OutFM_num-TMP_M);
				//if(LayerType!=0) TM_MIN = Tn;
				//printf("OK\n");
				bool MneZero = (m!=0);
				bool MneOne = (m!=1);
				bool MnemLoops = (m!=mLoops);
				bool MneMLoopsaddOne = (m!=mLoops_add1);
				bool input_flag = LayerType ? MnemLoops&&MneMLoopsaddOne: MnemLoops;
				bool process_flag = LayerType ? MneZero&&MneMLoopsaddOne : MnemLoops;
				bool write_flag = LayerType ? MneZero&&MneOne : MneZero;

				if(pingpongm==0)
				{
					intra_pingpong_wrapper(Input,Input1,Input2,Input3,Weight,output_buffer1,beta_buffer,input_buffer0,input_buffer1,input_buffer00,input_buffer10,
									InFM_num, Input_w, Input_h, Kernel_size, Kernel_stride,
									TMP_R, TMP_C, TMP_M, m, TM_MIN, TR_MIN, TC_MIN, TN, TRow, TCol, Padding,IHxIW,KxK,IFM_numxKxK,nLoops,IsNL,LayerType,TM, TMP_M_next1,TM_MIN_next1, pingpongm, input_flag,
									process_flag,InterSubBeta,WeightAddInputSubInter,InterSubOutput,trow_loops_6b);

					write_back_output_reorg(output_buffer0,Output,Output1,TMP_R,TMP_C,TMP_M_next0[0],output_w,output_h,TM_MIN_next0[0],TR_MIN*para,TC_MIN*para,OHxOW,write_flag,OutputQ, IsNL, InterSubOutput, LayerType);
					pingpongm = 1;
				}else
				{
					intra_pingpong_wrapper(Input,Input1,Input2,Input3,Weight,output_buffer0,beta_buffer,input_buffer0,input_buffer1,input_buffer00,input_buffer10,
									InFM_num, Input_w, Input_h, Kernel_size, Kernel_stride,
									TMP_R, TMP_C, TMP_M, m, TM_MIN, TR_MIN, TC_MIN, TN, TRow, TCol, Padding,IHxIW,KxK,IFM_numxKxK,nLoops,IsNL,LayerType,TM, TMP_M_next0,TM_MIN_next0, pingpongm, input_flag,
									process_flag,InterSubBeta,WeightAddInputSubInter,InterSubOutput,trow_loops_6b);

					write_back_output_reorg(output_buffer1,Output,Output1,TMP_R,TMP_C,TMP_M_next1[0],output_w,output_h,TM_MIN_next1[0],TR_MIN*para,TC_MIN*para,OHxOW,write_flag,OutputQ, IsNL, InterSubOutput, LayerType);
					pingpongm = 0;
				}
/*
				if(write_flag){
					printf("write!\n");
				} else {
					printf("NOT\n");
				}
*/
			}
		}
	}
}
////////////////////////////////////////////20181229 n4m32  v2 without input and reorg opt end input opt ok relu comb ok // input opt ok //output opt ok //weight opt ok (5)n4m32i4o2 ok end
