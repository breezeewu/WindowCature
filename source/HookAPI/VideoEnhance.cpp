#include "VideoEnhance.h"
#include <math.h>
#include <streams.h>
CVideoEnhance::CVideoEnhance(void)
{
	m_nBMPSize = 0;
	m_pBuffer  = NULL;
}

CVideoEnhance::~CVideoEnhance(void)
{
	if(m_pBuffer)
	{
		delete[] m_pBuffer;
		m_nBMPSize = 0;
	}
}

// 设置视频增强特效
HRESULT CVideoEnhance::SetEnhaceEffect(Enhance_Effect effect)
{
	m_eEnhanceEffect = effect;

	return S_OK;
}

//获取视频增强特效
HRESULT CVideoEnhance::GetEnhaceEffect(Enhance_Effect* peffect)
{
	if(peffect == NULL)
	{
		return E_POINTER;
	}

	*peffect = m_eEnhanceEffect;

	return S_OK;
}

// 视频增强
HRESULT CVideoEnhance::EnhaceVideo(BYTE* pdata,	BITMAPINFOHEADER* pbmp, int pixel)
{
	return S_OK;
	if(pbmp == NULL || pdata == NULL)
	{
		return E_POINTER;
	}

	if(m_nBMPSize < pbmp->biWidth * pbmp->biHeight * 3/2)
	{
		if(m_pBuffer)
		{
			delete[] m_pBuffer;
		}

		m_nBMPSize		= pbmp->biWidth * pbmp->biHeight * 3/2;
		m_pBuffer				= new BYTE[pbmp->biSizeImage];
	}

	CheckPointer(m_pBuffer, E_OUTOFMEMORY);

	int nWidth = pbmp->biWidth;
	int nHeight = pbmp->biHeight;


	nHeight = (nHeight>>1) << 1;

	RGB32toYV12SSE2(pdata,m_pBuffer, nWidth*4, nWidth, nWidth,nHeight) ;	
	EnhanceYV12(m_pBuffer,nWidth,nWidth>>1,nWidth,nHeight,nWidth);
	YV12toRGB32MMX(m_pBuffer,pdata,nWidth,nHeight);

	return S_OK;
}

void CVideoEnhance::RGB32toYV12SSE2(BYTE *src,   BYTE *dst, int srcrowsize,int destrowsize, int width, int height) 
{ 
	BYTE   *yp,   *up,   *vp; 
	BYTE   *prow; 
	int   i,   j ; 

	BYTE *y = dst;
	BYTE *u = y +destrowsize * height;
	BYTE *v = u +destrowsize/2 * height/2;

	__declspec(align(16))
		static short int ycoefs[8] = {2851,   22970,   6947,   0, 2851,   22970,   6947,   0 };
	__declspec(align(16))
		static short int ucoefs[8] =   {16384,   -12583,   -3801,   0, 16384,   -12583,   -3801,   0 };
	__declspec(align(16))
		static short int vcoefs[8] = {-1802,   -14582,   16384,   0, -1802,   -14582,   16384,   0 };

	_asm   
	{ 
		push eax
		push ebx
		push ecx
		push edx
		push edi
		push esi

		xor    edx,   edx 
		mov    eax,   width 
		sar    eax,2 
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
		mov    eax,   src 
		mov    prow,   eax 
		pxor    xmm7,   xmm7 
		mov    eax,   i 

heighttop:

		mov    i,   eax 
		mov    edi,   j 
		mov    ebx,   prow 
		mov    ecx,   yp 
		mov    edx,   up 
		mov    esi,   vp 

widthtop: 
		movdqu   xmm0,   [ebx]     //   MM5   has   0   r2   g2   b2   0   r1   g1   b1,   4   pixels  
		movq      xmm5,   xmm0 // xmm1  = 00..00 g3 b3 r2 g2 b2 r1 g1 b1
		pshufd     xmm0 ,xmm0 ,0x4e
		movq       xmm4,xmm0
		add           ebx,   16 

		punpcklbw   xmm5,   xmm7   //   MM5   has   0   r2   g2   b2 0   r1   g1   b1 
		punpcklbw   xmm4,   xmm7   //   MM4   has  0   r4   g4   b4 0   r3   g3   b3

		movdqa      xmm0,  xmm5 
		movdqa      xmm1,   xmm4 

		pmaddwd   xmm0,   ycoefs   //   MM0   has   r1*cr   and   g1*cg+b1*cb 
		movdqa      xmm2,   xmm0 
		PSRLQ     xmm2,   32 
		paddd     xmm0,   xmm2       //   MM0   has   y1 y2  in   lower   32   bits 
		pmaddwd   xmm1,   ycoefs   //   MM1   has   r2*cr   and   g2*cg+b2*cb 
		movdqa      xmm2,   xmm1 
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


		movdqa      xmm0,   xmm5 
		movdqa      xmm1,   xmm4 

		pmaddwd   xmm0,   ucoefs   //   MM0   has   r1*cr   and   g1*cg+b1*cb 
		movdqa      xmm2,  xmm0 
		PSRLQ     xmm2,   32 
		paddd     xmm0,   xmm2       //   MM0   has   u1   in   lower   32   bits 


		movd      eax,   xmm0 
		imul      eax,   224 
		sar       eax,   8 
		add       eax,   4210688 
		shr       eax,   15 
		mov       [edx],   al 
		inc       edx 
        
		movdqa      xmm0,   xmm5 
		pshufd     xmm0 ,xmm0 ,0x4e

		pmaddwd   xmm0,   vcoefs   //   MM0   has   r1*cr   and   g1*cg+b1*cb 
		movdqa      xmm2,  xmm0 
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
		movdqa     xmm2,   xmm1 
		PSRLQ     xmm2,   32 
		paddd     xmm1,   xmm2       //   MM5   has   v1   in   lower   32   bits 

		movd      eax,   xmm1 
		imul      eax,   224 
		sar       eax,   8 
		add       eax,   4210688 
		shr       eax,   15 
		mov       [edx],   al 
		inc       edx 
        
		movdqa      xmm1,   xmm4 
		pmaddwd   xmm1,   vcoefs   //   MM5   has   r1*cr   and   g1*cg+b1*cb 

		movdqa     xmm2,   xmm1 
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

		mov       eax,   destrowsize 
		add       yp,   eax 
		sar        eax,1

		test  i, 0x00000001
		jnz  L2

		add       up,   eax 
		add       vp,   eax 
L2:
		mov       eax,   srcrowsize 
		add       prow,   eax 
		mov       eax,   i 
		dec       eax 
		jnz       heighttop 

		pop esi
		pop edi
		pop edx
		pop ecx
		pop ebx
		pop eax
yuvexit:
		emms 
	} 
}

