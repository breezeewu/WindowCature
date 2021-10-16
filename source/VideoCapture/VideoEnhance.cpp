#include "stdafx.h"
#include "VideoEnhance.h"
#include <math.h>
#include <Mmsystem.h>
#include <wxdebug.h>
#include "Utility.h"

#define  OPEN_GAMMA

#pragma comment( lib,"winmm.lib" )
CVideoEnhance::CVideoEnhance(void)
#ifdef COM_INTERFACE
					:CUnknown(L"VideoEnhance", NULL)
#endif
{
	//检测CPU类型
	CPUTest();
	int i;
	double AdjustedValue = 0;
	for (i = 0;  i < 256; i++)
	{
		AdjustedValue = 255.0 * GetGammaAdjustedValue((double)(i-2) / (double)(255-2), 1.08);
		GammaTable[i] = (unsigned char)AdjustedValue;
	}

	for (i = 0;  i < 256; i++)
	{
		AdjustedValue = 255.0 * GetGammaAdjustedValue((double)(i-2) / (double)(255-2), 0.887);
		darkGammaTable[i] = (unsigned char)AdjustedValue;
	}
	m_frameCount = 0;
	m_speed = 0;

	m_start = 0;
	m_duration = 0;

	m_pYV12Image		= NULL;
	m_pRGBAlign			= NULL;
	m_pEffectbuf		= NULL;
	m_nImageSize		= 0;
	m_nRGBSize		= 0;
	m_nWidth			= 0;
	m_nHeight			= 0;
	m_brushWidth		= 0;
	m_brushHeight		= 0;
	m_eEnhanceEffect = Effect_Rolling;
}

CVideoEnhance::~CVideoEnhance(void)
{
	if(m_pYV12Image)
	{
		delete[] m_pYV12Image;
		m_pYV12Image = NULL;
	}

	if(m_pRGBAlign)
	{
		//销毁原来内存
		delete[] m_pRGBAlign;
		m_pRGBAlign = NULL;
	}

	m_nImageSize = 0;
	m_nRGBSize = 0;
}

#ifdef COM_INTERFACE
STDMETHODIMP CVideoEnhance::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
	//检测输入指针的合法性
	CheckPointer(ppv, E_POINTER);

	if(__uuidof(IUnknown) == riid)
	{
		*ppv = static_cast<IUnknown*>(this);
		AddRef();
		return S_OK;
	}
	else if(__uuidof(IVideoEnhance) == riid)
	{
		*ppv = static_cast<IVideoEnhance*>(this);
		AddRef();
		return S_OK;
	}

	return __super::NonDelegatingQueryInterface(riid, ppv);
}
#endif
HRESULT CVideoEnhance::SetEnhaceEffect(Enhance_Effect effect)
{
	m_eEnhanceEffect = effect;
	return S_OK;
}

HRESULT CVideoEnhance::GetEnhaceEffect(Enhance_Effect* peffect)
{
	CheckPointer(peffect, E_POINTER);

	*peffect = m_eEnhanceEffect;

	return S_OK;
}

