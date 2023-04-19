///////////////////////////////////////////////////////////////////////20181229 anti-reorg start => KxKxTmxTn
#define MAX(x,y) ((x)>(y)?(x):(y))
#define MIN(x,y) ((x)<(y)?(x):(y))
#define S 2
#define K 3

#define HALFWID 208
#define ATOMWID 13
#define BLOCK 512

#define Tn 4
#define Tm 32
#define Tr 26
//#define Tr 38
#define Tc 26
//#define Tc 38
#define OnChipIB_Width  ((Tc-1)*S+K)
#define OnChipIB_Height ((Tr-1)*S+K)
#define MAX_BETA_LENGTH (1024)
#define PARA 1

#define REORG_GEN
//#define REORG_TEST

//#define UPSAMPLE_TEST

/*
关于TR与 ONCHIPTR
显然TR< ONCHIPTR 这很显然，那是因为yolo2中所有的都是减小size的操作而没有upsmp
但是yolo3是有的，例如upsmp就是增加size，那么对于upsmp而言，
需要设置的输出数组大小要做一些改变，应该是输入size的2x2倍
但是我又不是很想改动size大小，因为要兼容其他的代码，那么只能考虑把out填充满，而inpt以及
循环控制要做修改

具体有关shorcut 这一步应该是可以完成的，因为有很多size不变的操作，他是要做一个统一的，那么在我这里
shortcut除了是做加法，size应该也是可复用，无非就是输入的加载需要一定的设置。

*/

//////////////////////////////////////////////////T3 start

/*
float *input,float input_buffer[Tn][OnChipIB_Height][OnChipIB_Width]  总的输入数组，要存放的新数组，
int r,int c,int n,当前运行到的行，列，页的位置块标记
int Kernel_stride,int Padding, 卷积核步长，是否padding
int TRow,int TCol,是否padding计算后的新的行列值
int Input_w,int Input_h,输入宽高
int TN_MIN,int IHxIW,输入的页长度，一页的总大小
int LayerType 类型，决定是否设置最小负值，但是在v3里无用

LOOP1：将当前的输入从二维化的扁平到一维化
LOOP2: 从第一个开始对每个位置遍历，对要存放的新数组的位置，如果不处于padding位置就赋值，否则置成padding值
maxpool为了不影响结果，padding设置为最小的负值


也就是说，这个函数加载（Tn * TRow * TCol）的输入值进入数组里
*/
void input_load(float *input,float input_buffer[Tn][OnChipIB_Height][OnChipIB_Width],int r,int c,int n,int Kernel_stride,int Padding,int TRow,int TCol,int Input_w,int Input_h,int TN_MIN,int IHxIW,int LayerType)
{
	int t1,t2,t3,t4;
	int xoffset;
	int yoffset;

	static float input_memcpy_buffer[Tn*OnChipIB_Height*OnChipIB_Width];

    int tempcoff,temproff,tempcurr;
    if(LayerType == 2){
        tempcoff = c;
        temproff = r;
        //tempcurr = 
    } else {
        tempcoff = c*Kernel_stride - Padding;
        temproff = r*Kernel_stride - Padding;
    }

	//const int Coffset = c*Kernel_stride - Padding;
	//const int Roffset = r*Kernel_stride - Padding;
	const int Coffset = tempcoff;
	const int Roffset = temproff;
	const int CurrentOffset = n*IHxIW + Roffset*Input_w + Coffset;
	//虽然说设置了是这么大的，但是可以不加载满
	//printf("TN_MIN*TRow:%d x %d = %d\n",TN_MIN,TRow,TN_MIN*TRow);

	float pad_value = 0;
	
	#ifdef UPSAMPLE_TEST
	if(LayerType==2){
		//pad_value = -1024*1024;
		printf("r = %d,c = %d,n = %d,Coffset = %d,Roffset = %d,CurrentOffset = %d,TRow = %d\n",r,c,n,Coffset,Roffset,CurrentOffset,TRow);
	}
	#endif
	
	int input_mmcpy_offset = 0;
	for(t1 = 0;t1 < TN_MIN; t1++)
		for(t2 = 0;t2 < TRow; t2++)
		{
			memcpy((float *)(input_memcpy_buffer + input_mmcpy_offset),(float *)(input + CurrentOffset + t1*IHxIW + t2*Input_w),TCol*sizeof(float));
			input_mmcpy_offset += TCol;
		}

	input_mmcpy_offset = 0;
	for(t1 = 0;t1 < Tn; t1++)
		for(t2 = 0;t2 < TRow; t2++)
			for(t3 = 0;t3 < TCol; t3++)
			{
				xoffset = Coffset + t3;
				yoffset = Roffset + t2;
				bool XEnable    = (xoffset >= 0)&&(xoffset < Input_w);
				bool YEnable    = (yoffset >= 0)&&(yoffset < Input_h);
				bool PaddingEnable = XEnable&&YEnable;
				if(PaddingEnable&&(t1 < TN_MIN))
					input_buffer[t1][t2][t3] = input_memcpy_buffer[input_mmcpy_offset];
				else
					input_buffer[t1][t2][t3] = pad_value;
				input_mmcpy_offset++;
			}
}

void weight_load(float *Weight,float weight_buffer[Tm][Tn][K][K],bool weight_load_enable,int m,int n,int IFM_numxKxK,int KxK,int Kernel_size,int TM_MIN,int TN_MIN)
{
	int t1,t2,t3,t4;
	static float weight_memcpy_buffer[Tm*Tn*K*K];
	
	if(!weight_load_enable)
		return;
	
	const int Woffset = m*IFM_numxKxK + n*KxK;
		
	int weight_memcpy_offset = 0;
	for(t1 = 0;t1 < TM_MIN; t1++)
		for(t2 = 0;t2 < TN_MIN; t2++)
		{
			memcpy((float *)(weight_memcpy_buffer + weight_memcpy_offset),(float *)(Weight + Woffset + t1*IFM_numxKxK + t2*KxK),KxK*sizeof(float));
			weight_memcpy_offset += KxK;
		}
	
	weight_memcpy_offset = 0;
	for(t1 = 0;t1 < Tm; t1++)
		for(t2 = 0;t2 < Tn; t2++)
			for(t3 = 0;t3 <Kernel_size; t3++)
				for(t4 = 0;t4 <Kernel_size; t4++)
				{
					bool Enable = (t1 < TM_MIN)&&(t2 < TN_MIN);
					if(Enable)
					{
						weight_buffer[t1][t2][t3][t4] =  weight_memcpy_buffer[weight_memcpy_offset];
						weight_memcpy_offset++;
					}
					else
						weight_buffer[t1][t2][t3][t4] = 0;
				}
}

void weight_load_reorg(float *Weight,float weight_buffer[Tm][Tn][K][K],bool weight_load_enable,int m,int n,int IFM_numxKxK,int KxK,int Kernel_size,int TM_MIN,int TN_MIN)
{
	int t1,t2,t3,t4;
	static float weight_memcpy_buffer[Tm*Tn*K*K];
	static int Woffset;

	if(!weight_load_enable)
		return;

	if(m==0&&n==0)
		Woffset = 0;

	memcpy(weight_memcpy_buffer,(float *)(Weight + Woffset),TM_MIN*TN_MIN*KxK*sizeof(float));
	Woffset += TM_MIN*TN_MIN*KxK;
	
	int weight_memcpy_offset = 0;
	for(t3 = 0;t3 <Kernel_size; t3++)
		for(t4 = 0;t4 <Kernel_size; t4++)
			for(t1 = 0;t1 < Tm; t1++)
				for(t2 = 0;t2 < Tn; t2++)
				{
					bool Enable = (t1 < TM_MIN)&&(t2 < TN_MIN);
					if(Enable)
					{
						weight_buffer[t1][t2][t3][t4] =  weight_memcpy_buffer[weight_memcpy_offset];
						weight_memcpy_offset++;
					}
					else
						weight_buffer[t1][t2][t3][t4] = 0;
				}	
}


