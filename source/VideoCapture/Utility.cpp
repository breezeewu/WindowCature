#include "stdafx.h"
#include "Utility.h"

//get the cpuid
#ifdef _WIN64
CCpuID g_cpuid;

CCpuID::CCpuID()
{
	// TODOX64 : ??
	m_flags = (flag_t)7;
}
#else // _WIN64

#define CPUID_STD_MMX          0x00800000
#define CPUID_STD_SSE          0x02000000
#define CPUID_STD_SSE2         0x04000000
#define CPUID_EXT_3DNOW        0x80000000

#define RGB2YUV(b, g, r, y, u, v) \
	y=(BYTE)(((int)30*r +(int)59*g +(int)11*b)/100); \
	u=(BYTE)(((int)-17*r -(int)33*g +(int)50*b+12800)/100); \
	v=(BYTE)(((int)50*r -(int)42*g -(int)8*b+12800)/100)

#define YUV2RGB(b, g, r, y, u, v) \
	r = ((int)((y) + 1.4075 * ((v) - 128)));\
	g = ((int)((y) - 0.3455 * ((u) - 128) - 0.7169 * ((v) - 128)));\
	b = ((int)((y) + 1.779 * ((u) - 128)))

#define Clip1(x) if((x) < 0){x = 0;} if((x) > 255){x = 255;}

bool CPUTest()
{
	 __try 
    {
        __asm xor    eax, eax
        __asm xor    ebx, ebx
        __asm xor    ecx, ecx
        __asm xor    edx, edx
        __asm cpuid
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
		g_cpuType = CPU_UNKNOW;
        return false;
    }
	unsigned int cpuFeature = 0;

	//探测CPU类型
	__asm
	{
		xor     eax, eax                      // CPUID function #0
		cpuid                                 // largest std func/vendor string

		test    eax, eax                      // largest standard function==0?
		jz      $no_standard_features         // yes, no standard features func

		mov eax, 1
		cpuid
		//测试是否支持MMX
		mov ecx,CPUID_STD_MMX
		and ecx,edx
		or  cpuFeature,ecx
		//测试是否支持SSE
		mov ecx,CPUID_STD_SSE
		and ecx,edx
		or  cpuFeature,ecx
		//测试是否支持SSE2
		mov ecx,CPUID_STD_SSE2
		and ecx,edx
		or  cpuFeature,ecx

$no_standard_features:
		}

    	bool avx_flag=false; //AVX_Support();
		if(cpuFeature >= (CPUID_STD_MMX + CPUID_STD_SSE + CPUID_STD_SSE2))
		{
			g_cpuType = CPU_SSE2;
		}
		else if(cpuFeature >= (CPUID_STD_MMX))
		{
			g_cpuType = CPU_MMX;
		}
		else 
		{
			g_cpuType = CPU_UNKNOW;
			return false;
		}

		if (avx_flag==true)
		{
		//	trace("AVX2检测成功￥￥￥￥￥￥￥￥￥￥￥￥￥￥");
			g_cpuType=CPUS_AVX2;
		}

		//测试操作系统是否支持SIMD
		_try {
		__asm xorps xmm0, xmm0 ;Streaming SIMD Extension
		} 
		_except(EXCEPTION_EXECUTE_HANDLER) {
		if (_exception_code()==STATUS_ILLEGAL_INSTRUCTION)
		{
			/* SSE not supported */
			g_cpuType = CPU_UNKNOW;
			return false;
		}
	}
	return true;
}

CCpuID::CCpuID()
{
	DWORD flags = 0;
	__asm
	{
		mov			eax, 1
		cpuid
		test		edx, 0x00800000		// STD MMX
		jz			TEST_SSE
		or			[flags], 1
TEST_SSE:
		test		edx, 0x02000000		// STD SSE
		jz			TEST_SSE2
		or			[flags], 2
		or			[flags], 4
TEST_SSE2:
		test		edx, 0x04000000		// SSE2	
		jz			TEST_3DNOW
		or			[flags], 8
TEST_3DNOW:
		mov			eax, 0x80000001
		cpuid
		test		edx, 0x80000000		// 3D NOW
		jz			TEST_SSEMMX
		or			[flags], 16
TEST_SSEMMX:
		test		edx, 0x00400000		// SSE MMX
		jz			TEST_END
		or			[flags], 2
TEST_END:
	}
	m_flags = (flag_t)flags;
}
CCpuID g_cpuid;
#endif