HRESULT CVideoEnhance::EnhaceVideo(BYTE* pdata,	BITMAPINFOHEADER* pbmp, int pixel)
{
	CheckPointer(pdata, E_POINTER);
	CheckPointer(pbmp,  E_POINTER);
	ASSERT(pbmp->biBitCount == 32);
	//MessageBox(0,0,0,0);
	/*EnhanceRGB32(pdata,	pbmp, pbmp->biWidth/2);*/
	/*memset(pdata, 0, abs(pbmp->biHeight) * pbmp->biWidth * 4);*/
	//return S_OK;
	/*if(pbmp->biWidth%2 == 1)
	{
		return S_OK;
	}*/
	int biWidth = (abs(pbmp->biWidth)+15)>>4<<4;
	int biHeight = (abs(pbmp->biHeight) + 15)>>4<<4;
	int yv12size = biWidth * biHeight * 3 / 2 + 16;
	if(yv12size > m_nImageSize)
	{
		if(m_pYV12Image)
		{
			//销毁原来内存
			delete[] m_pYV12Image;
			m_pYV12Image = NULL;
			m_nImageSize = 0;
		}

		//重新分配内存
		m_pYV12Image = new BYTE[yv12size];

		CheckPointer(m_pYV12Image, E_OUTOFMEMORY);

		m_nImageSize = yv12size;
		
	}
	
	m_nWidth			= abs(pbmp->biWidth);
	m_nHeight			= abs(pbmp->biHeight);

	int alignW = m_nWidth % 16;
	int alignH = m_nHeight % 2;
	alignW = 16 - alignW;

	if(1/*(m_nWidth % 2) == 1 || (m_nHeight % 2) == 1*/)
	{
		unsigned int alignSize = (m_nWidth+alignW)*4*(m_nHeight+alignH);
		if(alignSize > m_nRGBSize)
		{
			if(m_pRGBAlign)
			{
				//销毁原来内存
				delete[] m_pRGBAlign;
				m_pRGBAlign = NULL;
				m_nRGBSize = 0;
			}
			//重新分配内存
			m_pRGBAlign = new BYTE[alignSize];
			CheckPointer(m_pRGBAlign, E_OUTOFMEMORY);
			m_nRGBSize = alignSize;	
		}

		memset(m_pRGBAlign,0,(m_nWidth+alignW)*4*(m_nHeight+alignH));
		BYTE *pRGBAlign = m_pRGBAlign;
		BYTE *pRGB = pdata;

		for(int i =0;i<(m_nHeight);i++)
		{
			CopyMemory(pRGBAlign,pRGB,m_nWidth*4);
			pRGBAlign += ((m_nWidth+alignW)*4);
			pRGB += m_nWidth*4;
		}
		RGB32toYV12C(m_pRGBAlign, m_pYV12Image,(m_nWidth+alignW) * 4, m_nWidth+alignW, m_nWidth+alignW, m_nHeight+alignH);
		Enhance(m_pYV12Image, m_nWidth+alignW, (m_nWidth+alignW)>>1, m_nWidth+alignW, m_nHeight+alignH,1);
		YV12toRGB32(m_pYV12Image,m_pRGBAlign,m_nWidth+alignW,m_nHeight+alignH);
		pRGB = pdata;
		pRGBAlign = m_pRGBAlign;
		for(int i =0;i<m_nHeight;i++)
		{
			CopyMemory(pRGB,pRGBAlign,m_nWidth*4);
			pRGBAlign += ((m_nWidth+alignW)*4);
			pRGB += m_nWidth*4;
		}
	}
	else
	{
		RGB32toYV12C(pdata, m_pYV12Image,m_nWidth * 4, m_nWidth, m_nWidth, m_nHeight);
		//视频增强
		Enhance(m_pYV12Image, m_nWidth, m_nWidth>>1, m_nWidth, m_nHeight,0);
		YV12toRGB32C(m_pYV12Image,pdata,m_nWidth,m_nHeight);
	}
	
	return S_OK; 
}

HRESULT CVideoEnhance::Reset()
{
	m_frameCount = 0;
	m_speed = 0;
	m_start = 0;
	m_duration = 0;
	if(m_pEffectbuf)
	{
		delete [] m_pEffectbuf;
		m_pEffectbuf = NULL;
	}
	return S_OK;
}

HRESULT CVideoEnhance::SetEffectSize(int width,int height)
{
	m_brushWidth = width;
	int alignW = m_brushWidth % 16;
	alignW = 16 - alignW;
	m_brushWidth = m_brushWidth + alignW;
	m_brushHeight = height;
	return S_OK;
}


void CVideoEnhance::DestroyInstance()
{
	delete this;
}