void CVideoEnhance::YV12toRGB32MMX(BYTE *src,BYTE *dst,int width,int height)
{
	typedef unsigned __int64  UInt64;
	BYTE *pBmpTemp = dst;
	BYTE *y = src;
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
		BYTE *pDst = pBmpTemp + width*4 * i;

	__asm{
		mov ecx,nLoop;
		mov esi,pSrcY;
		mov edi,pDst;
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
	}
	__asm emms

//	边界处理

}

void CVideoEnhance::EnhanceYV12(BYTE *src,int pitchY,int pitchUV,int width,int height,int len)
{
	BYTE *y = src;
	BYTE *v = y +width * height;
	BYTE *u = v +width/2 * height/2;
	for (int i = 0; i < height; i+=2)
	{
		BYTE *pY = NULL;
		BYTE *pY2 = NULL;
		BYTE *pU = NULL;
		BYTE *pV = NULL;

		pY = y +  pitchY * i;
		pY2 = pY + pitchY;
		pU = u + pitchUV * (i/2);
		pV = v +  pitchUV * (i/2);

		for (int j = 0;j <len; j += 2)
		{
			
			int D1,D2,D3,D4;
			int E,F;


			D1 = *pY ; 
			D2 = *(pY + 1) ;
			D3 = *pY2 ; 
			D4 = *(pY2 + 1) ;
			E = *pU - 128;
			F = *pV - 128;

			///测试视频增强
			*(pY + len ) = D1;
			*(pY + len + 1 ) = D2;
			*(pY2 + len ) = D3;
			*(pY2 + len + 1) = D4;
			*(pU + len/2 ) = E + 128;
			*(pV + len/2 ) = F + 128;
			///

			int diffY =  (int)(0.08 *E + 0.126 *F);
			//int diffY =  (int)(0.06 *E + 0.124 *F);

			D1 = D1  - diffY;
			D2 = D2  - diffY;
			D3 = D3  - diffY;
			D4 = D4  - diffY;

			//E = (int)(1.267 * E + 128);
			//F = (int)(1.267 * F + 128);
			E = (int)(1.287 * E + 128);
			F = (int)(1.287 * F + 128);

			Clip1(E);
			Clip1(F);

			Clip1(D1);
			Clip1(D2);
			Clip1(D3);
			Clip1(D4);

			if(j == (len - 1) || j == (len - 2)
				 || j == (len - 3) || j == (len - 4 ))
			{
				D1 = 0;
				D2 = 0;
				D3 = 0;
				D4 = 0;
				E = 128;
				F = 128;
			}

			D1 = GammaTable[D1];
			D2 = GammaTable[D2];
			D3 = GammaTable[D3];
			D4 = GammaTable[D4];


			*pY = D1;
			*(pY + 1) = D2;
			*pY2 = D3;
			*(pY2 + 1) = D4;

			*pU = E;
			*pV = F;

			pY += 2;
			pY2 += 2;
			++ pU;
			++ pV;
		}
	}
}

void CVideoEnhance::Gamma()
{
	int i;
	double AdjustedValue;
	for (i = 0;  i < 256; i++)
	{
		AdjustedValue = 255.0 * GetGammaAdjustedValue((double)(i-2) / (double)(255-2), 1.1/*(double)((70>>1)+1000) / 1000.0*/);
		//AdjustedValue = 255.0 * GetGammaAdjustedValue((double)(i-2) / (double)(255-2), (double)((50)+1000) / 1000.0);
		GammaTable[i] = (unsigned char)AdjustedValue;
	}
}

double CVideoEnhance::GetGammaAdjustedValue(double Input, double Gamma)
{
	if(Input <= 0.0)
	{
		return 0.0;
	}
	else if(Input >= 1.0)
	{
		return 1.0;
	}
	else
	{
		return pow(Input, 1.0 / Gamma);
	}
}