void RGB32toYV12(BYTE *pSrc,   BYTE *pDst, int nSrcPitch,int nDstPitch, int width, int height)
{
	if(g_cpuType == CPU_SSE2)
	{
		if(width<33)
		{
			RGB32toYV12C(pSrc,pDst,nSrcPitch,nDstPitch,width,height);
		}
		else
		{
			RGB32toYV12SSE(pSrc,pDst,nSrcPitch,nDstPitch,width,height);

		}
	}
	else if(g_cpuType == CPU_MMX)
	{
		RGB32toYV12MMX(pSrc,pDst,nSrcPitch,nDstPitch,width,height);
	}
	else
	{
		RGB32toYV12C(pSrc,pDst,nSrcPitch,nDstPitch,width,height);
	}
}
void RGB32toYV12SSE(BYTE *pSrc,   BYTE *pDst, int nSrcPitch,int nDstPitch, int width, int height)
{
	//MessageBox(0,0,0,0);
	BYTE   *yp,   *up,   *vp; 
	BYTE   *prow; 
	int   i,   j ; 

	BYTE *y = pDst;
	BYTE *u = y +nDstPitch * height;
	BYTE *v = u +nDstPitch/2 * height/2;

	__declspec(align(16))
		static short int ycoefs[8] = {2851,   22970,   6947,   0, 2851,   22970,   6947,   0 };
	__declspec(align(16))
		static short int ucoefs[8] =   {16384,   -12583,   -3801,   0, 16384,   -12583,   -3801,   0 };
	__declspec(align(16))
		static short int vcoefs[8] = {-1802,   -14582,   16384,   0, -1802,   -14582,   16384,   0 };

	_asm   
	{ 
		xor    edx,   edx //置零
		mov    eax,   width //赋值
		sar    eax,2			//右移
		cmp    edx,   eax		//比较
		jge    yuvexit			//跨度等于0则退出

		mov    j,   eax			//j=跨度
		mov    eax,   height	//赋值

		mov    i,   eax			//i=高
		cmp    edx,   eax		//高==0
		jge    yuvexit			//高为0退出

		mov    eax,   y			//目标内存y
		mov    yp,   eax		//赋值给yp
		mov    eax,   u			//u数据指针
		mov    up,   eax		//赋值给up
		mov    eax,   v			//v数据指针
		mov    vp,   eax		//赋值给vp
		mov    eax,   pSrc		//源数据指针赋值给prow
		mov    prow,   eax 
		pxor    xmm7,   xmm7	//xmm7专属寄存器清零
		mov    eax,   i			//eax = 高

heighttop:

		mov    i,   eax			//高
		mov    edi,   j			//跨度
		mov    ebx,   prow		//源数据指针
		mov    ecx,   yp		//y数据指针
		mov    edx,   up		//u数据指针
		mov    esi,   vp		//v数据指针

widthtop: 
		movdqu   xmm0,   [ebx] //[]取内容    //   MM5   has   0   r2   g2   b2   0   r1   g1   b1,   4   pixels  
		movq      xmm5,   xmm0 //把源存储器内容值送入目的寄存器,但不必对齐内存16字节 xmm1  = 00..00 g3 b3 r2 g2 b2 r1 g1 b1
		pshufd     xmm0 ,xmm0 ,0x4e	//完成媒体寄存器与媒体寄存器的按16位宽度进行的数据交换
		movq       xmm4,xmm0		//64位数据拷贝,如果内存8位对齐的话,是一个64位写,否则2个32位写
		add           ebx,   16		//

		punpcklbw   xmm5,   xmm7   //   MM5   has   0   r2   g2   b2 0   r1   g1   b1 
		punpcklbw   xmm4,   xmm7   //   MM4   has  0   r4   g4   b4 0   r3   g3   b3

		movdqu      xmm0,  xmm5 //a
		movdqu      xmm1,   xmm4 //a

		pmaddwd   xmm0,   ycoefs   //   MM0   has   r1*cr   and   g1*cg+b1*cb 
		movdqu      xmm2,   xmm0 //a
		PSRLQ     xmm2,   32 
		paddd     xmm0,   xmm2       //   MM0   has   y1 y2  in   lower   32   bits 
		pmaddwd   xmm1,   ycoefs   //   MM1   has   r2*cr   and   g2*cg+b2*cb 
		movdqu      xmm2,   xmm1 //a
		PSRLQ    xmm2,   32 
		paddd     xmm1,   xmm2       //   MM1   has   y3 y4   in   lower   32   bits 

		movd      eax,  xmm0 
		imul      eax,   219 
		shr       eax,   8 
		add       eax,   540672 
		shr       eax,   15 
		mov       [ecx],   al 
		inc       ecx 

		pshufd     xmm0 ,xmm0 ,0x4e
		movd      eax,  xmm0 
		imul      eax,   219 
		shr       eax,   8 
		add       eax,   540672 
		shr       eax,   15 
		mov       [ecx],   al 
		inc       ecx 

		movd      eax,   xmm1 
		imul      eax,   219 
		shr       eax,   8 
		add       eax,   540672 
		shr       eax,   15 
		mov       [ecx],   al 
		inc       ecx 

		pshufd     xmm1 ,xmm1 ,0x4e
		movd      eax,  xmm1 
		imul      eax,   219 
		shr       eax,   8 
		add       eax,   540672 
		shr       eax,   15 
		mov       [ecx],   al 
		inc       ecx 

		test  i, 0x00000001
		jnz  L1


		movdqu      xmm0,   xmm5 //a
		movdqu      xmm1,   xmm4 //a

		pmaddwd   xmm0,   ucoefs   //   MM0   has   r1*cr   and   g1*cg+b1*cb 
		movdqu      xmm2,  xmm0 //a
		PSRLQ     xmm2,   32 
		paddd     xmm0,   xmm2       //   MM0   has   u1   in   lower   32   bits 


		movd      eax,   xmm0 
		imul      eax,   224 
		sar       eax,   8 
		add       eax,   4210688 
		shr       eax,   15 
		mov       [edx],   al 
		inc       edx 
        
		movdqu      xmm0,   xmm5 //a
		pshufd     xmm0 ,xmm0 ,0x4e

		pmaddwd   xmm0,   vcoefs   //   MM0   has   r1*cr   and   g1*cg+b1*cb 
		movdqu      xmm2,  xmm0 //a
		PSRLQ     xmm2,   32 
		paddd     xmm0,   xmm2       //   MM0   has   u1   in   lower   32   bits 

		movd      eax,   xmm0 
		imul      eax,   224 
		sar       eax,   8 
		add       eax,   4210688 
		shr       eax,   15 
		mov       [esi],   al 
		inc       esi 

		pmaddwd   xmm1,   ucoefs   //   MM5   has   r1*cr   and   g1*cg+b1*cb 
		movdqu     xmm2,   xmm1 //a
		PSRLQ     xmm2,   32 
		paddd     xmm1,   xmm2       //   MM5   has   v1   in   lower   32   bits 

		movd      eax,   xmm1 
		imul      eax,   224 
		sar       eax,   8 
		add       eax,   4210688 
		shr       eax,   15 
		mov       [edx],   al 
		inc       edx 
        
		movdqu      xmm1,   xmm4 //a
		pmaddwd   xmm1,   vcoefs   //   MM5   has   r1*cr   and   g1*cg+b1*cb 

		movdqu     xmm2,   xmm1 //a
		PSRLQ     xmm2,   32 
		paddd     xmm1,   xmm2       //   MM5   has   v1   in   lower   32   bits 

       
		movd      eax,   xmm1 
		imul      eax,   224 
		sar       eax,   8 
		add       eax,   4210688 
		shr       eax,   15 
		mov       [esi],   al 
		inc       esi 


L1:
		dec       edi 	
		jnz       widthtop 

		mov       eax,   nDstPitch 
		add       yp,   eax 
		sar        eax,1

		test  i, 0x00000001
		jnz  L2

		add       up,   eax 
		add       vp,   eax 
L2:
		mov       eax,   nSrcPitch 
		add       prow,   eax 
		mov       eax,   i 
		dec       eax 
		jnz       heighttop 

yuvexit:
		__asm emms 
	} 
}
void RGB32toYV12MMX(BYTE *pSrc,   BYTE *pDst, int nSrcPitch,int nDstPitch, int width, int height)
{
	unsigned   char   *yp,   *up,   *vp; 
	unsigned   char   *prow; 
	int   i,   j ; 

	short int   ycoef[4] = {2851,   22970,   6947,   0 };
	short int   ucoef[4] = {16384,   -12583,   -3801,  0 };
	short  int   vcoef[4] = {-1802,   -14582,   16384,   0 };

	short  int   *ycoefs   =   ycoef; 
	short  int   *ucoefs   =   ucoef; 
	short  int   *vcoefs   =   vcoef; 

	BYTE *y = pDst;
	BYTE *u = y +nDstPitch * height;
	BYTE *v = u +nDstPitch/2 * height/2;

	int n = height;
	_asm   
	{ 
		xor    edx,   edx 
		mov    eax,   width 
		sar    eax,1 
		cmp    edx,   eax 
		jge    yuvexit 

		mov    j,   eax 
		mov    eax,   height 

		mov    i,   eax 
		cmp    edx,   eax 
		jge    yuvexit 

		mov    eax,   y 
		mov    yp,   eax 
		mov    eax,   u 
		mov    up,   eax 
		mov    eax,   v 
		mov    vp,   eax 
		mov    eax,   pSrc 
		mov    prow,   eax 
		pxor   MM7,   MM7 
		mov    eax,   i 
heighttop:

		mov    i,   eax 
		mov    edi,   j 
		mov    ebx,   prow 
		mov    ecx,   yp 
		mov    edx,   up 
		mov    esi,   vp 
widthtop: 
		movq        MM5,   [ebx]     //   MM5   has   0   r2   g2   b2   0   r1   g1   b1,   two   pixels 
		add           ebx,   8 
		movq        MM6,   MM5 
		punpcklbw   MM5,   MM7   //   MM5   has   0   r1   g1   b1 
		punpckhbw   MM6,   MM7   //   MM6   has   0   r2   g2   b2 

		movq      MM0,   MM5 
		movq      MM1,   MM6 
		mov       eax,   ycoefs 
		pmaddwd   MM0,   [eax]   //   MM0   has   r1*cr   and   g1*cg+b1*cb 
		movq      MM2,   MM0 
		psrlq     MM2,   32 
		paddd     MM0,   MM2       //   MM0   has   y1   in   lower   32   bits 
		pmaddwd   MM1,   [eax]   //   MM1   has   r2*cr   and   g2*cg+b2*cb 
		movq      MM2,   MM1 
		psrlq     MM2,   32 
		paddd     MM1,   MM2       //   MM1   has   y2   in   lower   32   bits 
		movd      eax,   MM0 
		imul      eax,   219 
		shr       eax,   8 
		add       eax,   540672 
		shr       eax,   15 
		mov       [ecx],   al 
		inc       ecx 
		movd      eax,   MM1 
		imul      eax,   219 
		shr       eax,   8 
		add       eax,   540672 
		shr       eax,   15 
		mov       [ecx],   al 
		inc       ecx 

		test  i, 0x00000001
		jnz  L1


		movq      MM0,   MM5 
		//		movq      MM1,   MM6 
		movq      MM1,   MM5 
		mov       eax,   ucoefs
		pmaddwd   MM0,   [eax]   //   MM0   has   r1*cr   and   g1*cg+b1*cb 
		movq      MM2,   MM0 
		psrlq     MM2,   32 
		paddd     MM0,   MM2       //   MM0   has   u1   in   lower   32   bits 


		movd      eax,   MM0 
		imul      eax,   224 
		sar       eax,   8 
		add       eax,   4210688 
		shr       eax,   15 
		mov       [edx],   al 
		inc       edx 

		mov       eax,   vcoefs 
		pmaddwd   MM5,   [eax]   //   MM5   has   r1*cr   and   g1*cg+b1*cb 
		movq      MM2,   MM5 
		psrlq     MM2,   32 
		paddd     MM5,   MM2       //   MM5   has   v1   in   lower   32   bits 

		movd      eax,   MM5 
		imul      eax,   224 
		sar       eax,   8 
		add       eax,   4210688 
		shr       eax,   15 
		mov       [esi],   al 
		inc       esi 

L1:
		dec       edi 	
		jnz       widthtop 

		mov       eax,   nDstPitch 
		add       yp,   eax 
		sar        eax,1

		test  i, 0x00000001
		jnz  L2

		add       up,   eax 
		add       vp,   eax 
L2:
		mov       eax,   nSrcPitch 
		//sub       prow,   eax 
		add       prow,   eax 
		mov       eax,   i 
		dec       eax 
		jnz       heighttop 

yuvexit:
		emms 
	} 
}
/*
void RGB32toYV12C(BYTE *pSrc,   BYTE *pDst, int nSrcPitch,int nDstPitch, int width, int height)
{
	BYTE* pp = pSrc;
	BYTE* py = pDst;
	BYTE* pu = py + nDstPitch * height;
	BYTE* pv = pu + nDstPitch * height/4;
	BYTE* ptmp = pSrc;
	BYTE* y = py;
	BYTE* u = pu;
	BYTE* v = pv;
	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j+=2)
		{
			RGB2YUV(ptmp[0], ptmp[1], ptmp[2], *y, *u, *v);
			ptmp += 4;
			y++;
			
			RGB2YUV(ptmp[0], ptmp[1], ptmp[2], *y, *u, *v);
			y++;
			ptmp += 4;
			u++;
			v++;
		}
		if(i%2 == 1)
		{	
			pu += (nDstPitch/2);
			pv += (nDstPitch/2);
			u = pu;
			v= pv;
		}
		py += nDstPitch;
		y = py;
		pSrc += nSrcPitch;
		ptmp = pSrc;
	}
	
	if(width%2 == 1)
	{
		BYTE* y = pDst ;
		BYTE* u = py + nDstPitch * height;
		BYTE* v = pu + nDstPitch * height/4;
		BYTE *ptmp = pp +  width*4 - 4;
		BYTE *pSrcY = y + width -1;
		BYTE *pSrcU = u + nDstPitch/2 - 1;
		BYTE *pSrcV = v + nDstPitch/2 - 1;
		for(int i = 0; i < height; i++)
		{
			RGB2YUV(ptmp[0], ptmp[1], ptmp[2], *pSrcY, *pSrcU, *pSrcV);
			ptmp+=(width*4);
			pSrcY+=width;
			if(i%2 == 1)
			{
				pSrcU+=(nDstPitch/2);
				pSrcV+=(nDstPitch/2);

			}

		}
	}
	return ;

}
*/
void YV12toRGB32C(BYTE *pSrc,BYTE *pDst,int width,int height)
{
	int h2 = height/2;
	int w2 = width/2;
	int p2 = width/2;

	BYTE *y = pSrc;
	BYTE *v = y +width * height;
	BYTE *u = v +width/2 * height/2;
	BYTE *pBmpTemp = pDst;

	for (int i = 0; i < height;++i)
	{
		BYTE *pSrcY = y + i * width;
		BYTE *pSrcU = u + (i/2) * p2;
		BYTE *pSrcV = v + (i/2) * p2;
		BYTE *pDst = pBmpTemp + width * 4 * i;

		BYTE *pEnd = pSrcY + width;
		for (;pSrcY < pEnd; pSrcY+=2)
		{
			int nr,ng,nb;
			YUV2RGB(nr,ng,nb,pSrcY[0],pSrcV[0],pSrcU[0]);
			if (nr > 255)
			{
				pDst[0] = 0xff;
			}
			else if (nr < 0)
			{
				pDst[0] = 0;
			}
			else
			{
				pDst[0] = (BYTE)nr;
			}

			if (ng > 255)
			{
				pDst[1] = 0xff;
			}
			else if (ng < 0)
			{
				pDst[1] = 0;
			}
			else
			{
				pDst[1] = (BYTE)ng;
			}

			if (nb > 255)
			{
				pDst[2] = 0xff;
			}
			else if (nb < 0)
			{
				pDst[2] = 0;
			}
			else
			{
				pDst[2] = (BYTE)nb;
			}

			pDst[3] = 0xff;
			YUV2RGB(nr,ng,nb,pSrcY[1],pSrcV[0],pSrcU[0]);
			if (nr > 0xff)
			{
				pDst[4] = 0xff;
			}
			else if (nr < 0)
			{
				pDst[4] = 0;
			}
			else
			{
				pDst[4] = (BYTE)nr;
			}

			if (ng > 0xff)
			{
				pDst[5] = 0xff;
			}
			else if (ng < 0)
			{
				pDst[5] = 0;
			}
			else
			{
				pDst[5] = (BYTE)ng;
			}

			if (nb > 0xff)
			{
				pDst[6] = 0xff;
			}
			else if (nb < 0)
			{
				pDst[6] = 0;
			}
			else
			{
				pDst[6] = (BYTE)nb;
			}

			pDst[7] = 0xff;
			pSrcU ++;
			pSrcV ++;
			pDst += 8;
		}
	}

	if(width % 2 == 1)
	{
		BYTE *y = pSrc;
		BYTE *v = y +width * height;
		BYTE *u = v +width/2 * height/2;
		BYTE *pBmpTemp = pDst +  width*4 - 4;
		BYTE *pSrcY = y + width -1;
		BYTE *pSrcU = u + p2 - 1;
		BYTE *pSrcV = v + p2 - 1;
		for (int i = 0; i < height;++i)
		{
			int nr,ng,nb;
			YUV2RGB(nr,ng,nb,pSrcY[0],pSrcV[0],pSrcU[0]);
			if (nr > 255)
			{
				pDst[0] = 0xff;
			}
			else if (nr < 0)
			{
				pDst[0] = 0;
			}
			else
			{
				pDst[0] = (BYTE)nr;
			}

			if (ng > 255)
			{
				pDst[1] = 0xff;
			}
			else if (ng < 0)
			{
				pDst[1] = 0;
			}
			else
			{
				pDst[1] = (BYTE)ng;
			}

			if (nb > 255)
			{
				pDst[2] = 0xff;
			}
			else if (nb < 0)
			{
				pDst[2] = 0;
			}
			else
			{
				pDst[2] = (BYTE)nb;
			}

			pDst[3] = 0xff;
			pBmpTemp+=(width*4);
			pSrcY+=width;
			if(i%2 == 0)
			{
				pSrcU+=p2;
				pSrcV+=p2;

			}

		}
	}
}