/*
Kernel_stride, Padding, TRow, TCol, Input_w, Input_h, TN_MIN, IHxIW, LayerType 在某个层内一直是常数
bool initialize 无效参数

float *input,float *Weight, 输入和权重的地址
int InFM_num, 输入通道的总数
int Input_w,int Input_h, 输入宽高
int OutFM_num, 输出通道的总数
int Kernel_size,int Kernel_stride, 卷积核大小步长
int r,int c,int m,int n, 当前已经加载了的行列，输出页数，输入页数
int TM_MIN,
int TN,
int TRow,int TCol,int Padding, 是否进行padding计算后的新的行列值，是否padding
float input_buffer[Tn][OnChipIB_Height][OnChipIB_Width],float weight_buffer[Tm][Tn][K][K], 输入buff，权重buff
,int TMP_N_next[1],
bool enable,bool weight_load_enable,bool initialize,使能端，是否加载权重，无效参数
const int IHxIW,const int KxK,一页的总大小，卷积核大小
const int IFM_numxKxK,
const int LayerType 层类型

判断是否需要加载weight，分别加载input和weight，对非conv层来说，＝ input_load
*/
void copy_input_weight(float *input,float *Weight,int InFM_num,int Input_w,int Input_h,int OutFM_num,int Kernel_size,int Kernel_stride,int r,int c,int m,int n,
		int TM_MIN,int TN,int TRow,int TCol,int Padding,float input_buffer[Tn][OnChipIB_Height][OnChipIB_Width],float weight_buffer[Tm][Tn][K][K],int TMP_N_next[1],
		bool enable,bool weight_load_enable,bool initialize,const int IHxIW,const int KxK,const int IFM_numxKxK,const int LayerType)
{
	if(!enable)
		return ;

	const int TN_MIN = MIN(TN,InFM_num - n);
	TMP_N_next[0] = n;
	//if(LayerType == 2) printf("TN_MIN = %d,TRow = %d,TCol = %d\n",TN_MIN,TRow,TCol);

	input_load(input, input_buffer, r, c, n, Kernel_stride, Padding, TRow, TCol, Input_w, Input_h, TN_MIN, IHxIW, LayerType);
#ifdef REORG_TEST
	weight_load_reorg(Weight,weight_buffer,weight_load_enable,m,n,IFM_numxKxK,KxK,Kernel_size,TM_MIN,TN_MIN);
#else
	weight_load(Weight,weight_buffer,weight_load_enable,m,n,IFM_numxKxK,KxK,Kernel_size,TM_MIN,TN_MIN);
#endif

}
//////////////////////////////////////////////////T3 end
	void copy_local_beta(float beta_buffer[MAX_BETA_LENGTH],float local_beta_buffer[MAX_BETA_LENGTH],const int TM_MIN,int m)
{
	//memcpy(local_beta_buffer,(float *)(beta_buffer+m),TM_MIN*sizeof(float));
	int offset;
	int tm;
	for(tm = 0,offset = m;tm < TM_MIN;tm++)
	{
		local_beta_buffer[tm] = beta_buffer[offset];
		offset++;
	}
}

void nonlinear_leaky(float Input[Tm][Tr*PARA][Tc*PARA],const int TM_MIN,const int TR_MIN,const int TC_MIN,const bool IsNL)
{
	int tr,tc,tm;

	if(!IsNL)
		return ;
	
	for(tm = 0;tm < TM_MIN;tm++)
#pragma HLS LOOP_TRIPCOUNT min=1 max=1
		for(tr = 0;tr < TR_MIN;tr++)
#pragma HLS LOOP_TRIPCOUNT min=1 max=14
			for(tc = 0;tc < TC_MIN;tc++)
			{
#pragma HLS LOOP_TRIPCOUNT min=14 max=14
#pragma HLS PIPELINE
				float tmp = Input[tm][tr][tc];
				if(tmp < 0)
					Input[tm][tr][tc] = tmp*0.1;
			}

}

///#######################################################################################################################
void shortcut(float input_buffer0[Tn][OnChipIB_Height][OnChipIB_Width],float input_buffer1[Tn][OnChipIB_Height][OnChipIB_Width],
        float output_buffer[Tm][Tr*PARA][Tc*PARA],const int TM_MIN,const int TR_MIN,
        const int TC_MIN,bool enable){
    int tr,tc,tm;
	//printf("TM_MIN=%d,TR_MIN=%d,TC_MIN=%d\n",TM_MIN,TR_MIN,TC_MIN);
	//if(enable) printf("yes\n");

    if(!enable){
        //printf("No!\n");
		return;
    }
    //printf("Tn=%d,OnChipIB_Height=%d,OnChipIB_Width=%d,TM_MIN=%d,TR_MIN=%d,TC_MIN=%d\n",Tn,OnChipIB_Height,OnChipIB_Width,TM_MIN,TR_MIN,TC_MIN);
    //assert(TR_MIN < 32);
	//assert(TC_MIN < 32);
	
    for(tm = 0; tm < TM_MIN;tm++){
        for(tr = 0; tr < TR_MIN;tr++){
            for(tc = 0; tc < TC_MIN; tc++){
//#pragma HLS PIPELINE
                output_buffer[tm][tr][tc] = input_buffer0[tm][tr][tc] + input_buffer1[tm][tr][tc];
            }
        }
    }
    //printf("tm=%d,tr=%d,tc=%d\n",tm,tr,tc);
    //printf("output_buffer[tm][tr][tc] = input_buffer0[tm][tr][tc] + input_buffer1[tm][tr][tc] is %f = %f + %f\n",output_buffer[tm][tr][tc],input_buffer0[tm][tr][tc],input_buffer1[tm][tr][tc]);
    //printf("shortcut OK 3\n");
}
///#######################################################################################################################
//有问题，97层的太大了，弄出来不对劲，因为Tr,Tc只有26
void upsample(float input_bufferInput[Tn][OnChipIB_Height][OnChipIB_Width],float output_buffer[Tm][Tr*PARA][Tc*PARA],
        const int upsample_size,const int TM_MIN,const int TR_MIN,
        const int TC_MIN,bool enable){
    int tr,tc,tm,i,j;
	//printf("TM_MIN=%d,TR_MIN=%d,TC_MIN=%d\n",TM_MIN,TR_MIN,TC_MIN);
	//if(enable) printf("yes\n");

    if(!enable){
        //printf("No!\n");
		return;
    }

    //assert(TR_MIN < 32);
	//assert(TC_MIN < 32);
	
    for(tm = 0; tm < TM_MIN;tm++){
        for(tr = 0; tr < TR_MIN;tr++){
            for(tc = 0; tc < TC_MIN; tc++){
//#pragma HLS PIPELINE
                for(i = 0; i < upsample_size;i++){
#pragma HLS UNROLL
                    for(j = 0;j < upsample_size;j++){
#pragma HLS UNROLL
                            output_buffer[tm][tr*upsample_size+i][tc*upsample_size+j] = input_bufferInput[tm][tr][tc];
							//printf("page = %d,x = %d,y = %d,value = %f\n",tm,tr*upsample_size+i,tc*upsample_size+j,input_bufferInput[tm][tr][tc]);
                        }
                }
            }
        }
    }
    //printf("upsample OK 4\n");
}

///#######################################################################################################################

