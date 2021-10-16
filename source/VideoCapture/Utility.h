#ifndef __YUNFAN_LIB_UTILITY_H__
#define __YUNFAN_LIB_UTILITY_H__
#define   MEMORY_ALIGN	16

#include <string>
#include <windows.h>

enum CPUType{CPU_MMX = 1,CPU_SSE2,CPUS_AVX2,CPU_UNKNOW};
#define  SAFE_DELETE(x) if(x){delete(x);(x) = NULL;}
#define  SAFE_DELETE_ARRAY(x) if(x){delete[](x);(x) = NULL;}
//#define		CheckPointer(p, ret) if(0==p){return ret;}
#define		CheckRange(a, min, max) if(a <min || a > max){return E_INVALIDARG;}
#define		SafeDeleteArray(p) if(p){delete[] p;}
#define SafeRelease(p) if(p){p->Release();}
#define SafeDelete(pObject)		 if(pObject) {delete pObject; pObject = NULL;}
//#define YV12toRGB32_SSE_CONVERT
bool CPUTest();
class CCpuID 
{
public: 
	CCpuID();
	enum flag_t {mmx=1, ssemmx=2, ssefpu=4, sse2=8, _3dnow=16} m_flags;
};
extern CCpuID g_cpuid;

extern void RGB32toYV12(BYTE *pSrc,   BYTE *pDst, int nSrcPitch,int nDstPitch, int width, int height);
extern void RGB32toYV12SSE(BYTE *pSrc,   BYTE *pDst, int nSrcPitch,int nDstPitch, int width, int height);
void RGB32toYV12MMX(BYTE *pSrc,   BYTE *pDst, int nSrcPitch,int nDstPitch, int width, int height);
void RGB32toYV12C(BYTE *pSrc,   BYTE *pDst, int nSrcPitch,int nDstPitch, int width, int height);

extern void YV12toRGB32(BYTE *pSrc,BYTE *pDst,int width,int height);
#ifdef YV12toRGB32_SSE_CONVERT
void YV12toRGB32SSE(BYTE *pSrc,BYTE *pDst,int width,int height);
#endif
void YV12toRGB32MMX(BYTE *pSrc,BYTE *pDst,int width,int height);
void YV12toRGB32C(BYTE *pSrc,BYTE *pDst,int width,int height);


extern CPUType g_cpuType;
#endif