void RGB32toYV12C(BYTE *pSrc,   BYTE *pDst, int nSrcPitch,int nDstPitch, int width, int height)
{
	BYTE* py = pDst;
	width = ((width + 1) >>1) << 1;
	BYTE* pu = py + nDstPitch * height;
	BYTE* pv = pu + nDstPitch * height/4;
	BYTE* ptmp = pSrc;
	BYTE* ply = py;
	BYTE* plu = pu;
	BYTE* plv = pv;
	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j+=2)
		{
			//两个像素共用一个uv
			if(i % 2 == 1)
			{
				*py = (BYTE)(((int)30*ptmp[2] +(int)59*ptmp[1] +(int)11*ptmp[0])/100);
			}
			else
			{
				RGB2YUV(ptmp[0], ptmp[1], ptmp[2], (*py), (*pu), (*pv));
				pu++;
				pv++;
			}
			//后移一个像素
			ptmp += 4;
			py++;
			
			*py = (BYTE)(((int)30*ptmp[2] +(int)59*ptmp[1] +(int)11*ptmp[0])/100);
			py++;
			//后移一个像素
			ptmp += 4;
		}

		
		if(nDstPitch >= width)
		{
			py = ply + nDstPitch;
			ply = py;
			//两行公用uv
			if(i % 2 == 1)
			{
				pu = plu + nDstPitch/2;
				pv = plv + nDstPitch/2;
				plu = pu;
				plv = pv;
			}
		}
		
		pSrc += nSrcPitch;
		ptmp = pSrc;
	}
	return ;

}