void compute(float input_buffer[Tn][OnChipIB_Height][OnChipIB_Width],float output_buffer[Tm][Tr*PARA][Tc*PARA],
		float weight_buffer[Tm][Tn][K][K],float beta_buffer[MAX_BETA_LENGTH],int TMP_N_next[1],
		const int Kernel_size,const int Kernel_stride,int TMP_M,
		const int TM_MIN,const int TR_MIN,const int TC_MIN,bool enable,const bool IsNL,const bool reluenable)
{
	static float local_beta_buffer[Tm];
#pragma HLS ARRAY_PARTITION variable=local_beta_buffer complete dim=1

	if(!enable)
	{
		copy_local_beta(beta_buffer,local_beta_buffer,TM_MIN,TMP_M);
		return;
	}
	//printf("compute OK 5\n");
	//printf("TM_MIN=%d,TR_MIN=%d,TC_MIN=%d\n",TM_MIN,TR_MIN,TC_MIN);

	int i,j,tr,tc,tm,tn;
	int n = TMP_N_next[0];
	float partial_mul[Tm][Tn];
	float partial_add[Tm];

	for(i =0;i < Kernel_size; i++)
#pragma HLS LOOP_TRIPCOUNT min=1 max=5
		for(j = 0;j < Kernel_size; j++)
#pragma HLS LOOP_TRIPCOUNT min=1 max=5
			for(tr = 0;tr < TR_MIN;tr++)
#pragma HLS LOOP_TRIPCOUNT min=14 max=14
				for(tc = 0;tc < TC_MIN;tc++)
				{
#pragma HLS LOOP_TRIPCOUNT min=14 max=14
#pragma HLS PIPELINE
					for(tm = 0;tm < Tm;tm++)
					{
						if(i==0&&j==0&&n==0)
							partial_add[tm] = local_beta_buffer[tm];
						else
							partial_add[tm] = output_buffer[tm][tr][tc];
					}

					for(tm = 0;tm < Tm;tm++)
						for(tn = 0;tn <Tn;tn++)
						{
							partial_mul[tm][tn] = weight_buffer[tm][tn][i][j]*input_buffer[tn][Kernel_stride*tr+i][Kernel_stride*tc+j];
						}

					
					for(tm = 0;tm < Tm;tm++)
					{
						float partial_sum = 0;
						for(tn = 0;tn <Tn;tn++)
						{
							 partial_sum += partial_mul[tm][tn];
						}
						output_buffer[tm][tr][tc] = partial_add[tm] + partial_sum;
					}
				}

	if(reluenable)
		nonlinear_leaky(output_buffer,TM_MIN,TR_MIN,TC_MIN,IsNL);

}
/*
float output_buffer[Tm][Tr][Tc],处理好的三维数组
float *Output,要存放的线性数组
int r,int c,int m,在这次输入的三维数组之前已经有多少页了，之前有多少行了，之前多少列
const int Output_w,const int Output_h,输出的宽高

 const int TM_MIN,这次输入多少页
 const int TR_MIN,本次输入多少行
 const int TC_MIN,本次输入多少列
 const int OHxOW,输出的页面大小
 bool write_flag是否写入
*/
void write_back_output_reorg(float output_buffer[Tm][Tr*PARA][Tc*PARA],float *Output,int r,int c,int m,const int Output_w,const int Output_h,
					   const int TM_MIN,const int TR_MIN,const int TC_MIN,const int OHxOW,const int LayerType,bool write_flag)
{
	if(!write_flag)
		return;

	int tempoff;
	if(LayerType == 2){
	    tempoff = m*OHxOW + r*Output_w*2 + c*2;
	} else {
        tempoff = m*OHxOW + r*Output_w + c;
	}
	const int offset = tempoff;
	int tr,tm,tc;
	
	const int para = (LayerType==2) ? 2:1;
	//if(LayerType==2)	    printf("r = %d,c = %d,m = %d,TM=%d,TR=%d,TC=%d,Out_w=%d,OWOH=%d,offset=%d\n",r,c,m,TM_MIN,TR_MIN,TC_MIN,Output_w,OHxOW,offset);
	//if(LayerType==2) 	printf("TM=%d,TR=%d,TC=%d,Out_w=%d,OWOH=%d,offset=%d\n",TM_MIN,TR_MIN,TC_MIN,Output_w,OHxOW,offset);
	for(tm = 0;tm < TM_MIN;tm++)
		for(tr = 0;tr < TR_MIN*para;tr++)
			for(tc = 0;tc < TC_MIN*para;tc++)
			{
					Output[tm*OHxOW + tr*Output_w + tc + offset] = output_buffer[tm][tr][tc];
					if(LayerType==2){
						//printf("index = %d,value = %f\n",tm*OHxOW + tr*Output_w + tc + offset,output_buffer[tm][tr][tc]);
					}
			}
	//if(LayerType==2) 	printf("\n\n");

/*
	for(tm = 0;tm < TM_MIN;tm++)
		for(tr = 0;tr < TR_MIN*para;tr++)
		{
			memcpy((float *)(Output + tm*OHxOW + tr*Output_w + offset),output_buffer[tm][tr],TC_MIN*para*sizeof(float));
		}
*/
}
/*
void pool_yolo2(float Input[Tn][OnChipIB_Height][OnChipIB_Width],float Output[Tm][Tr][Tc],
		  const int Kernel_size,const int Kernel_stride,
		  const int TM_MIN,const int TR_MIN,const int TC_MIN,bool enable)
{
	if(!enable)
		return;

	int i,j,tr,tc,of;
	float tmp[Tn];

	for(tr = 0;tr < TR_MIN;tr++)
		for(tc = 0;tc < TC_MIN;tc++)
			for(i =0;i < Kernel_size; i++)
				for(j = 0;j < Kernel_size; j++)
				{
#pragma HLS PIPELINE
					for( of = 0; of < Tn; of++)
					{
						if(i==0&&j==0)
							tmp[of] = -1024*1024;

						if(Input[of][tr*Kernel_stride+i][tc*Kernel_stride+j] > tmp[of])
							tmp[of] = Input[of][tr*Kernel_stride+i][tc*Kernel_stride+j];

						if(i==1&&j==1)
							Output[of][tr][tc] = tmp[of];
					}
				}

}

void reorg_yolo2(float Input[Tn][OnChipIB_Height][OnChipIB_Width],float Output[Tm][Tr][Tc],
		  const int Kernel_size,const int Kernel_stride,
		  const int TM_MIN,const int TR_MIN,const int TC_MIN,bool enable)
{
	int x, y,kx,ky;
	unsigned char Yoffset;
	unsigned char Xoffset;

	if(!enable)
		return;

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
void intra_pingpong_wrapper(float *Input0,float *Input1,float *Weight, float output_buffer[Tm][Tr*PARA][Tc*PARA],float beta_buffer[MAX_BETA_LENGTH],
								 float input_buffer0[Tn][OnChipIB_Height][OnChipIB_Width],float input_buffer1[Tn][OnChipIB_Height][OnChipIB_Width],
								 float input_buffer00[Tn][OnChipIB_Height][OnChipIB_Width],float input_buffer10[Tn][OnChipIB_Height][OnChipIB_Width],
								 int InFM_num,int Input_w,int Input_h,int OutFM_num,int Kernel_size,int Kernel_stride,
								 int TMP_R,int TMP_C,int TMP_M,int m,int TM_MIN,int TR_MIN,int TC_MIN,int TN,int TRow,int TCol,int Padding,
								 int IHxIW,int KxK,int IFM_numxKxK,int nLoops,bool IsNL,int LayerType,int TM,int TMP_X_next[1],int TX_MIN_next[1],bool pingpongx,bool input_flag,bool process_flag)
{
	static float weight_buffer0[Tm][Tn][K][K];
#pragma HLS ARRAY_PARTITION variable=weight_buffer0 complete dim=1
#pragma HLS ARRAY_PARTITION variable=weight_buffer0 complete dim=2

	static float weight_buffer1[Tm][Tn][K][K];
#pragma HLS ARRAY_PARTITION variable=weight_buffer1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=weight_buffer1 complete dim=2

	static int NOP[1];
	static int tmp_x;
	static int tmp_tx_min;
	//printf("TM_MIN=%d,TR_MIN=%d,TC_MIN=%d\n",TM_MIN,TR_MIN,TC_MIN);
	//printf("intra OK 2\n");
	if(LayerType==0)
	{

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
				copy_input_weight(Input0,Weight,InFM_num,Input_w,Input_h,OutFM_num,Kernel_size,Kernel_stride,TMP_R,TMP_C,TMP_M,TMP_N,
					TM_MIN,TN,TRow,TCol,Padding,input_buffer1,weight_buffer1,TMP_N_next1,n!=nLoops,1,(m==0)&&(n==0),IHxIW,KxK,IFM_numxKxK,LayerType);
				compute(input_buffer0,output_buffer,weight_buffer0,beta_buffer,TMP_N_next0,Kernel_size,Kernel_stride,TMP_M,TM_MIN,TR_MIN,TC_MIN,n!=0,IsNL,n==nLoops);
				pingpong = 0;
			}else
			{
				copy_input_weight(Input0,Weight,InFM_num,Input_w,Input_h,OutFM_num,Kernel_size,Kernel_stride,TMP_R,TMP_C,TMP_M,TMP_N,
					TM_MIN,TN,TRow,TCol,Padding,input_buffer0,weight_buffer0,TMP_N_next0,n!=nLoops,1,(m==0)&&(n==0),IHxIW,KxK,IFM_numxKxK,LayerType);
				compute(input_buffer1,output_buffer,weight_buffer1,beta_buffer,TMP_N_next1,Kernel_size,Kernel_stride,TMP_M,TM_MIN,TR_MIN,TC_MIN,n!=0,IsNL,n==nLoops);
				pingpong = 1;
			}
		}
	}
	else if(LayerType==1)
	{
		if(pingpongx==0)
		{
			TMP_X_next[0] = tmp_x;
			TX_MIN_next[0] = tmp_tx_min;
			tmp_x = TMP_M;
			tmp_tx_min = TM_MIN;

			//copy_input_weight(Input,Weight,InFM_num,Input_w,Input_h,OutFM_num,Kernel_size,Kernel_stride,TMP_R,TMP_C,TMP_M,TMP_M,
			//	TM_MIN,TM,TRow,TCol,0,input_buffer0,NULL,NOP,input_flag,0,0,IHxIW,KxK,IFM_numxKxK,LayerType);
			copy_input_weight(Input0,Weight,InFM_num,Input_w,Input_h,OutFM_num,Kernel_size,Kernel_stride,TMP_R,TMP_C,TMP_M,TMP_M,
				TM_MIN,TM,TRow,TCol,0,input_buffer0,NULL,NOP,input_flag,0,0,IHxIW,KxK,IFM_numxKxK,LayerType);
			copy_input_weight(Input1,Weight,InFM_num,Input_w,Input_h,OutFM_num,Kernel_size,Kernel_stride,TMP_R,TMP_C,TMP_M,TMP_M,
				TM_MIN,TM,TRow,TCol,0,input_buffer00,NULL,NOP,input_flag,0,0,IHxIW,KxK,IFM_numxKxK,LayerType);
			//pool_yolo2(input_buffer1,output_buffer,Kernel_size,Kernel_stride,TX_MIN_next[0],TR_MIN,TC_MIN,process_flag);
			/*
			    void shortcut(short input_buffer[Tm][Tr][Tc],int output_buffer[Tm][Tr][Tc],
		            const int TM_MIN,const int TR_MIN,const int TC_MIN,bool enable)
			*/
			shortcut(input_buffer1,input_buffer10,output_buffer,TM_MIN,TR_MIN,TC_MIN,process_flag);
			//pool_yolo2(input_buffer1,output_buffer,Kernel_size,Kernel_stride,TX_MIN_next[0],TR_MIN,TC_MIN,process_flag);
		}else
		{
			TMP_X_next[0] = tmp_x;
			TX_MIN_next[0] = tmp_tx_min;
			tmp_x = TMP_M;
			tmp_tx_min = TM_MIN;

			//copy_input_weight(Input,Input1,Input2,Input3,Weight,InFM_num,Input_w,Input_h,Kernel_size,Kernel_stride,TMP_R,TMP_C,TMP_M,TMP_M,
			//	TM_MIN,TM,TRow,TCol,0,input_buffer1,weight_buffer0,NOP,input_flag,0,0,IHxIW,KxK,IFM_numxKxK,LayerType,trow_loops);
			copy_input_weight(Input0,Weight,InFM_num,Input_w,Input_h,OutFM_num,Kernel_size,Kernel_stride,TMP_R,TMP_C,TMP_M,TMP_M,
				TM_MIN,TM,TRow,TCol,0,input_buffer1,NULL,NOP,input_flag,0,0,IHxIW,KxK,IFM_numxKxK,LayerType);
			copy_input_weight(Input1,Weight,InFM_num,Input_w,Input_h,OutFM_num,Kernel_size,Kernel_stride,TMP_R,TMP_C,TMP_M,TMP_M,
				TM_MIN,TM,TRow,TCol,0,input_buffer10,NULL,NOP,input_flag,0,0,IHxIW,KxK,IFM_numxKxK,LayerType);
			//pool_yolo2(input_buffer0,output_buffer,Kernel_size,Kernel_stride,TX_MIN_next[0],TR_MIN,TC_MIN,process_flag);
			shortcut(input_buffer0,input_buffer00,output_buffer,TM_MIN,TR_MIN,TC_MIN,process_flag);
		}

	}
	else if(LayerType==2)
	{
		if(pingpongx==0)
		{
			TMP_X_next[0] = tmp_x;
			TX_MIN_next[0] = tmp_tx_min;
			tmp_x = TMP_M;
			tmp_tx_min = TM_MIN;

			copy_input_weight(Input0,Weight,InFM_num,Input_w,Input_h,OutFM_num,Kernel_size,Kernel_stride,TMP_R,TMP_C,TMP_M,TMP_M,
				TM_MIN,TM,TRow,TCol,0,input_buffer0,NULL,NOP,input_flag,0,0,IHxIW,KxK,IFM_numxKxK,LayerType);
			//reorg_yolo2(input_buffer1,output_buffer,Kernel_size,Kernel_stride,TX_MIN_next[0],TR_MIN,TC_MIN,process_flag);
			#ifdef UPSAMPLE_TEST
			for(int i = 0; i < 4; i++){
			    for(int j = 0; j < 13; j ++){
			        for(int k = 0;k < 13;k++){
			            printf("%f,",input_buffer0[i][j][k]);
			        }
			        printf("\n");
			    }
			    printf("\n");
			}
			printf("\n\n");
			#endif
			upsample(input_buffer1,output_buffer,Kernel_stride,TM_MIN,TR_MIN,TC_MIN,process_flag);
		}else
		{
			TMP_X_next[0] = tmp_x;
			TX_MIN_next[0] = tmp_tx_min;
			tmp_x = TMP_M;
			tmp_tx_min = TM_MIN;

			copy_input_weight(Input0,Weight,InFM_num,Input_w,Input_h,OutFM_num,Kernel_size,Kernel_stride,TMP_R,TMP_C,TMP_M,TMP_M,
				TM_MIN,TM,TRow,TCol,0,input_buffer1,NULL,NOP,input_flag,0,0,IHxIW,KxK,IFM_numxKxK,LayerType);
			//reorg_yolo2(input_buffer0,output_buffer,Kernel_size,Kernel_stride,TX_MIN_next[0],TR_MIN,TC_MIN,process_flag);
			#ifdef UPSAMPLE_TEST
			for(int i = 0; i < 4; i++){
			    for(int j = 0; j < 13; j ++){
			        for(int k = 0;k < 13;k++){
			            printf("%f,",input_buffer1[i][j][k]);
			        }
			        printf("\n");
			    }
			    printf("\n");
			}
			printf("\n\n");
			#endif
			upsample(input_buffer0,output_buffer,Kernel_stride,TM_MIN,TR_MIN,TC_MIN,process_flag);

		}
		printf("TRow = %d,TMP_R = %d,TR_MIN = %d,TC_MIN = %d,Kernel_stride = %d\n",TRow,TMP_R,TR_MIN,TC_MIN,Kernel_stride);
		//printf("TM_MIN = %d,TR_MIN = %d,TC_MIN = %d\n",TM_MIN,TR_MIN,TC_MIN);
		//	for(int i = 0;i < 26*26;i++){
		//        printf("output_buffer[%d][%d][%d] = %f\n",(int)i/(26*26),(int)(i/26),(int)(i%26),output_buffer[0][(int)(i/26)][(int)(i%26)]);
		//    }

	}

}
/*
float *Input0,float *Input1,float *Output,float *Weight,float *Beta,输入输出权值偏移数组地址
,const int InFM_num,const int OutFM_num, 输入的通道总数，输出通道总数，在v2代码里reorg的输入整个是按照一个channel处理的
const int Kernel_size,const int Kernel_stride,卷积核大小步长
const int Input_w,const int Input_h,const int Padding,const bool IsNL,const bool IsBN,输入长和宽，是否填充，是否relu是否批量正则化
const int TM,const int TN,const int TR,const int TC,计算好的参数
const int mLoops,const int nLoops,const int rLoops,const int cLoops,同上，也就是加速器的设计
,const int LayerType 层类型
*/