void CVideoEnhance::Enhance(BYTE *pBuffer,int pitchY,int pitchUV,int width,int height,int bAsm)
{
	BYTE *y = pBuffer;
	BYTE *u = y +width * height;
	BYTE *v = u +width/2 * height/2;

	if (g_cpuType == CPU_SSE2)
	{
		if(m_speed == -4)
		{
			if(bAsm && width > 16)
			{
				Enhance_SSE2(y,v,u,pitchY,pitchUV,width,height);
			}
			else
			{
				Enhance_C(y,v,u,pitchY,pitchUV,width,height,width,0);
			}	
			//USM_SSE2(y,width,height,pitchY,55,6);
			m_frameCount = 0;
		}
		else
		{
			if(width != m_brushWidth)
			{
				return ;
			}	
			m_duration = timeGetTime();
			if(m_frameCount >= 30 && (m_duration - m_start) > 2000)
			{
				m_speed+=4;
				if(m_speed >= width )
				{
					m_speed = -4;
					m_frameCount = 0;
					Enhance_C(y,u,v,pitchY,pitchUV,width,height,width,0);
					//USM_C(y,width,height,pitchY,55,6, width);
				}
				else
				{
					/*
					if(m_pEffectbuf == NULL)
					{
						m_pEffectbuf = new BYTE[width*274*2];
						memset(m_pEffectbuf,0,width*274*2);
						CopyYV12Frame(m_pEffectbuf,width,274,y,u,v);
					}

					BYTE *y1,*u1,*v1;
					y1 = m_pEffectbuf;
					u1 = y1 + width * 274;
					v1 = u1 + width / 2 * 274 / 2; 
					
					BYTE *y0 = y;
					BYTE *u0 = u;
					BYTE *v0 = v;

					for(int i = 0;i<274;i++)
					{
						memcpy(y0+m_speed+i*width,y1+m_speed+i*width,width-m_speed);
					}

					for(int i = 0;i < 274/2;i++)
					{
						memcpy(u0 + m_speed/2 + i*pitchUV,u1 + m_speed/2 + i*pitchUV,(width-m_speed)/2);
						memcpy(v0 + m_speed/2 + i*pitchUV,v1 + m_speed/2 + i*pitchUV,(width-m_speed)/2);
					}
*/
					Enhance_dark(y,u,v,pitchY,pitchUV,width,height,m_speed);
					Enhance_C(y,u,v,pitchY,pitchUV,width,height,m_speed,0);
					//USM_C(y,width,height,pitchY,55,6, m_speed);
				}
			}
			else
			{
				m_speed+=4;
				if(m_speed >= (width>>1))
				{		
					Enhance_dark(y,u,v,pitchY,pitchUV,width,height,width>>1);
					Enhance_C(y,u,v,pitchY,pitchUV,width,height,width>>1,0);
					//USM_C(y,width,height,pitchY,55,6,width>>1);		
					if(m_frameCount==0)
					{
						m_start = timeGetTime();
					}
					m_frameCount++;
					m_speed = width>>1;
				}
				else
				{
					Enhance_dark(y,u,v,pitchY,pitchUV,width,height,m_speed);
					Enhance_C(y,u,v,pitchY,pitchUV,width,height,m_speed,0);
				}			
			}

			
		}
	}
	else if (g_cpuType == CPU_MMX)
	{
		Enhance_MMX(y,v,u,pitchY,pitchUV,width,height);
	}
	else if(g_cpuType == CPU_UNKNOW)
	{
		Enhance_C(y,v,u,pitchY,pitchUV,width,height,width,0);
	}
}
void CVideoEnhance::USM(BYTE *pBuffer,int const sharpen ,int const nThreshold,int pitchY,int width,int height)
{
	if (g_cpuType == CPU_SSE2)
	{
		USM_SSE2(pBuffer,width,height,pitchY,sharpen,nThreshold);
	}
	else if (g_cpuType == CPU_MMX)
	{
		USM_MMX(pBuffer,width,height,pitchY,sharpen,nThreshold);
	}
	else if(g_cpuType == CPU_UNKNOW)
	{
		USM_C(pBuffer,width,height,pitchY,sharpen,nThreshold,width);
	}
}