void YV12toRGB32(BYTE *pSrc,BYTE *pDst,int width,int height)
{
	if(g_cpuType == CPU_SSE2)
	{
		if(width > 16)
		{
			YV12toRGB32MMX(pSrc,pDst,width,height);
		}
		else
		{
			YV12toRGB32C(pSrc,pDst,width,height);
		}	
	}
	else if(g_cpuType == CPU_MMX)
	{
		YV12toRGB32MMX(pSrc,pDst,width,height);
	}
	else
	{
		YV12toRGB32C(pSrc,pDst,width,height);
	}
}

#ifdef YV12toRGB32_SSE_CONVERT
void YV12toRGB32SSE(BYTE *pSrc,BYTE *pDst,int width,int height)
{
	typedef unsigned __int64  UInt64;
	//BYTE *pBmpTemp = pDst;
	BYTE *y = pSrc;
	BYTE *v = y +width * height;
	BYTE *u = v +width/2 * height/2;


	//B= 1.164 * (Y - 16) + 2.018 * (U - 128)
	//G= 1.164 * (Y - 16) -   0.38 * (U - 128) - 0.813 * (V - 128)
	//R= 1.164 * (Y - 16)                             + 1.159 * (V - 128)

	//	Y’ = 0.257*R' + 0.504*G' + 0.098*B' + 16
	//	Cb' = -0.148*R' - 0.291*G' + 0.439*B' + 128
	//	Cr' = 0.439*R' - 0.368*G' - 0.071*B' + 128
 
	//	R' = 1.164*(Y’-16) + 1.596*(Cr'-128)
	//	G' = 1.164*(Y’-16) - 0.813*(Cr'-128) - 0.392*(Cb'-128)
	//	B' = 1.164*(Y’-16) + 2.017*(Cb'-128)

	//	Y = 0.257*R + 0.504*G + 0.098*B + 16
	//	V'= -0.148*R - 0.291*G + 0.439*B + 128
	//	U = 0.439*R - 0.368*G - 0.071*B + 128

	//	R = 1.164*(Y-16) + 1.596*(U-128)
	//	G = 1.164*(Y-16) - 0.813*(U-128) - 0.392*(V-128)
	//	B = 1.164*(Y-16) + 2.017*(V-128)


	const  UInt64   csSSE_16_b[2]      = {0x1010101010101010, 0x1010101010101010}; // byte{16,16,16,16,16,16,16,16}
	const  UInt64   csSSE_128_w[2]     = {0x0080008000800080, 0x0080008000800080}; //short{  128,  128,  128,  128}
	const  UInt64   csSSE_0x00FF_w[2]  = {0x00FF00FF00FF00FF, 0x00FF00FF00FF00FF}; //掩码
	const  UInt64   csSSE_Y_coeff_w[2] = {0x2543254325432543, 0x2543254325432543}; //short{ 9539, 9539, 9539, 9539} =1.164383*(1<<13)
	const  UInt64   csSSE_U_blue_w[2]  = {0x408D408D408D408D, 0x408D408D408D408D}; //short{16525,16525,16525,16525} =2.017232*(1<<13)
	const  UInt64   csSSE_U_green_w[2] = {0xF377F377F377F377, 0xF377F377F377F377}; //short{-3209,-3209,-3209,-3209} =(-0.391762)*(1<<13)
	const  UInt64   csSSE_V_green_w[2] = {0xE5FCE5FCE5FCE5FC, 0xE5FCE5FCE5FCE5FC}; //short{-6660,-6660,-6660,-6660} =(-0.812968)*(1<<13)
	const  UInt64   csSSE_V_red_w[2]   = {0x3313331333133313, 0x3313331333133313}; //short{13075,13075,13075,13075} =1.596027*(1<<13)
	//const  unsigned __int64   csMMX_V_red_w   = 0x2516251625162516; //short{9494,9494,9494,9494} =1.1596027*(1<<13)

	int nLoop = width / 8;
	int p2 = width / 2; 

	//算法核心  把参数放大（1<<13倍）  Y  U  V 放大（1<3)倍。在两字节内运行完成。再通过交换，得到所要求的顺序

	for (int i = 0; i < height; ++i)
	{
		BYTE *pSrcY = y + i * width;
		BYTE *pSrcU = u + (i/2) * p2;
		BYTE *pSrcV = v + (i/2) * p2;
		BYTE *pBmpTemp = pDst + width*4 * i;

	__asm
		{
			mov ecx,nLoop;
			mov esi,pSrcY;
			mov edi,pBmpTemp;
			mov eax,pSrcV;
			mov edx,pSrcU;

	LOOP_begin:
			movdqa      xmm0,[esi];
			movq		xmm1,[eax];//pv
			movq		xmm2,[edx];//pu
			pxor		xmm4,xmm4;//清0
			punpcklbw	xmm1, xmm4;//0|v0|0|v1|0|v2... 把目的寄存器与源寄存器的低32位按字交错排列放入目的寄存器
			punpcklbw	xmm2, xmm4;//0|u0|0|u1|0|u2...

			psubusb     xmm0,csSSE_16_b		//y-16      源存储器与目的寄存器按字节对齐无符号饱和相减(目的减去源),内存变量必须对齐内存16字节
			psubsw      xmm1,csSSE_128_w     //u-128 (v-128, -128|v0-128|-128|v1-128|-128|v2-128...)
			movdqa      xmm7,xmm0             //py
			psubsw      xmm2,csSSE_128_w     //v-128 (u-128, -128|u0-128|-128|u1-128|-128|u2-128...) 源存储器与目的寄存器按字对齐有符号补码饱和相减(目的减去源),内存变量必须对齐内存16字节
			pand        xmm0,csSSE_0x00FF_w  //y6   y4  y2  y0 逻辑与
			psllw       xmm1,3                //放大u,把目的寄存器按字由源存储器(或imm8 立即数)指定位数逻辑左移,移出的位丢失
			psllw       xmm2,3                //放大v
			psrlw       xmm7,8               //0 y7  0 y5 0 y3 0 y1 把目的寄存器按字由源存储器(或imm8 立即数)指定位数逻辑右移,移出的位丢失
			movdqa      xmm3,xmm1              
			movdqa      xmm4,xmm2              

			pmulhw      xmm1,csSSEX_U_green_w		//g_u是4个16位数据的乘法,pmullw中是结果的低16位,pmulhw是结果的高16位
			psllw       xmm0,3                //放大y 按字逻辑左移3位
			pmulhw      xmm2,csMMX_V_green_w	  //g_v
			psllw       xmm7,3                //放大y
			pmulhw      xmm3,csMMX_U_blue_w[0]   //b_u
			paddsw      xmm1,xmm2              //g_uv 源存储器与目的寄存器按字对齐有符号补码饱和相加,内存变量必须对齐内存16字节
			pmulhw      xmm4,csMMX_V_red_w[0]    //r_v
			movq        xmm2,xmm3              //b_u
			pmulhw      xmm0,csMMX_Y_coeff_w  //pre_y  1
			movq        xmm6,xmm4              //r_v
			pmulhw      xmm7,csMMX_Y_coeff_w  //pre-y  2
			movq        xmm5,xmm1				 //g_uv 
			paddsw      xmm3,xmm0              //mm3:B1 -ok
			paddsw      xmm2,xmm7              //mm2:B2 -ok
			paddsw      xmm4,xmm0              //mm4:R1 -ok
			paddsw      xmm6,xmm7              //mm6:R2 -ok
			paddsw      xmm1,xmm0              //mm1:g1-ok
			paddsw      xmm5,xmm7              //mm5:g2-ok
			
			packuswb    xmm3,xmm4              //以下合起来部分
			packuswb    xmm2,xmm6              
			packuswb    xmm5,xmm1              
			movq        xmm4,xmm3              
			punpcklbw   xmm3,xmm2              
			punpckldq   xmm1,xmm5              
			punpckhbw   xmm4,xmm2              
			punpckhbw   xmm5,xmm1              


			pcmpeqb     xmm2,xmm2              

			movdqa        xmm0,xmm3              
			movdqa        xmm7,xmm4              
			punpcklbw   xmm0,xmm5              
			punpcklbw   xmm7,xmm2              
			movdqa        xmm1,xmm0              
			movdqa        xmm6,xmm3              
			punpcklwd   xmm0,xmm7              
			punpckhwd   xmm1,xmm7              
			movdqa		[edi],xmm0                 
			movdqa        xmm7,xmm4              
			punpckhbw   xmm6,xmm5              
			movdqa		[edi+16],xmm1               
			punpckhbw   xmm7,xmm2              
			movdqa        xmm0,xmm6              
			punpcklwd   xmm6,xmm7              
			punpckhwd   xmm0,xmm7              
			movdqa		[edi+16],xmm6              
			movdqa		[edi+24],xmm0 

			add esi,16;
			add edi,32;
			add eax,8;
			add edx,8;

			dec  ecx;
			jnz  LOOP_begin;
		}
	
		//int nRemain =  width % 8;
		//if (nRemain > 0)
		//{
		//	pSrcY = pSrcY + width - nRemain;
		//	pSrcU = pSrcU + width / 2 - nRemain/4;
		//	pSrcV = pSrcV + width / 2 - nRemain/4;
		//	pBmpTemp = pBmpTemp + nLoop * 8 * 4;
		//	for(int j = 0;j<nRemain;j++)
		//	{
		//		YUV2RGB(pBmpTemp[0],pBmpTemp[1],pBmpTemp[2],pSrcY[j],pSrcV[j/2],pSrcU[j/2]);
		//		Clip1(pBmpTemp[0]);
		//		Clip1(pBmpTemp[1]);
		//		Clip1(pBmpTemp[2]);
		//		pBmpTemp[3] == 0xff;
		//		pBmpTemp+=4;
		//	}
		//}
		
	}
	__asm emms
}
#endif