/*
为什么需要Loop bound？为什么又和类型有关系？
shortcut内KxK=0是不行的，方法1：在输入的时候设置，使用的时候设置为1
	const int TRow = (TR-1)*Kernel_stride+Kernel_size;	const int TCol = (TC-1)*Kernel_stride+Kernel_size;
	这两个在shortcut也成了0；，这里我也把kernel_stride设置成了1，原因是为了保持不变;

接下来就是LoopBound的设置了,我个人觉得这个和conv差不多所以对bound设置成1，他是什么含义咱们先不管
验一波输出

*/
void YOLO2_FPGA(float *Input0,float *Input1,float *Output,float *Weight,float *Beta,const int InFM_num,const int OutFM_num,
							  const int Kernel_size,const int Kernel_stride,
							  const int Input_w,const int Input_h,const int Padding,const bool IsNL,const bool IsBN,
							  const int TM,const int TN,const int TR,const int TC,
							  const int mLoops,const int nLoops,const int rLoops,const int cLoops,const int LayerType)
{
	//const int output_w = (Input_w - Kernel_size + 2*Padding)/Kernel_stride + 1 ;
	//const int output_h = (Input_h - Kernel_size + 2*Padding)/Kernel_stride + 1 ;
	int output_w;
	int output_h;
	int temptrow,temptcol;
	if(LayerType==0){
	    output_w = (Input_w - Kernel_size + (Padding << 1))/Kernel_stride + 1 ;
	    output_h = (Input_h - Kernel_size + (Padding << 1))/Kernel_stride + 1 ;
	} else if(LayerType==1)
	{
		output_w = Input_w;
		output_h = Input_h;
	} else if(LayerType == 2){
	    //you dian wen ti
	    output_w = Input_w*Kernel_stride;
	    output_h = Input_h*Kernel_stride;
	}
	//This is ok!
	//printf("output_w=%d,output_h=%d\n\n",output_w,output_h);

	const int OHxOW = output_h*output_w;
	if(LayerType==2){
	    //temptrow = (TR+1-Kernel_size)/Kernel_stride;
	    temptrow = TR;
	    temptcol = TC;
	    //temptcol = (TC+1-Kernel_size)/Kernel_stride;
	} else {
	    temptrow = (TR-1)*Kernel_stride+Kernel_size;
	    temptcol = (TC-1)*Kernel_stride+Kernel_size;
	}
	const int TRow = temptrow;
	const int TCol = temptcol;
	const int IHxIW   = Input_h*Input_w;
	const int KxK = Kernel_size*Kernel_size;
	const int IFM_numxKxK = InFM_num*KxK;
	const int mLoops_bound = (LayerType) ? (mLoops +2): (mLoops + 1);
	//const int mLoops_bound = (mLoops + 1);

	
	printf("output_w=%d,output_h=%d,TRow=%d,TCol=%d,IHxIW=%d,KxK=%d,IFM_numxKxK=%d,mLoops_bound=%d\n\n",output_w,output_h,TRow,TCol,IHxIW,KxK,IFM_numxKxK,mLoops_bound);

	static float input_buffer0[Tn][OnChipIB_Height][OnChipIB_Width];
#pragma HLS ARRAY_PARTITION variable=input_buffer0 complete dim=1

	static float input_buffer1[Tn][OnChipIB_Height][OnChipIB_Width];
#pragma HLS ARRAY_PARTITION variable=input_buffer1 complete dim=1

    static float input_buffer00[Tn][OnChipIB_Height][OnChipIB_Width];
#pragma HLS ARRAY_PARTITION variable=input_buffer0 complete dim=1

	static float input_buffer10[Tn][OnChipIB_Height][OnChipIB_Width];
#pragma HLS ARRAY_PARTITION variable=input_buffer1 complete dim=1

	static float output_buffer[Tm][Tr*PARA][Tc*PARA];
#pragma HLS ARRAY_PARTITION variable=output_buffer complete dim=1

	static float output_buffer1[Tm][Tr*PARA][Tc*PARA];
#pragma HLS ARRAY_PARTITION variable=output_buffer1 complete dim=1

	static float beta_buffer[MAX_BETA_LENGTH];

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

	if(LayerType==0)
		memcpy(beta_buffer,Beta,OutFM_num*sizeof(float));

	/*
	Loops都是设置好的，那么究竟是哪里出了问题呢
	*/
	printf("rLoops*cLoops*mLoops_bound = %d\n",rLoops*cLoops*mLoops_bound);
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
				//if(LayerType == 1)
				if(LayerType!=0) TM_MIN = Tn;
				//if(LayerType==2) printf("TMP_R=%d,output_h=%d,output_h -TMP_R=%d,TR_MIN=%d\t TMP_C=%d,output_w=%d,output_w -TMP_C=%d,TC_MIN=%d\t TMP_M=%d,OutFM_num=%d,OutFM_num-TMP_M=%d,TM_MIN=%d\n",TMP_R,output_h,output_h-TMP_R,TR_MIN,  TMP_C,output_w,output_w-TMP_C,TC_MIN,  TMP_M,OutFM_num,OutFM_num-TMP_M,TM_MIN);
				bool MneZero = (m!=0);
				bool MneOne = (m!=1);
				bool MnemLoops = (m!=mLoops);
				bool MneMLoopsaddOne = (m!=(mLoops+1));
				bool input_flag = LayerType ? MnemLoops&&MneMLoopsaddOne: MnemLoops;
				bool process_flag = LayerType ? MneZero&&MneMLoopsaddOne : MnemLoops;
				bool write_flag = LayerType ? MneZero&&MneOne : MneZero;
				//printf("FPGA OK 1\n");
				if(pingpongm==0)
				{
					intra_pingpong_wrapper(Input0,Input1,Weight,output_buffer1,beta_buffer,input_buffer0,input_buffer1,input_buffer00,input_buffer10,
									InFM_num, Input_w, Input_h, OutFM_num, Kernel_size, Kernel_stride,
									TMP_R, TMP_C, TMP_M, m, TM_MIN, TR_MIN, TC_MIN, TN, TRow, TCol, Padding,IHxIW,KxK,IFM_numxKxK,nLoops,IsNL,LayerType,TM, TMP_M_next1,TM_MIN_next1, pingpongm, input_flag, process_flag);

					write_back_output_reorg(output_buffer,Output,TMP_R,TMP_C,TMP_M_next0[0],output_w,output_h,TM_MIN_next0[0],TR_MIN,TC_MIN,OHxOW,LayerType,write_flag);
					pingpongm = 1;
				}else
				{
					intra_pingpong_wrapper(Input0,Input1,Weight,output_buffer,beta_buffer,input_buffer0,input_buffer1,input_buffer00,input_buffer10,
									InFM_num, Input_w, Input_h, OutFM_num, Kernel_size, Kernel_stride,
									TMP_R, TMP_C, TMP_M, m, TM_MIN, TR_MIN, TC_MIN, TN, TRow, TCol, Padding,IHxIW,KxK,IFM_numxKxK,nLoops,IsNL,LayerType,TM, TMP_M_next0,TM_MIN_next0, pingpongm, input_flag, process_flag);

					write_back_output_reorg(output_buffer1,Output,TMP_R,TMP_C,TMP_M_next1[0],output_w,output_h,TM_MIN_next1[0],TR_MIN,TC_MIN,OHxOW,LayerType,write_flag);
					pingpongm = 0;
				}

			}
		}
	}
}