#define Clip1(x) if((x) < 0){x = 0;} if((x) > 255){x = 255;}
void CVideoEnhance::Enhance_C(BYTE *y,BYTE*u,BYTE *v,int pitchY,int pitchUV,int width,int height,int len,int bValid)
{
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

		for (int j = 0;j < len; j += 2)
		{
			int D1,D2,D3,D4;
			int E,F;

			D1 = *pY ; 
			D2 = *(pY + 1) ;
			D3 = *pY2 ; 
			D4 = *(pY2 + 1) ;
			E = *pU - 128;
			F = *pV - 128;

			int diffY =  (int)(0.06 *E + 0.124 *F);

			D1 = D1  - diffY;
			D2 = D2  - diffY;
			D3 = D3  - diffY;
			D4 = D4  - diffY;

			E = (int)(1.2 * E + 128);
			F = (int)(1.16 * F + 128);


			Clip1(E);
			Clip1(F);

			Clip1(D1);
			Clip1(D2);
			Clip1(D3);
			Clip1(D4);
			

			if((len <= width - 4) || (len > width + 2)  )
			{
				if(j == (len - 1) || j == (len - 2))
				{
					D1 = D1+10;
					D2 = D2+10;
					D3 = D3+10;
					D4 = D4+10;
					E = E+2;
					F = F+2;
					Clip1(D1);
					Clip1(D2);
					Clip1(D3);
					Clip1(D4);
					Clip1(E);
					Clip1(F);
				}
				if( j == len || j == (len + 1))
				{
					D1 = D1-10;
					D2 = D2-10;
					D3 = D3-10;
					D4 = D4-10;
					E = E-2;
					F = F-2;
					Clip1(D1);
					Clip1(D2);
					Clip1(D3);
					Clip1(D4);
					Clip1(E);
					Clip1(F);
				}
			}

#ifdef OPEN_GAMMA
			D1 = GammaTable[D1];
			D2 = GammaTable[D2];
			D3 = GammaTable[D3];
			D4 = GammaTable[D4];
#endif 
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

void CVideoEnhance::Enhance_dark(BYTE *y,BYTE*u,BYTE *v,int pitchY,int pitchUV,int width ,int height,int len )
{
	BYTE *pY = NULL;
	BYTE *pY2 = NULL;
	BYTE *pU = NULL;
	BYTE *pV = NULL;
	for (int i = 0; i < height; i+=2)
	{
		pY = y +  pitchY * i ;
		pY2 = pY + pitchY  ;
		pU = u + pitchUV * (i/2);
		pV = v +  pitchUV * (i/2);
		for (int j = len;j < width ; j+=2)
		{
			int D1,D2,D3,D4;
			int E,F;
			D1 = pY[j] ; 
			D2 = pY[j+1];
			D3 = pY2[j] ; 
			D4 = pY2[j+1] ;

#ifdef OPEN_GAMMA
			D1 = darkGammaTable[D1];
			D2 = darkGammaTable[D2];
			D3 = darkGammaTable[D3];
			D4 = darkGammaTable[D4];
#endif 
			Clip1(D1);
			Clip1(D2);
			Clip1(D3);
			Clip1(D4);

			pY[j] = D1;
			pY[j+1] = D2;
			pY2[j] = D3;
			pY2[j+1] = D4;
		}
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

void CVideoEnhance::EnhanceRGB32(BYTE* pdata,	BITMAPINFOHEADER* pbmp, int pixel)
{
	RBGA* prgba = NULL;
	prgba = (RBGA*)pdata;
	BYTE* pNewline = pdata;
	for(int j = 0; j < abs(pbmp->biHeight); j++)
	{
		prgba = (RBGA*)pNewline;
		for(int i = 0; i < pbmp->biWidth; i++)
		{
			//增强
			int dur = prgba->r*2 - prgba->g - prgba->b;
			if(dur > 0)
			{
				dur = ENHANCE_PARAM * dur + prgba->r;
				prgba->r = dur > 255 ? 255 : dur;
			}

			if(dur = prgba->g*2 - prgba->r - prgba->b > 0)
			{
				dur = ENHANCE_PARAM * dur + prgba->g;
				prgba->g = dur > 255 ? 255 : dur;
				//prgba->g += ENHANCE_PARAM * (prgba->g*2 - prgba->r - prgba->b);
			}

			if(dur = prgba->b*2 - prgba->r - prgba->g > 0)
			{
				dur = ENHANCE_PARAM * dur + prgba->b;
				prgba->b = dur > 255 ? 255 : dur;
				//prgba->b += ENHANCE_PARAM * (prgba->b*2 - prgba->r - prgba->g);
			}

			//亮度调节
			int R = prgba->r + 0.299*255*0.05;
			int G = prgba->g + 0.587*255*0.05;
			int B = prgba->b + 0.114*255*0.05;

			
			prgba->r = R > 255 ? 255 : R;
			prgba->g = G > 255 ? 255 : G;
			prgba->b = B > 255 ? 255 : B;
			

			if(i+1 >= pixel)
			{
				break;
			}
			prgba++;
		}
		pNewline += pbmp->biWidth * 4;
	}
}

__declspec(align(16)) const __int64 MMX_006[2]  = {0x01EB01EB01EB01EB,0x01EB01EB01EB01EB};//0.060 
__declspec(align(16)) const __int64 MMX_124[2]  = {0x03F803F803F803F8,0x03F803F803F803F8};//0.124	  	
__declspec(align(16)) const __int64 MMX_120[2] = {0x2667266726672667,0x2667266726672667};//1.267
__declspec(align(16)) const __int64 MMX_116[2] = {0x251f251f251f251f,0x251f251f251f251f};//1.267
__declspec(align(16)) const __int64 INT_128[2]  = {0x0080008000800080,0x0080008000800080};
__declspec(align(16)) const __int64 MASK_Y[2]   = {0x00ff00ff00ff00ff,0x00ff00ff00ff00ff};
__declspec(align(16)) const __int64 MASK_UV[2]  = {0xff00ff00ff00ff00,0xff00ff00ff00ff00};
__declspec(align(16))const __int64 MMX_006_00[2]  = {0x01EB000001EB0000,0x01EB000001EB0000};//0.060 
__declspec(align(16))const __int64 MMX_124_00[2]  = {0x03F8000003F80000,0x03F8000003F80000};//0.124	 

void CVideoEnhance::Enhance_MMX(BYTE *y,BYTE*u,BYTE *v,int pitchY,int pitchUV,int width,int height)
{
	int count = width/8;
	if (count < 1)
	{
		return;
	}

	//int pitchY = pitchY;

	for (int i = 0; i < height; i+=2)
	{
		BYTE *pY = NULL;
		BYTE *pU = NULL;
		BYTE *pV = NULL;

		pY = y +  pitchY * i;
		pU = u +  pitchUV * (i/2);
		pV = v +  pitchUV * (i/2);

		_asm
		{
			mov ecx,count;

			mov esi,pU;
			mov edi,pV;
			mov edx,pY;
			mov eax,edx;
			add eax,pitchY
LOOP1:
			movd mm0,[esi];
			movd mm2,[edi];

			pxor mm7,mm7;

			punpcklbw mm0,mm7;
			punpcklbw mm2,mm7;

			movq	 mm6,INT_128
			psubw    mm0,mm6;
			psubw    mm2,mm6;

			movd     mm6,[edx];

			psllw       mm0,3
			psllw       mm2,3

			movq     mm1,mm0;
			movq     mm3,mm2;

			//0.06 *E
			//0.124 *F;
			pmulhw    mm0,MMX_006
			pmulhw	  mm2,MMX_124	

			//0.06 *E + 0.124 *F;
			paddsw   mm0,mm2;

			punpcklbw mm6,mm7
			psubsw  mm6,mm0;
			packuswb mm6,mm7;
			movd   [edx],mm6;

			movd     mm6,[edx+4];
			punpcklbw mm6,mm7
			psubsw  mm6,mm0;
			packuswb mm6,mm7;
			movd   [edx+4],mm6;

			movd   mm5,[eax]//第一行Y

			pmulhw    mm1,MMX_120
			pmulhw	  mm3,MMX_116	
			paddsw mm1,INT_128
			paddsw mm3,INT_128

			punpcklbw mm5,mm7
			psubsw  mm5,mm0;
			packuswb mm5,mm7;
			movd   [eax ],mm5//第二行Y

			movd   mm5,[eax+4]
			punpcklbw mm5,mm7
			psubsw  mm5,mm0;
			packuswb mm5,mm7;
			movd   [eax +4],mm5//第二行Y

			packuswb mm1,mm7;
			packuswb mm3,mm7;


			movd [esi],mm1;
			movd [edi],mm3;

			add esi,4;
			add edi,4;
			add edx,8
			add eax,8

			dec  ecx;
			jnz  LOOP1;

			emms
		}
	}

	int nRemain =  width % 8;
	if (nRemain > 0)
	{
		BYTE *pY = y + pitchY - nRemain;
		BYTE *pU = u + pitchUV - nRemain/2;
		BYTE *pV = v + pitchUV - nRemain/2;;
		Enhance_C(pY,pU,pV,pitchY,pitchUV,nRemain,height,nRemain,0);
	}
#ifdef OPEN_GAMMA
	for (int i = 0; i < height; ++i)
	{
		BYTE *p = y + i * pitchY;
		int ySize = (width >>4)<<4;
		for (int j = 0; j < ySize; ++j)
		{
			*p = GammaTable[*p];
			++p;
		}
	}
#endif
}


void CVideoEnhance::Enhance_SSE2(BYTE *y,BYTE*u,BYTE *v,int pitchY,int pitchUV,int width,int height)
{
	int count = width /16;//32对齐 16每循环
	if (count < 1)
	{
		return;
	}

	//int pitchY = pitchY;
	short * Table= (short*)GammaTable;

	for (int i = 0; i < height; i+=2)
	{
		BYTE *pY = NULL;
		BYTE *pU = NULL;
		BYTE *pV = NULL;

		pY = y +  pitchY * i;
		pU = u +  pitchUV * (i/2);
		pV = v +  pitchUV * (i/2);

		_asm
		{
			mov ecx,count;

			mov esi,pU;
			mov edi,pV;
			mov edx,pY;
			mov eax,edx;
			add eax,pitchY
LOOP1:
			movq	mm0,[esi]		//pu
			movq	mm2,[edi]		//pv

			movq2dq xmm0,mm0;		//pu 把源寄存器内容送入目的寄存器的低64位,高64位清零
			movq2dq xmm2,mm2;		//pv 把源寄存器内容送入目的寄存器的低64位,高64位清零

			pxor xmm7,xmm7;			//按位异或操作,置零

			punpcklbw xmm0,xmm7;	//把目的寄存器与源寄存器的低32位按字节交错排列放入目的寄存器目的寄存器
			punpcklbw xmm2,xmm7;	//a0|a1|a2|a3|a4|a5|a6|a7 源寄存器: b0|b1|b2|b3|b4|b5|b6|b7 目的寄存器处理结果：b4|a4|b5|a5|b6|a6|b7|a7

			movdqa	 xmm6,INT_128	//将双四字从源操作数（第二个操作数）移到目标操作数（第一个操作数）
			psubw    xmm0,xmm6;		//源存储器与目的寄存器按字对齐无符号普通相减(目的减去源),内存变量必须对齐内存16字节
			psubw    xmm2,xmm6;

			movq     mm0,[edx]
			
			psllw    xmm0,3
			psllw    xmm2,3

			movdqa     xmm1,xmm0;
			movdqa     xmm3,xmm2;

			//0.06 *E
			//0.124 *F;
			pmulhw    xmm0,MMX_006
			pmulhw	  xmm2,MMX_124	
			//0.06 *E + 0.124 *F;
			paddsw   xmm0,xmm2;

			movq     mm1,[edx + 8]
			movq2dq  xmm6,mm0;
			punpcklbw xmm6,xmm7
			psubsw   xmm6,xmm0;
			packuswb xmm6,xmm7
			movdq2q   mm0,xmm6;
			movq      [edx],mm0;
			
			movq2dq  mm6,mm1;
			punpcklbw xmm6,xmm7
			psubsw   xmm6,xmm0;
			packuswb xmm6,xmm7
			movdq2q   mm1,xmm6;
			movq      [edx + 8],mm1;

			pmulhw    xmm1,MMX_120
			pmulhw	  xmm3,MMX_116	
			movq     mm0,[eax]
			paddsw	  xmm1,INT_128
			paddsw    xmm3,INT_128

			movq     mm1,[eax + 8]
			movq2dq  mm5,mm0;
			punpcklbw xmm5,xmm7
			psubsw   xmm5,xmm0;
			packuswb xmm5,xmm7
			movdq2q   mm0,xmm5;
			movq   [eax],mm0;
			
			movq2dq   xmm5,mm1
			punpcklbw xmm5,xmm7
			psubsw   xmm5,xmm0;
			packuswb xmm5,xmm7
			movdq2q mm1,xmm5;
			movq   [eax + 8],mm1;

			packuswb xmm1,xmm7;
			packuswb xmm3,xmm7;

			movdq2q mm1,xmm1;
			movdq2q mm3,xmm3;

			movq [esi],mm1;
			movq [edi],mm3;

			add esi,8;
			add edi,8;
			add edx,16
			add eax,16

			dec  ecx;
			jnz  LOOP1;

			emms
		}
		//int nRemain =  width % 16;
		//if (nRemain > 0)
		//{
		//	pY = pY + count * 16;
		//	pU = u + count * 8;
		//	pV = v + pitchUV - nRemain/2;;
		//	Enhance_C(pY,pU,pV,pitchY,pitchUV,nRemain,height);
		//}
	}

	int nRemain =  width % 16;
	if (nRemain > 0)
	{
		BYTE *pY = y + pitchY - nRemain;
		BYTE *pU = u + pitchUV - nRemain/2;
		BYTE *pV = v + pitchUV - nRemain/2;;
		Enhance_C(pY,pU,pV,pitchY,pitchUV,nRemain,height,nRemain,0);
	}

 #ifdef OPEN_GAMMA
	for (int i = 0; i < height; ++i)
	{
		BYTE *p = y + i * pitchY;
		int ySize = (width >>4)<<4;
		for (int j = 0; j < pitchY; ++j)
		{
			*p = GammaTable[*p];
			++p;
		}
	}
#endif

}

bool CVideoEnhance::USM_C(BYTE *pBuffer,int w,int h,int p,int const sharpen,int const nThreshold,int len)
{
	int threshold = nThreshold;
	float count = (float)sharpen / 256 ;

	const int offset = 1;
	const int rowsize = w - 1;	

	BYTE *oldBuf = new BYTE[len];
	BYTE *newBuf =   new BYTE[len];

	memcpy(oldBuf,pBuffer,len);
	for(int y = 1; y < h - 1; y++)//必须从1开始
	{
		BYTE *curRow = pBuffer + y * p;
		BYTE *lastRow = oldBuf;
		BYTE *nextRow = curRow + p;

		memcpy(newBuf,curRow,len);//把当前行保存起来

		for(int x = offset ; x < len - 1; ++x)
		{
			int value =		  lastRow[x] 
			+ curRow[x-1]   + curRow[x]   + curRow[x+1]
			+ nextRow[x];

			int nCur = curRow[x] * 5;
			int diff =  nCur - value;
			
			if(diff > 0)
			{
				if(diff > threshold)
				{
					int maxValue =  curRow[x] + (int)(count * diff);
					curRow[x]  = (BYTE)min(255,maxValue);
				}
			}
			else if(diff < 0)
			{
				diff = - diff;
				if(diff > threshold)
				{
					int maxValue =  max(curRow[x] - (int)(count * diff),0);
					curRow[x]  = (BYTE)maxValue;
				}
			}
			else
			{

			}
		}
		//指针交换
		BYTE *pTemp = newBuf;
		newBuf = oldBuf;
		oldBuf = pTemp;
	}
	delete []newBuf;
	delete []oldBuf;

	return true;
}

void CVideoEnhance::USM_MMX(BYTE* pBuffer
							, int const nWidth
							, int const nHeight
							, int  const nPitch
							, int const sharpen
							, int const nThreshold)

{
	if (nPitch < nWidth)
	{
		ASSERT(nPitch >= nWidth);
		return ;
	}

	const int cycle = nPitch / 4 - 1;
	static   const __int64   qwTimes = 0x0005000500050005;

	__int64   threshold  = nThreshold;
	__int64   qwSharpness = sharpen;

	threshold |= threshold << 48 | threshold << 32 | threshold << 16;
	qwSharpness |= qwSharpness << 48 | qwSharpness << 32 | qwSharpness << 16;

	BYTE *oldBuf = (BYTE*)_aligned_malloc(nPitch,16);
	BYTE *newBuf = (BYTE*)_aligned_malloc(nPitch,16);

	int offset = 4;
	memcpy(oldBuf,pBuffer+offset,nWidth);
	for(int row = 2; row < nHeight - 2; row ++)
	{
		BYTE *pLine2 = pBuffer + row * nPitch + offset;
		BYTE *pLine1 = oldBuf ;
		BYTE *pLine3 = pLine2 + nPitch ;

		memcpy(newBuf,pLine2,nWidth);//把当前行保存起来
		__asm
		{
			mov esi,pLine2;
			mov eax,pLine1;
			mov edx,pLine3;
			mov ecx,cycle;

			movq mm7,qwSharpness;
			movq mm6,threshold;
			pxor mm0,mm0;
			movd mm1,[esi - 4];
			punpcklbw mm1, mm0
LOOP1:
			//mm5  
//  mm 3    mm1    mm4
			//mm2
			movq mm3,mm1;
			movd mm1,[esi]; //mm1 curRow
			movd mm2,[edx];
			movq mm4,[esi + 4];
			punpcklbw mm1, mm0;
			punpcklbw mm2, mm0;
			punpcklbw mm4, mm0;

			//////////
			movq  mm5,mm1;

			psrlq mm3,48;
			psllq mm5,16;

			por   mm3,mm5;

			movq   mm5,mm1;
			psllq  mm4,48;
			psrlq  mm5,16;
			por    mm4,mm5;

			movd mm5,[eax];

			paddw mm2,mm1;
			paddw mm3,mm4;
			paddw mm3,mm2;
			punpcklbw mm5, mm0;
			paddw mm3,mm5;//mm3 destRow

			movq  mm5,mm1;

			pmullw  mm5,qwTimes; // 放大curRow
			movq  mm4,mm5;//mm1 anothor currow;

			psubw mm4,mm3; // curRow - destRow
			movq  mm2,mm4;
			pcmpgtw mm2,mm6
			pand  mm4,mm2;//大于0的值
			pmullw mm4,mm7;
			psrlw  mm4,8;			

			psubw mm3,mm5; //   destRow - curRow
			movq   mm2,mm3;
			pcmpgtw mm2,mm6
			pand   mm3,mm2;
			pmullw mm3,mm7;
			psrlw   mm3,8;

			movq     mm2,mm1;
			paddusb  mm2,mm4;  //diff * count + curRow  saturate
			psubusb  mm2,mm3;
			packuswb mm2, mm2
			movd [esi],mm2

			add  eax,4;
			add  esi,4;
			add  edx,4;

			dec  ecx;
			jnz  LOOP1
			emms
		}
		
		//指针交换
		BYTE *pTemp = newBuf;
		newBuf = oldBuf;
		oldBuf = pTemp;
	}
	_aligned_free(oldBuf);
	_aligned_free(newBuf);

	USM_C(pBuffer,4,nHeight,nPitch,sharpen, nThreshold,4);
	USM_C(pBuffer + nPitch -4,4,nHeight,nPitch,sharpen, nThreshold,4);
}


void CVideoEnhance::USM_SSE2(BYTE *pBuffer
	, int const nWidth
	, int const nHeight
	, int  const nPitch
	, int const sharpen
	, int const nThreshold)

{
	const int cycle = nPitch / 8 - 2;
	if (cycle <= 0)
	{
		return;
	}

	__declspec(align(16))
		static   const __int64   qwTimes[2] = {0x0005000500050005,0x0005000500050005};

	__declspec(align(16))
		__int64   threshold[2]  = {nThreshold};

	__declspec(align(16))
		__int64 qwSharpness[2] = {sharpen};

	threshold[0] |= threshold[0] << 48 | threshold[0] << 32 | threshold[0] << 16;
	qwSharpness[0] |= qwSharpness[0] << 48 | qwSharpness[0] << 32 | qwSharpness[0] << 16;
	threshold[1] = threshold[0];
	qwSharpness[1] = qwSharpness[0];

	int offset = 8;
	BYTE *oldBuf = (BYTE*)_aligned_malloc(nPitch,16);
	BYTE *newBuf = (BYTE*)_aligned_malloc(nPitch,16);
	memcpy(oldBuf,pBuffer+offset,nWidth);

	for(int row = 2; row < nHeight - 2; row ++)
	{
		BYTE *pLine2 = pBuffer + row * nPitch + offset;
		BYTE *pLine1 = oldBuf ;
		BYTE *pLine3 = pLine2 + nPitch ;

		memcpy(newBuf,pLine2,nWidth);//把当前行保存起来
		__asm
		{
			mov esi,pLine2;
			mov eax,pLine1;
			mov edx,pLine3;
			mov ecx,cycle;

			movdqa xmm7,qwSharpness;
			movdqa xmm6,threshold;
			pxor xmm0,xmm0;
			movq   mm0,[esi - 8];
			movq2dq xmm1,mm0;
			punpcklbw xmm1, xmm0
LOOP2:
			//mm5  
//  mm 3    mm1    mm4
			//mm2

			movdqa xmm3,xmm1;
			movq   mm1,[esi];
			movq   mm2,[edx];
			movq   mm4,[esi+8];

			movq2dq  xmm1,mm1;
			movq2dq  xmm2,mm2;
			movq2dq  xmm4,mm4;
			punpcklbw xmm1, xmm0;
			punpcklbw xmm2, xmm0;
			punpcklbw xmm4, xmm0;

			//////////
			movdqa  xmm5,xmm1;
			psrldq xmm3,14;
			pslldq xmm5,2;
			por   xmm3,xmm5;

			movdqa   xmm5,xmm1;
			pslldq  xmm4,14;
			psrldq  xmm5,2;
			por    xmm4,xmm5;
			movq   mm0,[eax];
			movq2dq xmm5,mm0;
			//movdqu xmm5,[eax];

			paddw xmm2,xmm1;
			paddw xmm3,xmm4;
			paddw xmm3,xmm2;
			punpcklbw xmm5, xmm0;
			paddw xmm3,xmm5;//mm3 destRow

			movdqa  xmm5,xmm1;

			pmullw  xmm5,qwTimes; // 放大curRow
			movdqa  xmm4,xmm5;//mm1 anothor currow;

			psubw xmm4,xmm3; // curRow - destRow
			movdqa  xmm2,xmm4;
			pcmpgtw xmm2,xmm6
			pand  xmm4,xmm2;//大于0的值
			pmullw xmm4,xmm7;
			psrlw  xmm4,8;			

			psubw xmm3,xmm5; //   destRow - curRow
			movdqa   xmm2,xmm3;
			pcmpgtw xmm2,xmm6
			pand   xmm3,xmm2;
			pmullw xmm3,xmm7;
			psrlw   xmm3,8;

			movdqa     xmm2,xmm1;
			paddusb  xmm2,xmm4;  //diff * count + curRow  saturate
			psubusb  xmm2,xmm3;
			packuswb xmm2, xmm2
			//movdqu [esi],xmm2
			MOVDQ2Q  mm0,xmm2;
			movq	[esi],mm0;
			
			add  eax,8;
			add  esi,8;
			add  edx,8;

			dec  ecx;
			jnz  LOOP2

			emms
		}
		//指针交换
		BYTE *pTemp = newBuf;
		newBuf = oldBuf;
		oldBuf = pTemp;
	}
	_aligned_free(oldBuf);
	_aligned_free(newBuf);

	USM_C(pBuffer,8,nHeight,nPitch,sharpen, nThreshold,8);
	USM_C(pBuffer + nPitch -8,8,nHeight,nPitch,sharpen, nThreshold,8 + nPitch % 8 );
}
/*
EXTERN_C __declspec(dllexport) HRESULT InitVideoEnhance(void** ppMediaPost)
{
	//检测指针的合法性
	CheckPointer(ppMediaPost, E_POINTER);

	//给指针赋初值
	*ppMediaPost = NULL;

	//创建实例对象
	*ppMediaPost = static_cast<IMediaPost*>(new CMediaPost());

	return S_OK;
}

EXTERN_C __declspec(dllexport) HRESULT DestoryMediaPost(void** ppMediaPost)
{
	//检测指针的合法性
	CheckPointer(ppMediaPost, E_POINTER);

	//还原实例指针对象
	CMediaPost* pMediaPost = static_cast<CMediaPost*>(*ppMediaPost);

	//销毁实例
	delete pMediaPost;

	//置空
	*ppMediaPost = NULL;

	return S_OK;
	
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	g_hInstance = hModule;
    return TRUE;
}*/