void YV12toRGB32MMX(BYTE *pSrc,BYTE *pDst,int width,int height)
{
	typedef unsigned __int64  UInt64;
	//BYTE *pBmpTemp = pDst;
	BYTE *y = pSrc;
	BYTE *v = y +width * height;
	BYTE *u = v +width/2 * height/2;


	//B= 1.164 * (Y - 16) + 2.018 * (U - 128)
	//G= 1.164 * (Y - 16) -   0.38 * (U - 128) - 0.813 * (V - 128)
	//R= 1.164 * (Y - 16)                             + 1.159 * (V - 128)

	//	Y’ = 0.257*R' + 0.504*G' + 0.098*B' + 16
	//	Cb' = -0.148*R' - 0.291*G' + 0.439*B' + 128
	//	Cr' = 0.439*R' - 0.368*G' - 0.071*B' + 128
 
	//	R' = 1.164*(Y’-16) + 1.596*(Cr'-128)
	//	G' = 1.164*(Y’-16) - 0.813*(Cr'-128) - 0.392*(Cb'-128)
	//	B' = 1.164*(Y’-16) + 2.017*(Cb'-128)

	//	Y = 0.257*R + 0.504*G + 0.098*B + 16
	//	V'= -0.148*R - 0.291*G + 0.439*B + 128
	//	U = 0.439*R - 0.368*G - 0.071*B + 128

	//	R = 1.164*(Y-16) + 1.596*(U-128)
	//	G = 1.164*(Y-16) - 0.813*(U-128) - 0.392*(V-128)
	//	B = 1.164*(Y-16) + 2.017*(V-128)


	const  UInt64   csMMX_16_b      = 0x1010101010101010; // byte{16,16,16,16,16,16,16,16}
	const  UInt64   csMMX_128_w     = 0x0080008000800080; //short{  128,  128,  128,  128}
	const  UInt64   csMMX_0x00FF_w  = 0x00FF00FF00FF00FF; //掩码
	const  UInt64   csMMX_Y_coeff_w = 0x2543254325432543; //short{ 9539, 9539, 9539, 9539} =1.164383*(1<<13)
	const  UInt64   csMMX_U_blue_w  = 0x408D408D408D408D; //short{16525,16525,16525,16525} =2.017232*(1<<13)
	const  UInt64   csMMX_U_green_w = 0xF377F377F377F377; //short{-3209,-3209,-3209,-3209} =(-0.391762)*(1<<13)
	const  UInt64   csMMX_V_green_w = 0xE5FCE5FCE5FCE5FC; //short{-6660,-6660,-6660,-6660} =(-0.812968)*(1<<13)
	const  UInt64   csMMX_V_red_w   = 0x3313331333133313; //short{13075,13075,13075,13075} =1.596027*(1<<13)
	//const  unsigned __int64   csMMX_V_red_w   = 0x2516251625162516; //short{9494,9494,9494,9494} =1.1596027*(1<<13)

	int nLoop = width / 8;
	int p2 = width / 2; 

	//算法核心  把参数放大（1<<13倍）  Y  U  V 放大（1<3)倍。在两字节内运行完成。再通过交换，得到所要求的顺序

	for (int i = 0; i < height; ++i)
	{
		BYTE *pSrcY = y + i * width;
		BYTE *pSrcU = u + (i/2) * p2;
		BYTE *pSrcV = v + (i/2) * p2;
		BYTE *pBmpTemp = pDst + width*4 * i;

	__asm
		{
			mov ecx,nLoop;
			mov esi,pSrcY;
			mov edi,pBmpTemp;
			mov eax,pSrcV;
			mov edx,pSrcU;

	LOOP_begin:
			movq        mm0,[esi];
			movd		mm1,[eax];
			movd		mm2,[edx];
			pxor		mm4,mm4;
			punpcklbw	mm1, mm4;
			punpcklbw	mm2, mm4;

			psubusb     mm0,csMMX_16_b		//y-16      
			psubsw      mm1,csMMX_128_w     //u-128 
			movq        mm7,mm0             
			psubsw      mm2,csMMX_128_w     //v-128  
			pand        mm0,csMMX_0x00FF_w  //y6   y4  y2  y0
			psllw       mm1,3                //放大u
			psllw       mm2,3                //放大v
			psrlw       mm7,8               //0 y7  0 y5 0 y3 0 y1 
			movq        mm3,mm1              
			movq        mm4,mm2              

			pmulhw      mm1,csMMX_U_green_w  //g_u
			psllw       mm0,3                //放大y
			pmulhw      mm2,csMMX_V_green_w  //g_v
			psllw       mm7,3                //放大y
			pmulhw      mm3,csMMX_U_blue_w   //b_u
			paddsw      mm1,mm2              //g_uv
			pmulhw      mm4,csMMX_V_red_w    //r_v
			movq        mm2,mm3              //b_u
			pmulhw      mm0,csMMX_Y_coeff_w  //pre_y  1
			movq        mm6,mm4              //r_v
			pmulhw      mm7,csMMX_Y_coeff_w  //pre-y  2
			movq        mm5,mm1				 //g_uv 
			paddsw      mm3,mm0              //mm3:B1 -ok
			paddsw      mm2,mm7              //mm2:B2 -ok
			paddsw      mm4,mm0              //mm4:R1 -ok
			paddsw      mm6,mm7              //mm6:R2 -ok
			paddsw      mm1,mm0              //mm1:g1-ok
			paddsw      mm5,mm7              //mm5:g2-ok
			
			packuswb    mm3,mm4              //以下合起来部分
			packuswb    mm2,mm6              
			packuswb    mm5,mm1              
			movq        mm4,mm3              
			punpcklbw   mm3,mm2              
			punpckldq   mm1,mm5              
			punpckhbw   mm4,mm2              
			punpckhbw   mm5,mm1              


			pcmpeqb     mm2,mm2              

			movq        mm0,mm3              
			movq        mm7,mm4              
			punpcklbw   mm0,mm5              
			punpcklbw   mm7,mm2              
			movq        mm1,mm0              
			movq        mm6,mm3              
			punpcklwd   mm0,mm7              
			punpckhwd   mm1,mm7              
			movq		[edi],mm0                 
			movq        mm7,mm4              
			punpckhbw   mm6,mm5              
			movq		[edi+8],mm1               
			punpckhbw   mm7,mm2              
			movq        mm0,mm6              
			punpcklwd   mm6,mm7              
			punpckhwd   mm0,mm7              
			movq		[edi+16],mm6              
			movq		[edi+24],mm0 

			add esi,8;
			add edi,32;
			add eax,4;
			add edx,4;

			dec  ecx;
			jnz  LOOP_begin;
		}
	/*
		int nRemain =  width % 8;
		if (nRemain > 0)
		{
			pSrcY = pSrcY + width - nRemain;
			pSrcU = pSrcU + width / 2 - nRemain/4;
			pSrcV = pSrcV + width / 2 - nRemain/4;
			pBmpTemp = pBmpTemp + nLoop * 8 * 4;
			for(int j = 0;j<nRemain;j++)
			{
				YUV2RGB(pBmpTemp[0],pBmpTemp[1],pBmpTemp[2],pSrcY[j],pSrcV[j/2],pSrcU[j/2]);
				Clip1(pBmpTemp[0]);
				Clip1(pBmpTemp[1]);
				Clip1(pBmpTemp[2]);
				pBmpTemp[3] == 0xff;
				pBmpTemp+=4;
			}
		}
		*/
	}
	__asm emms

	
//	边界处理
}