int Weight_reorgnaization_anti(float *Weight,float *Weight_reorg,float* Alpha,int IFM_NUM,int OFM_NUM,int Kernel_size,int TM,int TN,const bool IsBN)
{
	const int KxK = Kernel_size*Kernel_size;
	const int IFM_NUMxKxK = IFM_NUM*KxK;

	int m,n;
	int tm,tn,tk;

	float weight_buffer[Tm*Tn*K*K];
	float weight_buffer2[Tm*Tn*K*K];

	int TM_MIN,TN_MIN;
	int offset = 0;

	for( m = 0; m < OFM_NUM; m += TM)
	{
		TM_MIN = MIN(TM,OFM_NUM - m);

		for(n = 0;n < IFM_NUM; n += TN)
		{
			TN_MIN = MIN(TN,IFM_NUM - n);

			int Woffset = m*IFM_NUMxKxK + n*KxK;

			for(tm = 0;tm < TM_MIN; tm++)
			{
				memcpy((float *)(weight_buffer + tm*TN_MIN*KxK),
					(float *)(Weight + tm*IFM_NUMxKxK + Woffset),TN_MIN*KxK*sizeof(float));
			}

			int TN_MINxTM_MIN = TN_MIN*TM_MIN;

			for(tk = 0;tk < KxK; tk++)
				for(tm = 0;tm < TM_MIN; tm++)
					for(tn = 0;tn < TN_MIN;tn++)
					{
						weight_buffer2[tk*TN_MINxTM_MIN + tm*TN_MIN + tn] = weight_buffer[tm*TN_MIN*KxK + tn*KxK + tk];
					}



			memcpy((float *)(Weight_reorg+offset),weight_buffer2,TM_MIN*TN_MIN*KxK*sizeof(float));
			offset += TM_MIN*TN_MIN*KxK;
		}							
	}

	return 0;
}

/*
TM * mLoops 是计算以后的通道数量 等于 l.n
TN * nLoops 是计算以前的通道数量 等于 l.c
TR * rLoops 是计算的行数量 等于 l.h
TC * cLoops 是计算的列数量 等于 l.w
*/