void CopyYV12Frame(BYTE* pDst,int nDstPitch,int nDstHeight,BYTE *y,BYTE *u,BYTE *v)
{
	if ((nDstPitch < 0)|| (nDstHeight < 0))
	{
		return;
	}

	//copy y

	BYTE *pDstY = pDst  ; 
	BYTE *pSrcY = y  ;
	int nPitchY = nDstPitch;
	int nHeightY = nDstHeight;
	for (int i = 0; i < nHeightY; ++i)
	{
		memcpy(pDstY,pSrcY,nPitchY);
		
		pDstY += nDstPitch;
		pSrcY += nDstPitch;
	}

	int nPitchUV = nPitchY / 2;
	int nHeightUV = nHeightY / 2;
	int nHalfSrcPic = nDstPitch / 2;
	int nHalfDstPic = nDstPitch / 2;


	//copy u		
	BYTE *pDstU = pDst + nDstPitch * nDstHeight;//this important;
	BYTE *pSrcU = u;

	for (int i = 0; i < nHeightUV; ++i)
	{
		memcpy(pDstU,pSrcU,nPitchUV);
		
		pDstU += nHalfDstPic;
		pSrcU += nHalfSrcPic;
	} 

	//copy v
	BYTE *pDstV =  pDst  + nDstPitch * nDstHeight * 5/4;//this important;
	BYTE *pSrcV =  v;
	for (int i = 0; i < nHeightUV; ++i)
	{
		memcpy(pDstV,pSrcV,nPitchUV);
		
		pDstV += nHalfDstPic;
		pSrcV += nHalfSrcPic;
	}
}	//}



CPUType g_cpuType;