void yolov2_hls_ps(network *net, float *input)
{
	int x;

	network orig = *net;
	net->input = input;
    int weight_offset[128] = {864,18432,2048,18432,
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
	int beta_offset[128] = {32,64,32,64,
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
	
	int offset_index = 0;

	//float *Weight_buf = (float *)calloc(203767168/4,sizeof(float));
	//float *Beta_buf   = (float *)calloc(43044/4,sizeof(float));
	float *Weight_buf = (float *)calloc(247583104/4,sizeof(float));
	float *Beta_buf   = (float *)calloc(108276/4,sizeof(float));

#ifdef REORG_TEST
	FILE *fp_w = fopen("weights_reorg.bin", "rb");
    	if(!fp_w) file_error("weights_reorg.bin");
#else
	FILE *fp_w = fopen("weights.bin", "rb");
    	if(!fp_w) file_error("weights.bin");
#endif

#ifdef REORG_GEN
	//float *Weight_reorg_buf = (float *)calloc(203767168/4,sizeof(float));
	float *Weight_reorg_buf = (float *)calloc(247583104/4,sizeof(float));
	FILE *fp_w_reorg = fopen("weights_reorg.bin", "wb");
    	if(!fp_w_reorg) file_error("weights_reorg.bin");
#endif

	FILE *fp_b = fopen("bias.bin", "rb");
    	if(!fp_b) file_error("bias.bin");

	//fread(Weight_buf, sizeof(float), 203767168/4, fp_w);
	//fread(Beta_buf, sizeof(float), 43044/4, fp_b);
	fread(Weight_buf, sizeof(float), 247583104/4, fp_w);
	fread(Beta_buf, sizeof(float), 108276/4, fp_b);
	
	fclose(fp_w);
	fclose(fp_b);


//#define MEM_LEN (416*416*32+208*208*32)
//#define MEM_LEN (608*608*32)
/*
	float *Memory_buf = (float*)calloc(MEM_LEN+1024*2,sizeof(float));//leave some memories for overflow
	float *Memory_top = Memory_buf+1024;
	float *Memory_bottom = Memory_top + MEM_LEN;
	memcpy(Memory_top,input,416*416*3*sizeof(float));//416x416x3 input_pic
*/

	#define MEM_LEN (HALFWID*HALFWID*64)

	float* Memory_top = (float*)calloc(MEM_LEN*6+BLOCK*6,sizeof(float));/*为什么加1024？*/
	
	float* Memory_top1 = Memory_top+BLOCK;
	float* Memory_top2 = Memory_top1+MEM_LEN*2+BLOCK;
	float* Memory_top3 = Memory_top2+MEM_LEN+BLOCK;
	float* Memory_top4 = Memory_top3+MEM_LEN+BLOCK;
	float* Memory_top5 = Memory_top4+MEM_LEN+BLOCK;
	float* Memory_bot  = Memory_top5+MEM_LEN+BLOCK;
	
	memcpy(Memory_top2,input,HALFWID*HALFWID*4*3*sizeof(float));
	
	float* in_ptr[107];
	float* out_ptr[107];
	
	#define ROUTE85_LEN (ATOMWID*ATOMWID*1024)
	#define ROUTE97_LEN (ATOMWID*ATOMWID*2048)
	
	//float* yolo_buf = (float *)calloc(HALFWID*HALFWID*64,sizeof(float));


/*
重写了内存管理部分，只需要五个小的buff就可以做完
但是不知道为什么，buf1一直不对，所以用了六个
修改后的代码不再需要route层，相对于之前的更快一些

似乎懂了为什么有问题，因为36层依赖于33，而33存在buf1，所以需要换个位置
给33层换个位置以后就好了。确实只需要五个小buf

*/
	int i = 0;

	in_ptr[0] = Memory_top2;
	out_ptr[0] = Memory_top1;
	in_ptr[1] = out_ptr[0];
	out_ptr[1] = Memory_top2;
	in_ptr[2] = Memory_top2;
	out_ptr[2] = Memory_top1;

	for(i=3;i<6;i++){
		if(i%2==0){
			in_ptr[i] = Memory_top3;
			out_ptr[i] = Memory_top1;
		} else {
			
			in_ptr[i] = out_ptr[i-1];
			out_ptr[i] = Memory_top3;
		}
	}
	
	for(i=6;i<10;i++){
		if(i%2==0){
			in_ptr[i] = out_ptr[i-1];
			out_ptr[i] = Memory_top2;
		} else {
			in_ptr[i] = Memory_top2;
			out_ptr[i] = Memory_top1;
		}
	}
	
	for(i = 10;i < 14;i++){
		if(i%2==0){
			in_ptr[i] = Memory_top1;
			out_ptr[i] = Memory_top3;
		} else {
			in_ptr[i] = out_ptr[i-1];
			out_ptr[i] = Memory_top1;
		}
	}
	
	for(i = 14;i< 17;i++){
		if(i%2==0){
			in_ptr[i] = Memory_top1;
			out_ptr[i] = Memory_top2;
		} else {
			in_ptr[i] = out_ptr[i-1];
			out_ptr[i] = Memory_top1;
		}
	}
	for(i=17;i<20;i++){
		if(i%2==0){
			in_ptr[i] = Memory_top3;
			out_ptr[i] = Memory_top2;
		} else {
			in_ptr[i] = out_ptr[i-1];
			out_ptr[i] = Memory_top3;
		}
	}
	for(i=20;i<23;i++){
		in_ptr[i] = out_ptr[i-1];
		if(i%2==0)
			out_ptr[i] = Memory_top1; 
		else 
			out_ptr[i] = Memory_top3;
	}
	for(i=23;i<26;i++){
		in_ptr[i] = out_ptr[i-1];
		if(i%2==0)
			out_ptr[i] = Memory_top1; 
		else 
			out_ptr[i] = Memory_top2;
	}
	for(i=26;i<29;i++){
		in_ptr[i] = out_ptr[i-1];
		if(i%2==0)
			out_ptr[i] = Memory_top3; 
		else 
			out_ptr[i] = Memory_top2;
	}
	for(i=29;i<32;i++){
		in_ptr[i] = out_ptr[i-1];
		if(i%2==0)
			out_ptr[i] = Memory_top3; 
		else 
			out_ptr[i] = Memory_top1;
	}
	for(i=32;i<35;i++){
		in_ptr[i] = out_ptr[i-1];
		if(i%2==0)
			out_ptr[i] = Memory_top2; 
		else{
			out_ptr[i] = Memory_top4;
		}
	}
	
	in_ptr[35] = Memory_top2;
	out_ptr[35] = Memory_top3;
	
	in_ptr[36] = Memory_top3;
	out_ptr[36] = Memory_top1+ROUTE97_LEN;
	/*0~36 现在都没有问题了*/
	in_ptr[37] = out_ptr[36];
	out_ptr[37] = Memory_top2;
	
	in_ptr[38] = Memory_top2;
	out_ptr[38] = Memory_top3;
	
	for(i=39;i<42;i++){
		in_ptr[i] = out_ptr[i-1];
		if(i%2==0)
			out_ptr[i] = Memory_top3; 
		else 
			out_ptr[i] = Memory_top4;
	}
	
	for(i=42;i<45;i++){
		in_ptr[i] = out_ptr[i-1];
		if(i%2==0)
			out_ptr[i] = Memory_top2; 
		else 
			out_ptr[i] = Memory_top4;
	}
	
	for(i=45;i<48;i++){
		in_ptr[i] = out_ptr[i-1];
		if(i%2==0)
			out_ptr[i] = Memory_top2; 
		else 
			out_ptr[i] = Memory_top3;
	}
	
	for(i=48;i<51;i++){
		in_ptr[i] = out_ptr[i-1];
		if(i%2==0)
			out_ptr[i] = Memory_top4; 
		else 
			out_ptr[i] = Memory_top3;
	}
	for(i=51;i<54;i++){
		in_ptr[i] = out_ptr[i-1];
		if(i%2==0)
			out_ptr[i] = Memory_top4; 
		else 
			out_ptr[i] = Memory_top2;
	}
	for(i=54;i<57;i++){
		in_ptr[i] = out_ptr[i-1];
		if(i%2==0)
			out_ptr[i] = Memory_top3; 
		else 
			out_ptr[i] = Memory_top2;
	}
	for(i=57;i<60;i++){
		in_ptr[i] = out_ptr[i-1];
		if(i%2==0)
			out_ptr[i] = Memory_top3; 
		else 
			out_ptr[i] = Memory_top4;
	}
	
	in_ptr[60] = Memory_top4;
	out_ptr[60] = Memory_top5;
	in_ptr[61] = Memory_top5;
	out_ptr[61] = Memory_top2+ROUTE85_LEN;
	in_ptr[62] = Memory_top2+ROUTE85_LEN;
	out_ptr[62] = Memory_top3;
	in_ptr[63] = Memory_top3;
	out_ptr[63] = Memory_top4;
	
	for(i=64;i<67;i++){
		in_ptr[i] = out_ptr[i-1];
		if(i%2==0)
			out_ptr[i] = Memory_top5; 
		else 
			out_ptr[i] = Memory_top4;
	}
	for(i=67;i<70;i++){
		in_ptr[i] = out_ptr[i-1];
		if(i%2==0)
			out_ptr[i] = Memory_top5; 
		else 
			out_ptr[i] = Memory_top3;
	}
	
	for(i=70;i<73;i++){
		in_ptr[i] = out_ptr[i-1];
		if(i%2==0)
			out_ptr[i] = Memory_top4; 
		else 
			out_ptr[i] = Memory_top3;
	}
	for(i=73;i<81;i++){
		in_ptr[i] = out_ptr[i-1];
		if(i%2==0)
			out_ptr[i] = Memory_top4; 
		else 
			out_ptr[i] = Memory_top5;
	}
	
	for(i=81;i<83;i++){
		in_ptr[i] = out_ptr[i-1];
		if(i%2==0)
			out_ptr[i] = Memory_top4; 
		else 
			out_ptr[i] = Memory_top3;
	}
	in_ptr[83] = out_ptr[79];
	out_ptr[83] = out_ptr[79];
	in_ptr[84] = out_ptr[83];
	out_ptr[84] = Memory_top4;
	
	in_ptr[85] = Memory_top4;
	out_ptr[85] = Memory_top2;
	in_ptr[86] = Memory_top2;
	out_ptr[86] = Memory_top2;
	
	for(i = 87;i<93;i++){
		in_ptr[i] = out_ptr[i-1];
		if(i%2==0)
			out_ptr[i] = Memory_top2; 
		else 
			out_ptr[i] = Memory_top3;
	}
	
	in_ptr[93] = Memory_top2;
	out_ptr[93] = Memory_top4;
	in_ptr[94] = Memory_top4;
	out_ptr[94] = Memory_top2;
	in_ptr[95] = Memory_top3;
	out_ptr[95] = Memory_top3;
	in_ptr[96] = Memory_top3;
	out_ptr[96] = Memory_top2;
	in_ptr[97] = Memory_top2;
	out_ptr[97] = Memory_top1;
	in_ptr[98] = Memory_top1;
	out_ptr[98] = Memory_top1;
	
	for(i = 99;i<107;i++){
		in_ptr[i] = out_ptr[i-1];
		if(i%2==0)
			out_ptr[i] = Memory_top1; 
		else 
			out_ptr[i] = Memory_top2;
	}

    network netp = *net;
    //int i;
	int woffset = 0;
	int aoffset = 0;
	int boffset = 0;
	int TR,TC,TM,TN;
	int output_w,output_h;
	int rLoops,cLoops,mLoops,nLoops;
	double sum_gop = 0.0;
	
	int T2Rate;
	int TRow;
	int trow_loops;

    for(i = 0; i < netp.n; ++i)
	{
        netp.index = i;
        layer l = netp.layers[i];
		printf("Layer[%2d]: ",i);
		switch(l.type)
		{
			case CONVOLUTIONAL:{
				printf("outputMemory:%8d;BN=%d;Activation=%d;conv  %5d %2d x%2d /%2d  %4d x%4d x%4d   ->  %4d x%4d x%4d  %5.3f BFLOPs\n",l.outputs,l.batch_normalize,l.activation, l.n, l.size, l.size, l.stride, l.w, l.h, l.c, l.out_w, l.out_h, l.out_c, (2.0 * l.n * l.size*l.size*l.c/l.groups * l.out_h*l.out_w)/1000000000.);
				sum_gop += (2.0 * l.n * l.size*l.size*l.c/l.groups * l.out_h*l.out_w)/1000000000.;
				output_w = (l.w - l.size + 2*l.pad)/l.stride + 1 ;
				output_h = (l.h - l.size + 2*l.pad)/l.stride + 1 ;

				TR = MIN(((OnChipIB_Height-l.size)/l.stride+1),Tr);//keep Kernel_stride>=1
				TR = MIN(output_h,TR);
				TC = MIN(((OnChipIB_Width-l.size)/l.stride+1),Tc);
				TC = MIN(output_w,TC);
				TM = MIN(l.n,Tm);
				TN = MIN(l.c,Tn);

				rLoops = (int)ceil(((float)output_h)/TR);
				cLoops = (int)ceil(((float)output_w)/TC);
				mLoops = (int)ceil(((float)l.n)/TM);
			    nLoops = (int)ceil(((float)l.c)/TN);
				
				switch(l.w)
				{
					case 26:
						T2Rate = 2;
						break;
					case 13:
						T2Rate = 4;
						break;
					default:
						T2Rate = 1;
						break;
				}
				TRow = (TR-1)*l.stride+l.size;
				trow_loops = (int)ceil(((float)TRow/T2Rate));
				
			    //printf("TR=%d,TC=%d,TM=%d,rLoops=%d,cLoops=%d,mLoops=%d,nLoops=%d\n",TR,TC,TM,rLoops,cLoops,mLoops,nLoops);
				printf("TRow = %d,trow_loops=%d,l.pad=%d,l.c=%d,l.n=%d,TR=%d,TC=%d,TM=%d,TN=%d,rLoops=%d,cLoops=%d,mLoops=%d,nLoops=%d,output_w=%d,output_h=%d\n",TRow,trow_loops,l.pad,l.c,l.n,TR,TC,TM,TN,rLoops,cLoops,mLoops,nLoops,output_w,output_h);

				YOLO2_FPGA(in_ptr[i],NULL,out_ptr[i],Weight_buf+woffset,Beta_buf+boffset,
					l.c,l.n,l.size,
					l.stride,l.w,l.h,l.pad,l.activation==LEAKY?1:0,l.batch_normalize?1:0,
					TM,TN,TR,TC,
					mLoops,nLoops,rLoops,cLoops,0);
#ifdef REORG_GEN
				Weight_reorgnaization_anti(Weight_buf + woffset,Weight_reorg_buf + woffset,NULL,l.c,l.n,l.size,TM,TN,0);
#endif

				woffset += weight_offset[offset_index];
				boffset += beta_offset[offset_index];
				offset_index++;

				break;
			}
			/*
			case MAXPOOL:
				printf("outputMemory:%8d;max          %d x %d / %d  %4d x%4d x%4d   ->  %4d x%4d x%4d\n",l.outputs, l.size, l.size, l.stride, l.w, l.h, l.c, l.out_w, l.out_h, l.out_c);
				//output_w = (l.w - l.size)/l.stride + 1 ;
				//output_h = (l.h - l.size)/l.stride + 1 ;
				output_w = l.out_h;
				output_h = l.out_w;

				TR = MIN(((OnChipIB_Height-l.size)/l.stride+1),Tr);//keep Kernel_stride>=1
				TC = MIN(((OnChipIB_Width-l.size)/l.stride+1),Tc);

				TR = MIN(output_h,TR);
				TC = MIN(output_w,TC);
				TM = MIN(Tm,Tn);
				TM = MIN(l.c,TM);

				rLoops = (int)ceil(((float)output_h)/TR);
				cLoops = (int)ceil(((float)output_w)/TC);
				mLoops = (int)ceil(((float)l.c)/TM);

				YOLO2_FPGA(in_ptr[i],out_ptr[i],NULL,NULL,l.c,l.c,
					l.size,l.stride,l.w,l.h,l.pad,0,0,TM,0,TR,TC,mLoops,0,rLoops,cLoops,1);

				break;
			case REORG:
				printf("outputMemory:%8d;reorg              /%2d  %4d x%4d x%4d   ->  %4d x%4d x%4d\n",l.outputs,  l.stride, l.w, l.h, l.c, l.out_w, l.out_h, l.out_c);			
				output_w = 26;
				output_h = 32*13;

				TR = MIN(((OnChipIB_Height-l.stride)/l.stride+1),Tr);//keep Kernel_stride>=1
				TR = MIN(output_h,TR);
				TC = MIN(((OnChipIB_Width-l.stride)/l.stride+1),Tc);
				TC = MIN(output_w,TC);
				TM = 4;

				rLoops = (int)ceil(((float)output_h)/TR);
				cLoops = (int)ceil(((float)output_w)/TC);
				mLoops = 1;

				YOLO2_FPGA(in_ptr[i],out_ptr[i],NULL,NULL,1,4,
							  l.stride,l.stride,52,32*26,0,0,0,TM,0,TR,TC,mLoops,0,rLoops,cLoops,2);

				break;
			case ROUTE:
				printf("outputMemory:%8d;route ",l.outputs);
				int j;
				for(j = 0; j < l.n; ++j){
					printf(" %d", l.input_layers[j]);
				}
				printf("\n");
				break;
			case REGION:
				printf("outputMemory:%8d;Detection\n",l.outputs);
				forward_region_layer(l, in_ptr[i]);
				break;
			}
		*/
		    case ROUTE:{
				printf("outputMemory:%8d;route ",l.outputs);
				int j;
				for(j = 0; j < l.n; ++j){
					printf(" %d", l.input_layers[j]);
				}
				printf("\n");
				//forward_route_layer(l,netp);
				break;
			}
			case SHORTCUT:{
			    //as same as reorg
			    printf("res  %3d                %4d x%4d x%4d   ->  %4d x%4d x%4d\n",l.index, netp.layers[i-1].w,netp.layers[i-1].h,netp.layers[i-1].n, l.w,l.h,l.c);
/*
			    output_w = l.out_h;
				output_h = l.out_w;

				TR = MIN(((OnChipIB_Height-l.stride)/l.stride+1),Tr);//keep Kernel_stride>=1
				TR = MIN(output_h,TR);
				TC = MIN(((OnChipIB_Width-l.stride)/l.stride+1),Tc);
				TC = MIN(output_w,TC);
				TM = 4;
				TN = TM;

				rLoops = (int)ceil(((float)output_h)/TR);
				cLoops = (int)ceil(((float)output_w)/TC);
				mLoops = 1;
*/
				//TM=TN;
				//mLoops=nLoops;
				//TN=TM;
				//nLoops=mLoops;
				output_w = l.out_h;
				output_h = l.out_w;

				//TR = MIN(((OnChipIB_Height-l.stride)/l.stride+1),Tr);//keep Kernel_stride>=1
				TR = MIN(output_h,Tr);
				//TC = MIN(((OnChipIB_Width-l.stride)/l.stride+1),Tc);
				TC = MIN(output_w,Tc);
				//TM = 4;
				//TN = TM;
				//TM = MIN(l.n,Tm);
				TN = MIN(l.c,Tn);
				TM = TN;

				rLoops = (int)ceil(((float)output_h)/TR);
				cLoops = (int)ceil(((float)output_w)/TC);
				//mLoops = (int)ceil(((float)l.n)/TM);
			    nLoops = (int)ceil(((float)l.c)/TN);
				mLoops = nLoops;
				//mLoops = 1;
				
				switch(l.w)
				{
					case 26:
						T2Rate = 2;
						break;
					case 13:
						T2Rate = 4;
						break;
					default:
						T2Rate = 1;
						break;
				}
				TRow = TR;
				trow_loops = (int)ceil(((float)TRow/T2Rate));
				
			    //printf("TR=%d,TC=%d,TM=%d,rLoops=%d,cLoops=%d,mLoops=%d,nLoops=%d\n",TR,TC,TM,rLoops,cLoops,mLoops,nLoops);
				printf("TRow = %d,trow_loops=%d,l.pad=%d,l.c=%d,l.n=%d,TR=%d,TC=%d,TM=%d,TN=%d,rLoops=%d,cLoops=%d,mLoops=%d,nLoops=%d,output_w=%d,output_h=%d\n",TRow,trow_loops,l.pad,l.c,l.n,TR,TC,TM,TN,rLoops,cLoops,mLoops,nLoops,output_w,output_h);
/*				
				switch(52)
				{
					case 26:
						T2Rate = 2;
						break;
					case 13:
						T2Rate = 4;
						break;
					default:
						T2Rate = 1;
						break;
				}
				TRow = (TR-1)*l.stride+l.stride;
				trow_loops = (int)ceil(((float)TRow/T2Rate));
*/
/*
				YOLO2_FPGA(in_ptr[i],out_ptr[i],NULL,NULL,l.c,l.c,
					l.size,l.stride,l.w,l.h,l.pad,0,0,TM,0,TR,TC,mLoops,0,rLoops,cLoops,1);
*/
				YOLO2_FPGA(out_ptr[i-1],out_ptr[l.index],out_ptr[i],NULL,NULL,l.c,l.c,
					1,1,l.w,l.h,l.pad,0,0,TM,TN,TR,TC,mLoops,nLoops,rLoops,cLoops,1);
					//inputQ[offset_index],inputQ[offset_index],INTERWIDTH,INTERWIDTH,trow_loops);
			    break;
			}
			case UPSAMPLE:{
			    //as same as pool
			    printf("upsample           %2dx  %4d x%4d x%4d   ->  %4d x%4d x%4d\n", l.stride, l.w, l.h, l.c, l.out_w, l.out_h, l.out_c);
			    output_w = l.out_w;
				output_h = l.out_h;

				//TR TC 到底怎么选择？是按照最小的还是只按输出？
				//是按照输出来的，因为loop除以的是输出宽度，但是事实只需要一遍
				//所以是按照输入计算
				//但是好像都可以，看后期实现哪个方便吧
				//TR = MIN(output_h,Tr);
				TR = 13;
				//TR = MIN(TR,l.h);
				//TC = MIN(((OnChipIB_Width-l.stride)/l.stride+1),Tc);
				//TC = MIN(output_w,Tc);
				TC = 13;
				//TC = MIN(TC,l.w);
				//TM = 4;
				//TN = TM;
				//TM = MIN(l.n,Tm);
				TN = MIN(l.c,Tn);
				TM = TN;

				rLoops = (int)ceil(((float)l.h)/TR);
				cLoops = (int)ceil(((float)l.w)/TC);
				//mLoops = (int)ceil(((float)l.n)/TM);
			    nLoops = (int)ceil(((float)l.c)/TN);
				mLoops = nLoops;
		
				switch(l.w)
				{
					case 13:
						T2Rate = 1;
						break;
					default:
						T2Rate = 1;
						break;
				}
				TRow = TR;
				trow_loops = (int)ceil(((float)TRow/T2Rate));
				
			    //printf("TR=%d,TC=%d,TM=%d,rLoops=%d,cLoops=%d,mLoops=%d,nLoops=%d\n",TR,TC,TM,rLoops,cLoops,mLoops,nLoops);
				printf("l.w = %d,l.stride = %d,TRow = %d,trow_loops=%d,l.pad=%d,l.c=%d,l.n=%d,TR=%d,TC=%d,TM=%d,TN=%d,rLoops=%d,cLoops=%d,mLoops=%d,nLoops=%d,output_w=%d,output_h=%d\n",l.w,l.stride,TRow,trow_loops,l.pad,l.c,l.n,TR,TC,TM,TN,rLoops,cLoops,mLoops,nLoops,output_w,output_h);
/*				
				switch(52)
				{
					case 26:
						T2Rate = 2;
						break;
					case 13:
						T2Rate = 4;
						break;
					default:
						T2Rate = 1;
						break;
				}
				TRow = (TR-1)*l.stride+l.stride;
				trow_loops = (int)ceil(((float)TRow/T2Rate));
*/
//				YOLO2_FPGA(in_ptr[i],in_ptr[i],in_ptr[i],in_ptr[i],out_ptr[i],out_ptr[i],NULL,NULL,1,4,
//					l.stride,l.stride,52,32*26,output_w,output_h,
//					0,0,0,TM,TN,TR,TC,mLoops,1,rLoops,cLoops,2);
//(float *Input0,float *Input1,float *Output,float *Weight,float *Beta,const int InFM_num,const int OutFM_num,
//							  const int Kernel_size,const int Kernel_stride,
//							  const int Input_w,const int Input_h,const int Padding,const bool IsNL,const bool IsBN,
//							  const int TM,const int TN,const int TR,const int TC,
//							  const int mLoops,const int nLoops,const int rLoops,const int cLoops,const int LayerType)
				YOLO2_FPGA(in_ptr[i],in_ptr[i],out_ptr[i],NULL,NULL,l.c,l.c,
					1,l.stride,
					l.w,l.h,l.pad,0,0,
					TM,TN,TR,TC,
					mLoops,nLoops,rLoops,cLoops,2);
					//inputQ[offset_index],inputQ[offset_index],INTERWIDTH,INTERWIDTH,trow_loops);
			    			    
			    break;
			}
			case YOLO:{
			    //YOLO as same as REGION
				printf("outputMemory:%8d;yolo ",l.outputs);	
			    /*
				double OutputPara = pow(2.0,-inputQ[offset_index]);
				bool NextPixelFlag = true;
				int OutputPixelOffset = 0;
				short current_p,next_p,output_p;
				int *Output_ptr = (int *)(in_ptr[i]);
				for(j=0;j<l.outputs;j++)
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
					yolo_buf[j] = output_p*OutputPara;
				}*/
				netp.layers[i].output = forward_yolo_array(l,out_ptr[i-1]);    
			    break;
			}
			
		}

		netp.input = l.output;
/*
		    for( x = 0; x < 500; x++)
		    {
			    //sprintf(line, "%f\n", out_ptr[i][x]);
				printf("%f,",out_ptr[i][x]);
				if((x+1)%10==0){
					printf("\n");
				}
			    //if(fputs(line,fp3)<0)fprintf(stderr,"write FILE failed\n");
		    }
*/	
		/*
		//mycode 
		//if(i==84||i==85){
		    char line[256];
		    FILE *fp3;
		    char filename[256];
		    sprintf(filename, "fpga_net_layer_%d.txt", i);
		    if( (fp3 = fopen(filename, "w")) == NULL)fprintf(stderr,"CANNOT OPEN\n");
		    for( x = 0; x < l.outputs; x++)
		    {
			    sprintf(line, "%f\n", out_ptr[i][x]);
			    if(fputs(line,fp3)<0)fprintf(stderr,"write FILE failed\n");
		    }
		    fclose(fp3);
		    printf("layer[%d]:Write END!\n\n",i);
		*/
		//}

    }
	printf("SUM_GOP=%g\n",sum_gop);
	*net = orig;

#ifdef REORG_GEN
	//fwrite(Weight_reorg_buf, sizeof(float), 203767168/4, fp_w_reorg);
	fwrite(Weight_reorg_buf, sizeof(float), 247583104/4, fp_w_reorg);
	fclose(fp_w_reorg);
	free(Weight_reorg_buf);
#endif
	free(Memory_top);
	free(Weight_buf);
	free(Beta_buf);

}
///////////////////////////////////////////////////////////////////////20181229 anti-reorg ok end n4m32
