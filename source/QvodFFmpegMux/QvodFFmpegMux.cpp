// QvodFFmpegMux.cpp : 定义 DLL 应用程序的入口点。
//
#pragma once
#include "stdafx.h"
#include "QvodFFmpegMux.h"



static const GUID MEDIASUBTYPE_AVC = {0x31435641, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71}};
static const GUID MEDIASUBTYPE_MKV_HEVC	=
{ 0x35363248,0x0000,0x0010,{0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71}};
static const GUID WMMEDIASUBTYPE_I420	=
{ 0x30323449,0x0000,0x0010,{0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71}};
//static const GUID MEDIASUBTYPE_H264 ={0x34363248, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};


//#define DEBUG_MUX 1 

int max_frames[4] = {INT_MAX, INT_MAX, INT_MAX, INT_MAX};

#define lrintf(f) (f>=0?(int32_t)(f+(float)0.5):(int32_t)(f-(float)0.4))
#define MY_SAMPLE_FFMPEG
#define SAMPLE_FFMPEG
#ifndef SAMPLE_FFMPEG
#pragma comment(lib, "../lib/ffmpeglib0.8/avcodec-53.lib")
#pragma comment(lib, "../lib/ffmpeglib0.8/avformat-53.lib")
#pragma comment(lib, "../lib/ffmpeglib0.8/avutil-51.lib")
#pragma comment(lib, "../lib/ffmpeglib0.8/swscale-2.lib")
#else
#ifdef MY_SAMPLE_FFMPEG
#pragma comment(lib, "../lib/MyFFmpeglib0.8/avcodec.lib")
#pragma comment(lib, "../lib/MyFFmpeglib0.8/avformat.lib")
#pragma comment(lib, "../lib/MyFFmpeglib0.8/avutil.lib")
#pragma comment(lib, "../lib/MyFFmpeglib0.8/swscale.lib")
#else
#pragma comment(lib, "../lib/Simple0.8/avcodec.lib")
#pragma comment(lib, "../lib/Simple0.8/avformat.lib")
#pragma comment(lib, "../lib/Simple0.8/avutil.lib")
#pragma comment(lib, "../lib/Simple0.8/swscale.lib")
#endif
#endif
AVRational _AVRational(int num, int den) 
{
	AVRational r = {num, den};
	return r;
}

const AMOVIESETUP_MEDIATYPE sudPinTypes =
{
	&MEDIATYPE_NULL,            // Major type
	&MEDIASUBTYPE_NULL          // Minor type
};

const AMOVIESETUP_PIN sudPins =
{
	L"Input",                   // Pin string name
	FALSE,                      // Is it rendered
	FALSE,                      // Is it an output
	FALSE,                      // Allowed none
	FALSE,                      // Likewise many
	&CLSID_NULL,                // Connects to filter
	L"Output",                  // Connects to pin
	1,                          // Number of types
	&sudPinTypes                // Pin information
};


const AMOVIESETUP_FILTER sudFilter =
{
	&CLSID_QvodFFmpegMux, L"QvodFFmpegMuxer", MERIT_DO_NOT_USE, 1, &sudPins
};

CFactoryTemplate g_Templates[] =
{
	
		L"QvodFFmpegMuxer",
			&CLSID_QvodFFmpegMux,
			CFFmpegMuxFilter::CreateInstance,
			NULL,
			&sudFilter
	
};

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

STDAPI DllRegisterServer()
{
	return AMovieDllRegisterServer2(TRUE);
}

STDAPI DllUnregisterServer()
{
	return AMovieDllRegisterServer2(FALSE);
}

extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);//APIENTRY

BOOL WINAPI DllMain(HANDLE hModule, 
					  DWORD  dwReason, 
					  LPVOID lpReserved)
{
	return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}

CFFmpegMuxInputPin::CFFmpegMuxInputPin(LPCWSTR pName, CFFmpegMuxFilter* pFilter, CCritSec* pLock, HRESULT* phr)
: CBaseInputPin(NAME("CFFmpegMuxInputPin"), pFilter, pLock, phr, pName)
, m_fActive(FALSE)
, m_ParentFilter(pFilter)
, m_fEndOfStreamReceived(false)
{
	m_bVideo = TRUE;
	m_bAudio = TRUE;
	m_bCheckMediaType = TRUE;
	allocated_audio_out_size = 0;
	allocated_audio_buf_size = 0;
	video_buf = NULL;
	audio_buf = NULL;
	audio_out = NULL;
	allocated_audio_out_size = 0;
	allocated_audio_buf_size = 0;
	memset(&video_fmt,0,sizeof(BITMAPINFOHEADER));
	memset(&audio_fmt,0,sizeof(WAVEFORMATEX));
	m_bThreadActive = FALSE; 
	video_sync_method = -1;
	ost = NULL;
	m_bCanDecoder = FALSE;
	m_pExtradata = NULL;
	m_nExtradataLen = 0;
	//m_vBitrate = 0;
}

CFFmpegMuxInputPin::~CFFmpegMuxInputPin()
{
	if(m_pExtradata)
	{
		delete m_pExtradata;
		m_pExtradata = NULL;
	}
	if(video_buf)
	{
		av_free(video_buf);
		video_buf = NULL;
	}
	if(audio_buf)
	{
		av_free(audio_buf);
	}
	if(audio_out)
	{
		av_free(audio_out);
	}
	allocated_audio_buf_size= allocated_audio_out_size= 0;
}

CMediaType& CFFmpegMuxInputPin::CurrentMediaType()
{
	return m_mt;
}
//WMMEDIASUBTYPE_I420
HRESULT CFFmpegMuxInputPin::CheckMediaType(const CMediaType* pmt)
{
	if	((pmt->majortype == MEDIATYPE_Video 
		&& pmt->subtype == MEDIASUBTYPE_YV12 ) 
		|| 
		(pmt->majortype == MEDIATYPE_Video && pmt->subtype == MEDIASUBTYPE_AVC /*&& pmt->formattype == FORMAT_MPEG2_VIDEO*/)
		||
		(pmt->majortype == MEDIATYPE_Video && pmt->subtype == MEDIASUBTYPE_H264 /*&& pmt->formattype == FORMAT_MPEG2_VIDEO*/)
		||
		(pmt->majortype == MEDIATYPE_Video && MEDIASUBTYPE_MKV_HEVC == pmt->subtype)
		||
		(pmt->majortype == MEDIATYPE_Video && WMMEDIASUBTYPE_I420 == pmt->subtype))
	{
		if(m_bCheckMediaType)
		{
			m_bCheckMediaType = FALSE;
		}
		bool b = GetBmpHeader(*pmt,video_fmt);
		
	 	
		if(pmt->formattype  == FORMAT_VideoInfo2)
		{
	/*		
			MPEG2VIDEOINFO* pm2vi = (MPEG2VIDEOINFO*)pmt->Format();
			video_fmt = pm2vi->hdr.bmiHeader;
			m_nExtradataLen = pm2vi->cbSequenceHeader+7;
			m_pExtradata = new BYTE[m_nExtradataLen];
			BYTE* dst = m_pExtradata;
			BYTE *temp = (BYTE*)pm2vi->dwSequenceHeader;
			int jj = 0;
			unsigned spslen = ((unsigned)temp[jj] << 8) | temp[jj+1];

			dst[0] = 0x01;
			dst[1] = pm2vi->dwProfile;
			dst[2] = 0;
			dst[3] = pm2vi->dwLevel;
			dst[4] = (pm2vi->dwFlags-1) | 0xFF;
			dst[5] = 0xe1;
			dst+=6;
			memcpy(dst, &pm2vi->dwSequenceHeader, spslen+2);
			dst+=(spslen+2);
			*dst++ = 0x01;

			jj = jj+2+spslen;

			unsigned ppslen = ((unsigned)temp[jj] << 8) | temp[jj+1];
			for(int i = 0;i < ppslen+2; i++)
			{
				*dst++ = temp[jj+i];
			}
			if(pm2vi->hdr.AvgTimePerFrame>0)
			{
				m_vBitrate = (double)(10000000/double(pm2vi->hdr.AvgTimePerFrame)) ;
			}
		}*/
			return S_OK;
		}	
		return S_OK;
	} 
	else if(pmt->majortype == MEDIATYPE_Audio && pmt->subtype == MEDIASUBTYPE_PCM )
	{
		BOOL b  = GetWaveFormat(*pmt,audio_fmt);
		return S_OK;
	}
	else 
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

	return S_OK;
}

HRESULT CFFmpegMuxInputPin::BreakConnect()
{
	HRESULT hr;

	if(FAILED(hr = __super::BreakConnect()))
		return hr;
	return hr;
}

HRESULT CFFmpegMuxInputPin::CompleteConnect(IPin* pPin)
{
	m_ParentFilter->AddInput();
	return S_OK;
}

IBaseFilter* CFFmpegMuxInputPin::GetFilterFromPin(IPin* pPin)
{
	if(!pPin) return NULL;
	IBaseFilter* pBF = NULL;
	PIN_INFO pi;
	if(pPin && SUCCEEDED(pPin->QueryPinInfo(&pi)))
		pBF = pi.pFilter;
	return(pBF);
}

BITMAPINFOHEADER CFFmpegMuxInputPin::GetVideoInfo()
{
	return video_fmt;
}

WAVEFORMATEX CFFmpegMuxInputPin::GetAudioInfo()
{
	return audio_fmt;
}

BYTE * CFFmpegMuxInputPin::GetExtradata()
{
	return m_pExtradata;
} 

int   CFFmpegMuxInputPin::GetExtradataLength()
{
	return m_nExtradataLen;
} 

//double CFFmpegMuxInputPin::GetVideoBitrate()
//{
//	return m_vBitrate;
//}

HRESULT CFFmpegMuxInputPin::Active()
{
	m_ParentFilter->AddStream();
	m_fActive = TRUE;
	m_fEndOfStreamReceived = FALSE;
	return __super::Active();
}

HRESULT CFFmpegMuxInputPin::Inactive()
{	
	m_fActive = FALSE;
	m_fEndOfStreamReceived = TRUE;
	return __super::Inactive();
}

STDMETHODIMP CFFmpegMuxInputPin::EndOfStream()
{
	HRESULT hr;
	
	if(FAILED(hr = __super::EndOfStream()))
		return hr;

	{
		CAutoLock cAutoLock(&m_ParentFilter->m_csQueue);
		m_block.RemoveAll();
	}
	
	m_fEndOfStreamReceived = true;

	return hr;
}

STDMETHODIMP CFFmpegMuxInputPin::ReceiveCanBlock()
{
	return __super::ReceiveCanBlock();
}

STDMETHODIMP CFFmpegMuxInputPin::NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
{
	return __super::NewSegment(tStart, tStop, dRate);
}

STDMETHODIMP CFFmpegMuxInputPin::BeginFlush()
{
	{
		CAutoLock cAutoLock(&m_ParentFilter->m_csQueue);
		m_block.RemoveAll();
	}
	return __super::BeginFlush();
}

STDMETHODIMP CFFmpegMuxInputPin::EndFlush()
{
	{
		CAutoLock cAutoLock(&m_ParentFilter->m_csQueue);
		m_block.RemoveAll();
	}
	return __super::EndFlush();
}



void CFFmpegMuxInputPin::SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) 
{
	FILE *pFile;
	char szFilename[32];
	int  y;

	// Open file
	sprintf(szFilename, "frame%d.ppm", iFrame);
	pFile=fopen(szFilename, "wb");
	if(pFile==NULL)
		return;

	// Write header
	fprintf(pFile, "P6\n%d %d\n255\n", width, height);

	// Write pixel data
	for(y=0; y<height; y++)
		fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);

	// Close file
	fclose(pFile);
}

void CFFmpegMuxInputPin::YV12ToAVFrame(BYTE * pData,int nLen,int nWidth,int nHeight,AVFrame * pFrame)
{
	//avcodec_get_frame_defaults(pFrame);
	//int size=avpicture_get_size(PIX_FMT_YUV420P,nWidth ,nHeight );
	//ASSERT(size==nLen);
	avpicture_fill( (AVPicture*) pFrame,pData,PIX_FMT_YUV420P , nWidth ,nHeight);
	BYTE * ptmp=pFrame->data [1];//U,V互换
	pFrame->data [1]=pFrame->data [2];
	pFrame->data [2]=ptmp;
}

bool CFFmpegMuxInputPin::GetDiskSpace(wchar_t *strDir,DWORD *nFreeDisk)
{
	DWORD  dwSectorsPerCluster;  //sectorspercluster
	DWORD  dwBytesPerSector; //bytespersector
	DWORD  dwNumberOfFreeClusters;  //freeclusters
	DWORD  dwTotalNumberOfClusters;  //totalclusters

	int len = wcslen(strDir);
	int i = 0;
	for( i = 0;i < len; i++)
	{
		if(strDir[i] == '\\' )
			break;
	}
	wchar_t buffer[5] = {0};
	wcsncpy(buffer,strDir,i+1);


	*nFreeDisk = 0;


	BOOL bOk = GetDiskFreeSpace(
		buffer,
		&dwSectorsPerCluster,
		&dwBytesPerSector,
		&dwNumberOfFreeClusters,
		&dwTotalNumberOfClusters
		);

	if (bOk)
	{
		*nFreeDisk = (DWORD)((double)dwNumberOfFreeClusters / 1024  /1024 * dwBytesPerSector * dwSectorsPerCluster);
		return true;
	}
	else
		return false;
}



STDMETHODIMP CFFmpegMuxInputPin::Receive(IMediaSample* pSample)
{
	//OutputDebugString(L"Receive\n");
	if(m_fEndOfStreamReceived) 
	{
		return S_FALSE;
	}
	CAutoLock locLock(&m_csReceive);
	
	if(m_mt.subtype == MEDIASUBTYPE_YV12 
		|| m_mt.subtype == MEDIASUBTYPE_AVC 
		|| m_mt.subtype ==MEDIASUBTYPE_H264 
		|| m_mt.subtype == MEDIASUBTYPE_MKV_HEVC
		|| m_mt.subtype == WMMEDIASUBTYPE_I420)
	{
		while(m_fActive )
		{
			{
				CAutoLock cAutoLock2(&m_ParentFilter->m_csQueue);
				if(m_block.GetCount() < 50)
				{
					break;	
				}
			}
#ifdef DEBUG_MUX
			//OutputDebugString(L"video is Full !\n");
#endif
			Sleep(1);
		}
		
	}
	if(m_mt.subtype == MEDIASUBTYPE_PCM)
	{
		while(m_fActive)
		{
			{
				CAutoLock cAutoLock2(&m_ParentFilter->m_csQueue);
				if(m_block.GetCount() < 10 && m_ParentFilter->m_bEnableAudio)
					break;
			}
#ifdef DEBUG_MUX
			//OutputDebugString(L"audio is Full !\n");
#endif
			Sleep(10);
		}
	}
	
	
	if(!m_fActive) 
	{
		return S_FALSE;
	}

	

	HRESULT hr;
	int out_size;
	
	if(FAILED(hr = __super::Receive(pSample)))
	{
#ifdef DEBUG_MUX
		OutputDebugString(L"Receive is Failed !\n");
#endif
		return hr;
	} 
	
	/*FILTER_STATE state = State_Stopped;
	m_pFilter->GetState(100, &state);
	if(State_Paused == state)
	{
	OutputDebugString(L"暂停时Receive****************\n");
	return S_OK;
	}*/
	
	BYTE* SampleBuff =NULL;
	pSample->GetPointer(&SampleBuff);
	long len = pSample->GetActualDataLength();
	if(len == 0)
	{
		return S_OK;
	}

	LONGLONG locStart = -1,locEnd = -1;
	hr = pSample->GetTime(&locStart, &locEnd);
	if(FAILED(hr) || locStart == -1 || locEnd == -1)
	{
#ifdef DEBUG_MUX
		OutputDebugString(L"GetTime is failed!/n");
#endif
		m_ParentFilter->NotifyEvent(EC_ERRORABORT, VFW_E_SAMPLE_TIME_NOT_SET, 0);
		return VFW_E_SAMPLE_TIME_NOT_SET;
	}

	if(m_mt.subtype == MEDIASUBTYPE_YV12 
		|| m_mt.subtype == MEDIASUBTYPE_AVC 
		|| m_mt.subtype ==MEDIASUBTYPE_H264 
		|| m_mt.subtype == MEDIASUBTYPE_MKV_HEVC 
		|| m_mt.subtype == WMMEDIASUBTYPE_I420)
	{
		double sync_ipts;
		AVFrame *final_picture, *formatted_picture,*resampling_dst;
		AVFrame p;
		if(m_bVideo)
		{
			m_bVideo = FALSE;
//			if(m_ParentFilter->GetFileDuration()  == 0)
//			{
//				REFERENCE_TIME m_rtDur = 0;
//				CComQIPtr<IMediaSeeking> pMS ;
//				if((pMS = GetFilterFromPin(GetConnected())) || (pMS = GetConnected()))
//					pMS->GetDuration(&m_rtDur);
//				if(m_rtDur == 0)
//				{
//					if(m_ParentFilter->GetQvodCallBack())
//					{	
//#ifdef DEBUG_MUX
//						OutputDebugString(L" video m_rtDur == 0 !\n");
//#endif
//						m_ParentFilter->GetQvodCallBack()->EventNotify_(QTC_MSG_TRANSCODE_ERROR,0,0);
//						
//					}
//					//m_fEndOfStreamReceived = TRUE;
//					//return S_FALSE;
//				}
//				else
//				{
//					m_ParentFilter->GetFileDuration(&m_rtDur);
//				}
//
//				return S_OK;
//			}
			for(int i = 0;i < m_ParentFilter->GetStreamCount();i++)
			{
				AVOutputStream *ost_temp = m_ParentFilter->GetOutStream(i);
				if(ost_temp->st->codec->codec_type == AVMEDIA_TYPE_VIDEO)
				{
					ost = ost_temp;
					break;
				}
			}
			if(ost == NULL)
			{
#ifdef DEBUG_MUX
				OutputDebugString(L"ost is NULL !\n");
#endif
				return S_FALSE;
			}
			///////////
			AM_MEDIA_TYPE *infmt = NULL;
			hr = pSample->GetMediaType(&infmt);
			if(hr == S_OK && (m_mt.subtype == MEDIASUBTYPE_AVC || m_mt.subtype ==MEDIASUBTYPE_H264 || m_mt.subtype == MEDIASUBTYPE_MKV_HEVC))
			{
				m_nExtradataLen = infmt->cbFormat /*- FIELD_OFFSET(VIDEOINFOHEADER2, bmiHeader)*/;
				m_pExtradata = new BYTE[m_nExtradataLen];
				BYTE* dst = m_pExtradata;
				memcpy(dst, infmt->pbFormat, m_nExtradataLen);
				ost->st->codec->extradata = m_pExtradata;
				ost->st->codec->extradata_size = m_nExtradataLen;
				DeleteMediaType(infmt);	
			}
			if(av_write_header(m_ParentFilter->GetContext()) < 0)
			{
#ifdef DEBUG_MUX
				OutputDebugString(L" video m_rtDur == 0 !\n");
#endif
				m_ParentFilter->GetQvodCallBack()->EventNotify_(QTC_MSG_TRANSCODE_ERROR,0,0);
			}
			m_ParentFilter->m_bEnableAudio = true;
			/////////////
			video_buf = (BYTE*)av_malloc(video_fmt.biWidth*video_fmt.biHeight*6 +200);		
			avcodec_get_frame_defaults(&p);
			int size=avpicture_get_size(PIX_FMT_YUV420P,video_fmt.biWidth, video_fmt.biHeight);
		}
				
	    AVCodecContext *enc= ost->st->codec;

		sync_ipts =  (double)locStart/10/AV_TIME_BASE / av_q2d(enc->time_base);

		
		int nb_frames = 1;

		if(video_sync_method)
		{
			double vdelta = sync_ipts - ost->sync_opts;
			if (vdelta < -1.1)
				nb_frames = 0;
			else if (video_sync_method == 2 || (video_sync_method<0 && (m_ParentFilter->GetContext()->oformat->flags & AVFMT_VARIABLE_FPS)))
			{
				if(vdelta<=-0.6)
				{
					nb_frames=0;
				}
				else if(vdelta>0.6)
					ost->sync_opts= lrintf(sync_ipts);
			}
			else if (vdelta > 1.1)
				nb_frames = lrintf(vdelta);
		}
		else
			ost->sync_opts= lrintf(sync_ipts);

		

		if(m_mt.subtype == MEDIASUBTYPE_AVC || MEDIASUBTYPE_H264 == m_mt.subtype || m_mt.subtype == MEDIASUBTYPE_MKV_HEVC)
		{
			CAutoLock cAutoLock2(&m_ParentFilter->m_csQueue);
			//for(int i=0;i<nb_frames;i++)
			{
			AVFrame avframe;
			
			CAutoPtr<BLOCK> b(new BLOCK());
			av_init_packet(&b->packet);

			//__int64 ost_tb_start_time= av_rescale_q(0, AV_TIME_BASE_Q, ost->st->time_base);

			avcodec_get_frame_defaults(&avframe);
			ost->st->codec->coded_frame= &avframe;

			if (ost->st->codec->coded_frame->pts != AV_NOPTS_VALUE)
				b->packet.pts= av_rescale_q(ost->st->codec->coded_frame->pts, ost->st->codec->time_base, ost->st->time_base);
			b->start = locStart;
			b->stop = locEnd;
			b->packet.stream_index= ost->st->index;
			b->packet.data = SampleBuff;
			b->packet.size = len;
			b->packettype = Video_Type;
			//b->packet.pts = locStart/1000000 ;
			//if (ost->st->codec->coded_frame->pts != AV_NOPTS_VALUE)
				//b->packet.pts= av_rescale_q(ost->sync_opts, ost->st->codec->time_base, ost->st->time_base);
			
			//b->packet.duration = (locEnd - locStart) ;
			//b->packet.dts = locStart ;
		
			HRESULT hr = pSample->IsSyncPoint();
			if(hr ==S_OK)
			{
				avframe.key_frame = 1;
			}
			else
			{
				avframe.key_frame = 0;
			}
			if(ost->st->codec->coded_frame->key_frame)
				b->packet.flags |= AV_PKT_FLAG_KEY;
			
			int ret = av_interleaved_write_frame(m_ParentFilter->GetContext(), &b->packet);	
			if(ret < 0)
			{
#ifdef DEBUG_MUX
				OutputDebugString(L"av_interleaved_write_frame failed/n"); 
#endif
				DWORD diskSize = 0;
				if(GetDiskSpace(m_ParentFilter->GetFileURL(),&diskSize))
				{
					if( diskSize <=10)
					{
						if(m_ParentFilter->GetQvodCallBack())
						{	
							m_ParentFilter->GetQvodCallBack()->EventNotify_(QTC_MSG_FULLDISK_ERROR,0,0);
						}
						return VFW_E_RUNTIME_ERROR;
					}
				}
				if(m_ParentFilter->GetQvodCallBack())
				{	
					m_ParentFilter->GetQvodCallBack()->EventNotify_(QTC_MSG_TRANSCODE_ERROR,0,0);
				}
				return VFW_E_RUNTIME_ERROR;
			}
			m_block.AddTail(b);
		
		    ost->sync_opts++;
		    ost->frame_number++;
			}
		}

		if(m_mt.subtype == MEDIASUBTYPE_YV12 
			|| m_mt.subtype == WMMEDIASUBTYPE_I420)
		{
			nb_frames= FFMIN(nb_frames, max_frames[AVMEDIA_TYPE_VIDEO] - ost->frame_number);
			if (nb_frames <= 0) 
				return S_OK;
			YV12ToAVFrame(SampleBuff,len,video_fmt.biWidth, video_fmt.biHeight,&p);

			formatted_picture = &p;
			final_picture = formatted_picture;
			resampling_dst = &ost->pict_tmp;

			if (0/*ost->video_resample*/) 
			{
				final_picture = &ost->pict_tmp;
				sws_scale(ost->img_resample_ctx, formatted_picture->data, formatted_picture->linesize,
					0, ost->resample_height, resampling_dst->data, resampling_dst->linesize);
			}
			{
				CAutoLock cAutoLock2(&m_ParentFilter->m_csQueue);
				for(int i=0;i<nb_frames;i++)
				{	
					AVFrame big_picture;
					big_picture= *final_picture;
					big_picture.interlaced_frame = p.interlaced_frame;
					big_picture.quality = ost->st->quality;
					big_picture.pict_type = (AVPictureType)0;
					big_picture.pts = ost->sync_opts;
					if (ost->forced_kf_index < ost->forced_kf_count &&
						big_picture.pts >= ost->forced_kf_pts[ost->forced_kf_index]) {
							big_picture.pict_type = AV_PICTURE_TYPE_I;
							ost->forced_kf_index++;
					}

					out_size = avcodec_encode_video(ost->st->codec, video_buf, video_fmt.biWidth*video_fmt.biHeight*6 +200, &big_picture);							
					if(out_size < 0)
					{
#ifdef DEBUG_MUX
						OutputDebugString(L"avcodec_encode_video failed/n"); 
#endif 
						if(m_ParentFilter->GetQvodCallBack())
						{	
							m_ParentFilter->GetQvodCallBack()->EventNotify_(QTC_MSG_TRANSCODE_ERROR,0,0);
						}
						return VFW_E_RUNTIME_ERROR;
					}
#ifdef DEBUG_MUX
					//wchar_t str1[100];
					//swprintf(str1, L"out_size: %d\n",out_size);
					//OutputDebugString(str1); 
#endif 
					/* if zero size, it means the image was buffered */
					if (out_size > 0)
					{ 
						CAutoPtr<BLOCK> b(new BLOCK());
						av_init_packet(&b->packet);
						if (ost->st->codec->coded_frame->pts != AV_NOPTS_VALUE)
							b->packet.pts= av_rescale_q(ost->st->codec->coded_frame->pts, ost->st->codec->time_base, ost->st->time_base);
						b->start = locStart;
						b->stop = locEnd;
						if(ost->st->codec->coded_frame->key_frame)
							b->packet.flags |= AV_PKT_FLAG_KEY;
						b->packet.stream_index= ost->st->index;
						b->packet.data= video_buf;
						b->packet.size= out_size;
						b->packettype = Video_Type;
						{	
							int ret = av_interleaved_write_frame(m_ParentFilter->GetContext(), &b->packet);								
							if(ret < 0)
							{
#ifdef DEBUG_MUX
								OutputDebugString(L"av_interleaved_write_frame failed/n"); 
#endif
								DWORD diskSize = 0;
								if(GetDiskSpace(m_ParentFilter->GetFileURL(),&diskSize))
								{
									if( diskSize <=10)
									{
										if(m_ParentFilter->GetQvodCallBack())
										{	
											m_ParentFilter->GetQvodCallBack()->EventNotify_(QTC_MSG_FULLDISK_ERROR,0,0);
										}
										return VFW_E_RUNTIME_ERROR;
									}
								}
								if(m_ParentFilter->GetQvodCallBack())
								{	
									m_ParentFilter->GetQvodCallBack()->EventNotify_(QTC_MSG_TRANSCODE_ERROR,0,0);
								}
								return VFW_E_RUNTIME_ERROR;
							}
							m_block.AddTail(b);							
						}
						
					}
					ost->sync_opts++;
					ost->frame_number++;
				}
			}	
		}
	}

	if(m_mt.subtype == MEDIASUBTYPE_PCM)
	{
		pSample->SetSyncPoint(TRUE);
		AM_MEDIA_TYPE *infmt = NULL;
		hr = pSample->GetMediaType(&infmt);
		if(hr == S_OK)
		{
			//OutputDebugString(L"hr = pSample->GetMediaType(&infmt);/n"); 
			WAVEFORMATEX *pWfe = (WAVEFORMATEX *)infmt->pbFormat;
			//m_SrcChannels = pWfe->nChannels;
			//m_insf = *infmt;
			DeleteMediaType(infmt);	
		}
		uint8_t *buftmp;
		int64_t audio_out_size, audio_buf_size;
		int64_t allocated_for_size= len;

		int size_out, frame_bytes, ret;
		
		if(m_bAudio)
		{
			m_bAudio = FALSE;
			if(audio_fmt.nChannels == 0 || audio_fmt.nSamplesPerSec == 0)
			{
				m_fEndOfStreamReceived = TRUE;
				return S_FALSE;
			}
			
			for(int i = 0;i < m_ParentFilter->GetStreamCount();i++)
			{
				AVOutputStream *ost_temp = m_ParentFilter->GetOutStream(i);
				if(ost_temp->st->codec->codec_type == AVMEDIA_TYPE_AUDIO)
				{
					ost = ost_temp;
					break;
				}
			}
			if(ost == NULL)
			{
				m_fEndOfStreamReceived = TRUE;
				return S_FALSE;
			}	
			else
			{
				if(ost->st->codec)
				{
					if(ost->st->codec->codec == NULL)
					{
						m_fEndOfStreamReceived = TRUE;
						return S_FALSE;
					}				
				}
			}
		}

		AVCodecContext *enc= ost->st->codec;
		int osize= av_get_bits_per_sample_format(enc->sample_fmt)/8;
		int isize=2/*av_get_bits_per_sample_format(AV_SAMPLE_FMT_FLT)/8*/;

		const int coded_bps = av_get_bits_per_sample(enc->codec->id);
		audio_buf_size= (allocated_for_size + isize*audio_fmt.nChannels - 1) / (isize*audio_fmt.nChannels);
		audio_buf_size= (audio_buf_size*enc->sample_rate + audio_fmt.nSamplesPerSec) / audio_fmt.nSamplesPerSec;
		audio_buf_size= FFMAX(audio_buf_size, enc->frame_size)/*audio_buf_size*2 + 10000*/; //safety factors for the deprecated resampling API
		audio_buf_size*= osize*enc->channels;


		audio_out_size= FFMAX(audio_buf_size, enc->frame_size * osize * enc->channels);
		if(coded_bps > 8*osize)
			audio_out_size= audio_out_size * coded_bps / (8*osize);
		audio_out_size += FF_MIN_BUFFER_SIZE;

		if(audio_out_size > INT_MAX || audio_buf_size > INT_MAX)
		{
			return S_FALSE;
		}

		av_fast_malloc(&audio_buf, &allocated_audio_buf_size, audio_buf_size);
		av_fast_malloc(&audio_out, &allocated_audio_out_size, audio_out_size);

		if (!audio_buf || !audio_out)
		{
			return S_FALSE;
		}

		if (enc->channels != audio_fmt.nChannels)
			ost->audio_resample = 1;

		if (ost->audio_resample &&!ost->resample)
		{		
			ost->resample = av_audio_resample_init(enc->channels,   audio_fmt.nChannels,
				enc->sample_rate, audio_fmt.nSamplesPerSec,
				enc->sample_fmt, enc->sample_fmt ,16, 10, 0, 0.8);
			if (!ost->resample) 
			{
#ifdef DEBUG_MUX
				OutputDebugString(L"av_audio_resample_init failed/n"); 
#endif
				//return VFW_E_RUNTIME_ERROR;
				return S_OK;
			}
		}

#define MAKE_SFMT_PAIR(a,b) ((a)+SAMPLE_FMT_NB*(b))
		if (!ost->audio_resample && enc->sample_fmt/*AV_SAMPLE_FMT_FLT*/!=enc->sample_fmt &&
			MAKE_SFMT_PAIR(enc->sample_fmt,enc->sample_fmt/*AV_SAMPLE_FMT_FLT*/)!=ost->reformat_pair) 
		{
			if (ost->reformat_ctx)
				av_audio_convert_free(ost->reformat_ctx);
			ost->reformat_ctx = av_audio_convert_alloc(enc->sample_fmt, 1,enc->sample_fmt/*AV_SAMPLE_FMT_FLT*/, 1, NULL, 0);
			if (!ost->reformat_ctx) {
				return S_FALSE;
			}
			ost->reformat_pair=MAKE_SFMT_PAIR(enc->sample_fmt,enc->sample_fmt/*AV_SAMPLE_FMT_FLT*/);
		}
		ost->sync_opts= lrintf((double)locStart/10/AV_TIME_BASE* enc->sample_rate)
			- av_fifo_size(ost->fifo)/(ost->st->codec->channels * 2); //FIXME wrong

#ifdef DEBUG_MUX
		//wchar_t str1[100];
		//swprintf(str1, L"optsi packets: %lld\n", ost->sync_opts);
		//OutputDebugString(str1); 
#endif 

		if (ost->audio_resample) 
		{
			buftmp = audio_buf;
			size_out = audio_resample(ost->resample,
				(short *)buftmp, (short *)SampleBuff,
				len / (audio_fmt.nChannels * isize));
			size_out = size_out * enc->channels * osize;
		} 
		else
		{
			buftmp = SampleBuff;
			size_out = len;
		}

		if (!ost->audio_resample && enc->sample_fmt/*AV_SAMPLE_FMT_FLT*/!=enc->sample_fmt) 
		{
			const void *ibuf[6]= {buftmp};
			void *obuf[6]= {audio_buf};
			int istride[6]= {isize};
			int ostride[6]= {osize};
			int len= size_out/istride[0];
			if (av_audio_convert(ost->reformat_ctx, obuf, ostride, ibuf, istride, len)<0) {
				return S_FALSE;
			}
			buftmp = audio_buf;
			size_out = len*osize;
		}


		/* now encode as many frames as possible */
		if (enc->frame_size > 1)
		{
			/* output resampled raw samples */
			if (av_fifo_realloc2(ost->fifo, av_fifo_size(ost->fifo) + size_out) < 0)
			{
				return S_FALSE;
			}
			av_fifo_generic_write(ost->fifo, buftmp, size_out, NULL);
	
			frame_bytes = enc->frame_size * osize * enc->channels;

			{
				CAutoLock cAutoLock2(&m_ParentFilter->m_csQueue);
				while (av_fifo_size(ost->fifo) >= frame_bytes)
				{
					av_fifo_generic_read(ost->fifo, audio_buf, frame_bytes, NULL);
					ret = avcodec_encode_audio(ost->st->codec, audio_out, audio_out_size,(short *)audio_buf);			
					if (ret < 0) 
					{
#ifdef DEBUG_MUX
						OutputDebugString(L"avcodec_encode_audio failed/n"); 
#endif
						if(m_ParentFilter->GetQvodCallBack())
						{	
							m_ParentFilter->GetQvodCallBack()->EventNotify_(QTC_MSG_TRANSCODE_ERROR,0,0);
						}
						return VFW_E_RUNTIME_ERROR;
					}

					CAutoPtr<BLOCK> b(new BLOCK());
					av_init_packet(&b->packet);	
					if (ost->st->codec->coded_frame && ost->st->codec->coded_frame->pts != AV_NOPTS_VALUE)
					{
						b->packet.pts=av_rescale_q(ost->st->codec->coded_frame->pts, ost->st->codec->time_base, ost->st->time_base);
					}						
					b->start = locStart;
					b->stop = locEnd;
					b->packet.flags |= AV_PKT_FLAG_KEY;
					b->packet.stream_index= ost->st->index;	
					b->packet.data= audio_out;
					b->packet.size= ret;
					b->packettype = Audio_Type;
					ret = av_interleaved_write_frame(m_ParentFilter->GetContext(), &b->packet);				
					if(ret < 0)
					{
#ifdef DEBUG_MUX
						OutputDebugString(L"av_interleaved_write_frame failed/n"); 
#endif
						DWORD diskSize = 0;
						if(GetDiskSpace(m_ParentFilter->GetFileURL(),&diskSize))
						{
							if( diskSize <=10)
							{
								if(m_ParentFilter->GetQvodCallBack())
								{	
									m_ParentFilter->GetQvodCallBack()->EventNotify_(QTC_MSG_FULLDISK_ERROR,0,0);
								}
								return VFW_E_RUNTIME_ERROR;
							}
						}
						if(m_ParentFilter->GetQvodCallBack())
						{	
							m_ParentFilter->GetQvodCallBack()->EventNotify_(QTC_MSG_TRANSCODE_ERROR,0,0);
						}
						return VFW_E_RUNTIME_ERROR;
					}
					m_block.AddTail(b);	
					ost->sync_opts += enc->frame_size;				
				}
			}
		} 
	}
	return S_OK;
}





bool CFFmpegMuxInputPin::GetBmpHeader(const CMediaType &mt,BITMAPINFOHEADER & bph)
{
	if(*mt.FormatType() == FORMAT_VideoInfo) 
	{
		VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *) mt.Format();
		bph = pvi->bmiHeader;
		int width = bph.biWidth;
		int height = bph.biHeight;
		if(pvi->AvgTimePerFrame>0)
		{
			//m_vBitrate = (double)(10000000/double(pvi->AvgTimePerFrame)) ;
		}
		m_ParentFilter->SetWidth(&width);
		m_ParentFilter->SetHeight(&height);
		return true;
	}
	else if(*mt.FormatType() == FORMAT_VideoInfo2) 
	{
		VIDEOINFOHEADER2 *pvi = (VIDEOINFOHEADER2 *) mt.Format();
		bph = pvi->bmiHeader;
		int width = bph.biWidth;
		int height = bph.biHeight;
		if(pvi->AvgTimePerFrame>0)
		{
			//m_vBitrate = (double)(10000000/double(pvi->AvgTimePerFrame)) ;
		}
		
		m_ParentFilter->SetWidth(&width);
		m_ParentFilter->SetHeight(&height);
		return true;
	}
	else
	{
		return false;
	}
}

bool CFFmpegMuxInputPin::GetWaveFormat(const CMediaType &mt,WAVEFORMATEX & p)
{
	if(*mt.FormatType() == FORMAT_WaveFormatEx) 
	{
		WAVEFORMATEX *pai = (WAVEFORMATEX *) mt.Format();
		p = *pai;
		return true;
	}
	else
	{
		return false;
	}
}



void CFFmpegMuxInputPin::FillYuvImage(AVFrame *pict, BYTE* pData, int width, int height,int yPitch,int uvPitch)
{
	for(int i = 0;i < height; i++)
	{
		CopyMemory(pict->data[0],pData,pict->linesize[0]);
		pData += yPitch;
		pict->data[0] += pict->linesize[0];

	}
	for(int i = 0;i < height/2; i++)
	{
		CopyMemory(pict->data[2],pData,pict->linesize[2]);
		pData += uvPitch;
		pict->data[2] += pict->linesize[2];
	}
	for(int i = 0;i < height/2; i++)
	{
		CopyMemory(pict->data[1],pData,pict->linesize[1]);
		pData += uvPitch;
		pict->data[1] += pict->linesize[1];
	}	    
}
AVFrame *CFFmpegMuxInputPin::alloc_picture(int pix_fmt, int width, int height)
{
	AVFrame *picture;
	BYTE *picture_buf;
	int size;

	picture = avcodec_alloc_frame();
	if (!picture)
		return NULL;
	size = avpicture_get_size((PixelFormat)pix_fmt, width, height);
	picture_buf = (BYTE*)av_malloc(size);
	if (!picture_buf) {
		av_free(picture);
		return NULL;
	}
	avpicture_fill((AVPicture *)picture, picture_buf,
		(PixelFormat)pix_fmt, width, height);
	return picture;
}

CUnknown * WINAPI CFFmpegMuxFilter::CreateInstance(LPUNKNOWN punk, HRESULT *phr) 
{
	CFFmpegMuxFilter *pNewObject = new CFFmpegMuxFilter(punk, phr);
	if (pNewObject == NULL) {
		*phr = E_OUTOFMEMORY;
	}
	return pNewObject;
} 

CFFmpegMuxFilter::CFFmpegMuxFilter(LPUNKNOWN pUnk, HRESULT* phr)
: CBaseFilter(NAME("QvodFFmpegMuxer"), NULL, this, CLSID_QvodFFmpegMux)
, m_rtCurrent(0)
{
	CAutoLock cAutoLock(this);
	memset(m_sFilePath,0,MAX_PATH);
	audio_stream_copy = 0;
	video_stream_copy = 0;

	audio_able = 0;
	video_able = 0;
	mux_preload= 0.5;
	mux_max_delay= 0.7;
	loop_output = AVFMT_NOOUTPUTLOOP;
	frame_width  = 0;
	frame_height = 0;
	audio_sample_rate = 44100;
	audio_channels = 2;
	thread_count= 2;
	frame_pix_fmt = PIX_FMT_NONE;
	video_global_header = 0;
	frame_aspect_ratio = 0;
	frame_padtop  = 0;
	frame_padbottom = 0;
	frame_padleft  = 0;
	frame_padright = 0;
	me_threshold = 0;
	intra_dc_precision = 8;
	audio_sample_fmt = SAMPLE_FMT_NONE;
	channel_layout = 0;
	extra_size = 0;
	m_oc = NULL;
	ost = NULL;
	ost_table = NULL;
	m_Width = 0;
	m_Height = 0;
	m_QvodCallBack = NULL; 
	m_Progress = 0;
	m_rtDur = 0;
	m_TransStopTime = 0;
	m_nStreamNum = 0;
	m_bEnableAudio = false;
	m_bError = false;
	
	memset(&m_TransConfig,0,sizeof(MUX_CONFIG));
	video_opts = NULL;
	sws_flags = SWS_BILINEAR;
	sws_opts = NULL;
	vtemp = 0;
	atemp = 0;

	pVideoPin = NULL;
	pAudioPin = NULL;
	m_vBitrate = 0; 

	
	
	POSITION pos = m_pInputs.GetHeadPosition();
	while(pos)
	{
		CBasePin* pPin = m_pInputs.GetNext(pos);
		if(!pPin->IsConnected()) return;
	}
	CString name;
	name.Format(L"Track %d", m_pInputs.GetCount());
	HRESULT hr;
	CAutoPtr<CFFmpegMuxInputPin> pPin(new CFFmpegMuxInputPin(name, this, this, &hr));
	m_pInputs.AddTail(pPin);
	if(phr) *phr = S_OK;
}



CFFmpegMuxFilter::~CFFmpegMuxFilter()
{
	CAutoLock cAutoLock(this);
	VideoBlock.RemoveAll(); 
	AudioBlock.RemoveAll();
	//ReleaseSource();
}

STDMETHODIMP CFFmpegMuxFilter::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
	CheckPointer(ppv, E_POINTER);
	if (riid == IID_IFFmpegMux)
	{
		return GetInterface((IFFmpegMux *) this, ppv);
	}
	else if(riid == IID_IFileSinkFilter)
	{
		return GetInterface((IFileSinkFilter *) this, ppv);//
	}
	else
	{
		return CBaseFilter::NonDelegatingQueryInterface(riid, ppv);
	}
}

void CFFmpegMuxFilter::AddInput()
{
	POSITION pos = m_pInputs.GetHeadPosition();
	while(pos)
	{
		CBasePin* pPin = m_pInputs.GetNext(pos);
		if(!pPin->IsConnected()) return;
	}

   CString name;
	name.Format(L"Track %d", m_pInputs.GetCount()+1);

	HRESULT hr;
	CAutoPtr<CFFmpegMuxInputPin> pPin(new CFFmpegMuxInputPin(name, this, this, &hr));
	m_pInputs.AddTail(pPin);

}

int CFFmpegMuxFilter::GetPinCount()
{
	return m_pInputs.GetCount();
}

CBasePin* CFFmpegMuxFilter::GetPin(int n)
{
	CAutoLock cAutoLock(this);

	if(n >= 0 && n < (int)m_pInputs.GetCount())
	{
		if(POSITION pos = m_pInputs.FindIndex(n))
			return m_pInputs.GetAt(pos);
	}

	return NULL;
}

void CFFmpegMuxFilter::ReleaseSource()
{
	if(m_oc == NULL)
	{
		return ;
	}
	bool bVideo = true;
	bool bAudio = true;
	if(pVideoPin)
	{
		bVideo = pVideoPin->m_bVideo;
	}
	if(pAudioPin)
	{
		bVideo = pAudioPin->m_bAudio;
	}
	if(!m_bError && (!bVideo || !bAudio ))
	{
		av_write_trailer(m_oc);
	}
	for(int i=0;i<m_oc->nb_streams;i++)
	{
		ost = ost_table[i];
		if (ost->encoding_needed) {
			av_freep(&ost->st->codec->stats_in);
			avcodec_close(ost->st->codec);
		}
	}
	if (ost_table)
	{
		for(int i=0;i<m_oc->nb_streams;i++) 
		{
			ost = ost_table[i];
			if (ost) 
			{
				av_fifo_free(ost->fifo); /* works even if fifo is not
										 initialized but set to zero */
				av_free(ost->pict_tmp.data[0]);
				av_free(ost->st->codec->priv_data);
				if (0/*ost->video_resample*/)
					sws_freeContext(ost->img_resample_ctx);
				if (ost->resample)
					audio_resample_close(ost->resample);
				av_free(ost);
			}
		}
		av_free(ost_table);
		ost_table = NULL;
	}
	if (!(m_oc->oformat->flags & AVFMT_NOFILE) && m_oc->pb)
	{
		url_fclose(m_oc->pb);
	}
	for(int j=0;j<m_oc->nb_streams;j++) 
	{
		av_free(m_oc->streams[j]->codec);
		av_free(m_oc->streams[j]);
	}
	
	av_dict_free(&video_opts);
	av_free(m_oc);
	m_oc = NULL;

} //

BOOL CFFmpegMuxFilter::IsAsc(char *pSource, int nlen)
{
	unsigned char*pTempSource = (unsigned char*)pSource;
	int len = nlen;//strlen(pSource);
	BOOL bAsc = TRUE;
	while(len > 0)
	{
		if( *pTempSource > 0x80) // (10000000): 值小于0x80的为ASCII字符
		{
			bAsc = FALSE;
			break;
		}
		pTempSource++;
		len--;
	}
	return bAsc;
}

STDMETHODIMP CFFmpegMuxFilter::Stop()
{
	CAutoLock cAutoLock(this);

	HRESULT hr;

	if(FAILED(hr = __super::Stop()))
		return hr;
	
	CallWorker(CMD_EXIT);
	ReleaseSource();
	return hr;
}

STDMETHODIMP CFFmpegMuxFilter::Pause()
{
	CAutoLock cAutoLock(this);

	FILTER_STATE fs = m_State;

	HRESULT hr;

	if(FAILED(hr = __super::Pause()))
		return hr;

	if(fs == State_Stopped )
	{
		CAMThread::Create();
		CallWorker(CMD_RUN);
	}
	/*else
	{
		VideoBlock.RemoveAll();
		AudioBlock.RemoveAll();
	}*/

	return hr;
}

STDMETHODIMP CFFmpegMuxFilter::Run(REFERENCE_TIME tStart)
{
	CAutoLock cAutoLock(this);

	HRESULT hr;

	if(FAILED(hr = __super::Run(tStart)))
		return hr;


	return hr;
}

STDMETHODIMP CFFmpegMuxFilter::SetMuxConfig(MUX_CONFIG* pMuxConfig)
{
	CAutoLock cAutoLock(this);
	if(!pMuxConfig)
	{
		return S_FALSE;
	}
	m_TransConfig = * pMuxConfig;
	return S_OK;
}
STDMETHODIMP CFFmpegMuxFilter::GetMuxConfig(MUX_CONFIG* pMuxConfig)
{
	return S_OK;
}

STDMETHODIMP CFFmpegMuxFilter::SetMuxThreadCount(DWORD Threadcount) 
{
	CAutoLock cAutoLock(this);
	thread_count = Threadcount;
	return S_OK;

}
STDMETHODIMP CFFmpegMuxFilter::GetMuxThreadCount(DWORD &Threadcount) 
{
	CAutoLock cAutoLock(this);
	Threadcount = thread_count;
	return S_OK;
}

STDMETHODIMP CFFmpegMuxFilter::SetMuxDuration(REFERENCE_TIME* rtStart, REFERENCE_TIME* rtStop)
{
	CAutoLock cAutoLock(this);
	if(rtStop == NULL || rtStart == NULL)
	{
		return S_FALSE;
	}
	if(*rtStop > 0)
	{
		m_TransStopTime = *rtStop;
		return S_OK;
	}
	return S_FALSE;

}
STDMETHODIMP CFFmpegMuxFilter::GetMuxDuration(REFERENCE_TIME* rtStart, REFERENCE_TIME* rtStop) 
{
	if(rtStart == NULL || rtStop == NULL)
	{
		return S_FALSE;
	}
	*rtStart = 0;
	*rtStop  = m_rtDur;
	return S_OK;
}

STDMETHODIMP CFFmpegMuxFilter::SetSrcFrameRate(float framerate)
{
	m_vBitrate = framerate;
	return S_OK;
}

STDMETHODIMP CFFmpegMuxFilter::SetQvodCallBack(interface IQvodCallBack* pQvodCallBack)
{
	if(!pQvodCallBack)
	{
		return S_FALSE;
	}
	m_QvodCallBack = pQvodCallBack;
	return S_OK;

}

STDMETHODIMP CFFmpegMuxFilter::GetDuration ( LONGLONG *lt) 
{
	CAutoLock locLock(m_pLock);
	POSITION pos = m_pInputs.GetHeadPosition();
	CFFmpegMuxInputPin *pPin= NULL;
	while(pos)
	{
		pPin = m_pInputs.GetNext(pos);
	}
	//这个函数有BUG，有时得到的总时间不正确
	CRendererPosPassThru *pPosition;
	HRESULT hr = NOERROR;
	pPosition = new CRendererPosPassThru(NAME("Renderer CPosPassThru"),
		CBaseFilter::GetOwner(),
		(HRESULT *) &hr,
		pPin);
	if(hr == NOERROR)
	{
		pPosition->GetDuration(lt);
		delete pPosition;
		return S_OK;
	}
	if (pPosition)
	{
		delete pPosition;
	}
	return S_FALSE;	
}

//IFileSinkFilter Implementation
HRESULT CFFmpegMuxFilter::SetFileName(LPCOLESTR inFileName, const AM_MEDIA_TYPE* inMediaType) 
{
	CAutoLock locLock(m_pLock);
	avcodec_register_all();
	av_register_all();
	sws_opts = sws_getContext(16,16,(PixelFormat)0, 16,16,(PixelFormat)0, sws_flags, NULL,NULL,NULL);
	wcscpy(mFileName,inFileName);
	
	WideCharToMultiByte( CP_UTF8, 0, inFileName, -1,m_sFilePath, MAX_PATH, NULL, NULL );//wcslen(inFileName) + 1
	//WideCharToMultiByte(CP_ACP, 0, inFileName, -1, m_sFilePath, MAX_PATH, NULL, NULL);
	return OutputFile(m_sFilePath);
}
HRESULT CFFmpegMuxFilter::GetCurFile(LPOLESTR* outFileName, AM_MEDIA_TYPE* outMediaType)
{
	//Return the filename and mediatype of the raw data
	CheckPointer(outFileName, E_POINTER);
	*outFileName = NULL;

	if (mFileName != NULL) 
	{
		*outFileName = (LPOLESTR)
			QzTaskMemAlloc(sizeof(WCHAR) * (1+lstrlenW(mFileName)));

		if (*outFileName != NULL) 
		{
			lstrcpyW(*outFileName, mFileName);
		}
	}

	if(outMediaType) 
	{
		ZeroMemory(outMediaType, sizeof(*outMediaType));
		outMediaType->majortype = MEDIATYPE_NULL;
		outMediaType->subtype = MEDIASUBTYPE_NULL;
	}

	return S_OK;
}

long CFFmpegMuxFilter::EventNotify_(UINT uMsg, long wparam, long lparam)
{
	if(uMsg == QTC_MSG_PROGRESS)
	{	
		lparam = m_Progress;
	}
	return NOERROR;
}

IQvodCallBack *CFFmpegMuxFilter::GetQvodCallBack()
{
	return m_QvodCallBack;
}

REFERENCE_TIME CFFmpegMuxFilter::GetFileDuration()
{
	return m_rtDur;
}

REFERENCE_TIME CFFmpegMuxFilter::GetShizhuanDuration()
{
	return m_TransStopTime;
}

wchar_t * CFFmpegMuxFilter::GetFileURL()
{
	return mFileName;
}


AVFormatContext * CFFmpegMuxFilter::GetContext()
{
	return m_oc;
}
AVOutputStream * CFFmpegMuxFilter::GetOutStream(int num)
{
	return ost_table[num];
}

DWORD CFFmpegMuxFilter::ThreadProc()
{
	CAtlList<CFFmpegMuxInputPin*> pActivePins;

	POSITION pos = m_pInputs.GetHeadPosition();
	while(pos)
	{
		CFFmpegMuxInputPin *pPin = m_pInputs.GetNext(pos);
		if(pPin->IsConnected()) pActivePins.AddTail(pPin);
	}

	pVideoPin  = NULL;
	pAudioPin = NULL ;

	pos = pActivePins.GetHeadPosition();

	while(pos)	
	{
		CFFmpegMuxInputPin * pTmp = pActivePins.GetNext(pos);
		CMediaType mt = pTmp->CurrentMediaType();
		if(mt.subtype == MEDIASUBTYPE_H264 
			|| mt.subtype == MEDIASUBTYPE_AVC 
			|| mt.subtype == MEDIASUBTYPE_YV12 
			|| mt.subtype == MEDIASUBTYPE_MKV_HEVC
			|| mt.subtype == WMMEDIASUBTYPE_I420)
		{
			pVideoPin = pTmp;	
			if(mt.subtype == MEDIASUBTYPE_H264 || mt.subtype == MEDIASUBTYPE_MKV_HEVC || mt.subtype == MEDIASUBTYPE_AVC )
			{
				if(mt.subtype == MEDIASUBTYPE_MKV_HEVC)
				{
					video_stream_copy = 2;
				}
				if(mt.subtype == MEDIASUBTYPE_H264  || mt.subtype == MEDIASUBTYPE_AVC )
				{
					video_stream_copy = 1;
				}
				
			}
		}
		if(mt.subtype == MEDIASUBTYPE_PCM)
		{
			pAudioPin = pTmp;
		}
	}

	if(m_oc == NULL)
	{
		if(m_QvodCallBack)
		{
#ifdef DEBUG_MUX
			OutputDebugString(L"m_oc == NULL/n");
#endif
			m_QvodCallBack->EventNotify_(QTC_MSG_TRANSCODE_ERROR,0,0);
		}
	}
	if(pAudioPin)
	{
		NewAudioStream(m_oc);
	}
	if(pVideoPin)
	{
		NewVideoStream(m_oc);
	}

	if(!pAudioPin && !pVideoPin)
	{
#ifdef DEBUG_MUX
		OutputDebugString(L"pAudioPin and  pVideoPin is NULL !\n");
#endif
		if(m_QvodCallBack)
		{	
			m_bError = true;
#ifdef DEBUG_MUX
			OutputDebugString(L"pAudioPin and  pVideoPin is NULL !\n");
#endif
			m_QvodCallBack->EventNotify_(QTC_MSG_TRANSCODE_ERROR,0,0);
		}
	}

	int ret = 0, i,  n,  nb_ostreams = 0;
	AVFormatContext *os;
	AVCodecContext *codec;
	ost =NULL;ost_table = NULL;


	nb_ostreams = 0;
	

	os = m_oc;
	if (!os->nb_streams) 
	{
		av_dump_format(os, 0, os->filename, 1);
	}
	nb_ostreams += os->nb_streams;

	ost_table = (AVOutputStream **)av_mallocz(sizeof(AVOutputStream *) * nb_ostreams);

	for(i=0;i<nb_ostreams;i++)
	{
		ost = (AVOutputStream*)av_mallocz(sizeof(AVOutputStream));
		ost_table[i] = ost;
	}
	
#define MAKE_SFMT_PAIR(a,b) ((a)+SAMPLE_FMT_NB*(b))
	n = 0;
    os = m_oc;
	for(i=0;i<nb_ostreams;i++,n++)
	{
		ost = ost_table[n];
		ost->file_index = 0;
		ost->index = i;
		ost->st = os->streams[i];

		ost->st->disposition =0;
		
		codec = ost->st->codec;
		codec->bits_per_raw_sample= 0;
		codec->chroma_sample_location =(AVChromaLocation)0;
		if (ost->st->stream_copy) 
		{			
			/* if stream_copy is selected, no need to decode or encode */
			if(pVideoPin == NULL)
			{
				m_bError = true;
#ifdef DEBUG_MUX
				OutputDebugString(L"acculate pVideoPin is NULL !\n");
#endif
				m_QvodCallBack->EventNotify_(QTC_MSG_TRANSCODE_ERROR,0,0);
			}
			if(video_stream_copy == 1)
			{
				codec->codec_id = CODEC_ID_H264;
			}
			if(video_stream_copy == 2)
			{
				//codec->codec_id = CODEC_ID_H265;
			}
			codec->codec_type = AVMEDIA_TYPE_VIDEO;
			ost->st->r_frame_rate = _AVRational(0,0);
			ost->st->sample_aspect_ratio =  _AVRational(1,1);
			codec->sample_aspect_ratio =  _AVRational(1,1);
			//codec->codec_tag = 34;
			codec->bit_rate = 0;
			codec->pix_fmt = PIX_FMT_YUV420P;
			codec->extradata= pVideoPin->GetExtradata();
			codec->extradata_size= pVideoPin->GetExtradataLength();
			//double vBitrate  = pVideoPin->GetVideoBitrate();
			float vBitrate  = m_vBitrate * 100 ;
			if(m_vBitrate <= 0 || m_vBitrate >= 30)
			{
#ifdef DEBUG_MUX
				OutputDebugString(L"m_TransConfig.VideoCodec.framerate = 25\n");
#endif	
				vBitrate = 2500;
			}
			codec->time_base = _AVRational(100,vBitrate);

			switch(codec->codec_type) 
			{
			case AVMEDIA_TYPE_AUDIO:
				break;
			case AVMEDIA_TYPE_VIDEO:
				codec->pix_fmt = PIX_FMT_YUV420P;
				codec->width =pVideoPin->GetVideoInfo().biWidth;
				codec->height = pVideoPin->GetVideoInfo().biHeight;
				codec->has_b_frames = 0;
				break;
			default:
				abort();
			}
		}
		else
		{
			switch(codec->codec_type) 
			{
			case AVMEDIA_TYPE_AUDIO:
				ost->fifo= av_fifo_alloc(1024);
				if(!ost->fifo)
				{
					if(m_QvodCallBack)
					{
						m_bError = true;
#ifdef DEBUG_MUX
						OutputDebugString(L"ost->fifo is NULL !\n");
#endif
						m_QvodCallBack->EventNotify_(QTC_MSG_TRANSCODE_ERROR,0,0);
					}
				}
					
				ost->reformat_pair = MAKE_SFMT_PAIR(SAMPLE_FMT_NONE,SAMPLE_FMT_NONE);

				if(m_TransConfig.AudioCodec.samplingrate == 0 
					|| m_TransConfig.AudioCodec.bitrate ==0
					|| m_TransConfig.AudioCodec.channel == 0)
				{
					m_TransConfig.AudioCodec.samplingrate = 44100;
					m_TransConfig.AudioCodec.bitrate = 64000;
					m_TransConfig.AudioCodec.channel = 2; 
					m_TransConfig.AudioCodec.CodecType = _A_AAC;

				}
				if((m_TransConfig.AudioCodec.CodecType == _A_AAC ||  m_TransConfig.AudioCodec.CodecType == _A_MP3) && m_TransConfig.AudioCodec.samplingrate <= 22050)
				{
					m_TransConfig.AudioCodec.samplingrate = 44100;
				}
				
				ost->st->codec->time_base= _AVRational(1,m_TransConfig.AudioCodec.samplingrate);
				ost->st->codec->sample_rate = m_TransConfig.AudioCodec.samplingrate;
				ost->st->codec->bit_rate = m_TransConfig.AudioCodec.bitrate;
				ost->st->codec->channels = m_TransConfig.AudioCodec.channel;

				if(m_TransConfig.AudioCodec.CodecType == _A_AAC)
				{
					ost->st->codec->codec_id = CODEC_ID_AAC;
				}
				if(m_TransConfig.AudioCodec.CodecType == _A_MP3)
				{
					ost->st->codec->codec_id = CODEC_ID_MP3;
				}
				if(m_TransConfig.AudioCodec.CodecType == _A_AC3)
				{
					ost->st->codec->bit_rate = 128000;
					//if(ab > )
					ost->st->codec->codec_id = CODEC_ID_AC3;
				}
				if(m_TransConfig.AudioCodec.CodecType == _A_WMA2)
				{
					ost->st->codec->codec_id = CODEC_ID_WMAV1;
				}
				if(m_TransConfig.AudioCodec.CodecType == _A_AMR_NB)
				{
					if(pAudioPin)
					{
							ost->st->codec->channels =1;		
					}
					ost->st->codec->codec_id = CODEC_ID_AMR_NB;
				}
				if(pAudioPin)
				{
					ost->audio_resample = ost->st->codec->sample_rate != pAudioPin->GetAudioInfo().nSamplesPerSec ;
					ost->encoding_needed = 1;
				}
				break;
			case AVMEDIA_TYPE_VIDEO:
				if (ost->st->codec->pix_fmt == PIX_FMT_NONE) 
				{
					if(m_QvodCallBack)
					{
						m_bError = true;
#ifdef DEBUG_MUX
					    OutputDebugString(L"ost->st->codec->pix_fmt == PIX_FMT_NONE/n");
#endif
						m_QvodCallBack->EventNotify_(QTC_MSG_TRANSCODE_ERROR,0,0);
					}
				}
				if(m_TransConfig.VideoCodec.width == 0
					|| m_TransConfig.VideoCodec.height == 0
					|| m_TransConfig.VideoCodec.bitrate == 0
					|| m_TransConfig.VideoCodec.framerate == 0)
				{
					m_TransConfig.VideoCodec.width = m_Width;
					m_TransConfig.VideoCodec.height = m_Height;
					m_TransConfig.VideoCodec.bitrate= 512000; 
					m_TransConfig.VideoCodec.CodecType = _V_H264;
					m_TransConfig.VideoCodec.framerate = 25;
				}


				if(pVideoPin)
				{
					m_TransConfig.VideoCodec.width = pVideoPin->GetVideoInfo().biWidth;
					m_TransConfig.VideoCodec.height = pVideoPin->GetVideoInfo().biHeight;	
				}
				
				ost->st->codec->width = m_TransConfig.VideoCodec.width;
				ost->st->codec->height =m_TransConfig.VideoCodec.height;

				codec->pix_fmt = PIX_FMT_YUV420P;
	

				ost->st->codec->bit_rate = m_TransConfig.VideoCodec.bitrate;
				ost->st->codec->time_base= _AVRational(1,m_TransConfig.VideoCodec.framerate);
				//ost->st->codec->time_base = _AVRational(100,2997);
				if(m_TransConfig.VideoCodec.CodecType == _V_H264)
				{
					if(thread_count >= 4)
					{
						thread_count = thread_count / 2;
					}
					avcodec_thread_init(ost->st->codec,thread_count);
					ost->st->codec->codec_id = CODEC_ID_H264;
				}
				if(m_TransConfig.VideoCodec.CodecType == _V_MPEG_4_Visual)
				{
					//ost->st->codec->flags |= CODEC_FLAG_QSCALE;
					//ost->st->codec->rc_max_rate = 500;
					//ost->st->codec->rc_min_rate = 100;
					//ost->st->codec->bit_rate = m_TransConfig.VideoCodec.bitrate  ;
					//ost->st->codec->rc_buffer_size=m_TransConfig.VideoCodec.bitrate ;
					//ost->st->codec->rc_initial_buffer_occupancy = ost->st->codec->rc_buffer_size*3/4;
					//ost->st->codec->rc_buffer_aggressivity= (float)1.0;
					avcodec_thread_init(ost->st->codec,2);	
					ost->st->codec->codec_id = CODEC_ID_MPEG4;
				}
				if(m_TransConfig.VideoCodec.CodecType == _V_H263)
				{
					ost->st->codec->codec_id = CODEC_ID_H263;
				}
				if(m_TransConfig.VideoCodec.CodecType == _V_WMV8)
				{
					ost->st->codec->codec_id = CODEC_ID_WMV1;
				}
				if(m_TransConfig.VideoCodec.CodecType == _V_MPEG_2)
				{
					ost->st->codec->codec_id = CODEC_ID_MPEG2VIDEO;
				}
				ost->video_resample = (m_TransConfig.VideoCodec.width != m_Width || m_TransConfig.VideoCodec.height != m_Height);
				if (0/*ost->video_resample*/) 
				{
					avcodec_get_frame_defaults(&ost->pict_tmp);
					if(avpicture_alloc((AVPicture*)&ost->pict_tmp, codec->pix_fmt,
						codec->width, codec->height)) 
					{
						if(m_QvodCallBack)
						{
							m_QvodCallBack->EventNotify_(QTC_MSG_TRANSCODE_ERROR,0,0);
						}
					}
					sws_flags = av_get_int(sws_opts, "sws_flags", NULL);
					ost->img_resample_ctx = sws_getContext(
						m_Width ,
						m_Height,
						codec->pix_fmt,
						codec->width ,
						codec->height ,
						codec->pix_fmt,
						sws_flags, NULL, NULL, NULL);
					if (ost->img_resample_ctx == NULL)
					{
						if(m_QvodCallBack)
						{
							m_QvodCallBack->EventNotify_(QTC_MSG_TRANSCODE_ERROR,0,0);
						}
					}

					ost->original_height = m_Height;
					ost->original_width  = m_Width;
					ost->resample_width = m_Width;
					ost->resample_height = m_Height;

					codec->bits_per_raw_sample= 0;
				}
				ost->resample_pix_fmt= PIX_FMT_YUV420P;
				
				if(pVideoPin && pVideoPin->CurrentMediaType().subtype == MEDIASUBTYPE_YV12)
				{
					ost->encoding_needed = 1;
				}
				else if(pVideoPin && pVideoPin->CurrentMediaType().subtype == WMMEDIASUBTYPE_I420){
					ost->encoding_needed = 1;
				}
				else
				{
					ost->encoding_needed = 0;
				}
				break;
			default:
				abort();
				break;
			}
		}
	}
    
	
	for(i=0;i<nb_ostreams;i++) 
	{
		ost = ost_table[i];
		if (ost->encoding_needed) 
		{
			AVCodec *codec = NULL;
			if (!codec)
				codec = avcodec_find_encoder(ost->st->codec->codec_id);
			if (!codec) 
			{
				if(m_QvodCallBack)
				{
					m_bError = true;
#ifdef DEBUG_MUX
					OutputDebugString(L" avcodec_find_encoder is failed !\n");
#endif
					m_QvodCallBack->EventNotify_(QTC_MSG_TRANSCODE_ERROR,0,0);
				}
			}
			if(ost->st->codec->codec_type == AVMEDIA_TYPE_VIDEO)
			{
				if(m_TransConfig.VideoCodec.CodecType == _V_H264)
				{
					ost->st->codec->priv_data  = av_mallocz(codec->priv_data_size);
					if (ost->st->codec->priv_data) {
						*(const AVClass **)ost->st->codec->priv_data = codec->priv_class;
						av_opt_set_defaults(ost->st->codec->priv_data);
					}
					int ret = av_set_string3(ost->st->codec->priv_data,"profile", "main",1,NULL);

					//根据分辨率设置编码等级
					if(ost->st->codec->width < 720)
					{
						ret = av_set_string3(ost->st->codec->priv_data,"level", "2.2",1,NULL);
					}
					else if(ost->st->codec->width < 1280)
					{
						ret = av_set_string3(ost->st->codec->priv_data,"level", "3.1",1,NULL);
					}
					else
					{
						ret = av_set_string3(ost->st->codec->priv_data,"level", "4.1",1,NULL);
					}
					ret = av_set_string3(ost->st->codec, "g", "30", 1, NULL);
					ret = av_set_string3(ost->st->codec, "keyint_min", "25", 1, NULL);
					ost->st->codec->b_frame_strategy  = 0;
					ost->st->codec->refs = 1;
				}
			}
			if (avcodec_open(ost->st->codec, codec) < 0) 
			{
				if(m_QvodCallBack)
				{
					m_bError = true;
#ifdef DEBUG_MUX
					OutputDebugString(L" avcodec_open is Failed !\n");
#endif
					m_QvodCallBack->EventNotify_(QTC_MSG_TRANSCODE_ERROR,0,0);
				}
			}		
			extra_size += ost->st->codec->extradata_size;
		}
	}

	if(pAudioPin && pVideoPin == NULL)
	{
		m_bEnableAudio = true;
		for(i=0;i<1;i++) 
		{
			os = m_oc;
			int i = -1;
			if ( i= av_write_header(os) < 0) 
			{
#ifdef DEBUG_MUX
				OutputDebugString(L" av_write_header is Failed !\n");
#endif
				//m_QvodCallBack->EventNotify_(QTC_MSG_TRANSCODE_ERROR,0,0);	
				goto dump_format;
			}
		}
	}
	
dump_format:
	for(i=0;i<1;i++) 
	{
		av_dump_format(os, i, os->filename, 1);
	}

	SetThreadPriority(m_hThread,THREAD_PRIORITY_NORMAL);

	BOOL bConvertTime = TRUE;

	while(1)
	{
		DWORD cmd = GetRequest();
		switch(cmd)
		{
		default:
		case CMD_EXIT:
			VideoBlock.RemoveAll(); 
			AudioBlock.RemoveAll();		
			CAMThread::m_hThread = NULL;
			Reply(S_OK);
			//OutputDebugString(L"CMD_EXIT \n"); 
			return 0;

		case CMD_RUN:
			Reply(S_OK);

			while(!CheckRequest(NULL))
			{
				if(m_State == State_Paused)
				{
					Sleep(10);
					continue;
				}
				if(m_nStreamNum==1)
				{
					if(pAudioPin&&pAudioPin->m_fEndOfStreamReceived)
					{
						if(m_QvodCallBack)
						{
							m_QvodCallBack->EventNotify_(QTC_MSG_PROGRESS,0,10000);
						}
						CAMThread::m_hThread = NULL;
						Reply(S_OK);

						return 0;
					}
					if(pVideoPin&&pVideoPin->m_fEndOfStreamReceived)
					{
						if(m_QvodCallBack)
						{			
							m_QvodCallBack->EventNotify_(QTC_MSG_PROGRESS,0,10000);
						}

						CAMThread::m_hThread = NULL;
						Reply(S_OK);

						return 0;
					}
				}
				else
				{
					if(pVideoPin&&pVideoPin->m_fEndOfStreamReceived  && pAudioPin&&pAudioPin->m_fEndOfStreamReceived
						&& AudioBlock.GetCount() == 0 && VideoBlock.GetCount() == 0)
					{
						if(CheckRequest(NULL))
						{
							continue;
						}

						if(m_QvodCallBack)
						{
							m_QvodCallBack->EventNotify_(QTC_MSG_PROGRESS,0,10000);
						}
						VideoBlock.RemoveAll(); 
						AudioBlock.RemoveAll();
						CAMThread::m_hThread = NULL;
						Reply(S_OK);
						return 0;
					}
				}
				
				
				if(pVideoPin&&pAudioPin)
				{
					size_t nVideoPackets = 0;
					size_t nAudioPackets = 0;
					CAutoPtr<BLOCK> video,audio;
					
					if(VideoBlock.GetCount() > 0)
					{
					}
					else
					{	
						//CAutoPtr<BLOCK> video;
						CAutoLock cLock(&m_csQueue);					
						nVideoPackets = pVideoPin->m_block.GetCount();
				
						if(nVideoPackets > 0)
						{
							vtemp = pVideoPin->m_block.GetHead()->start;
							if(bConvertTime)
							{
								bConvertTime = FALSE;
								if(m_TransStopTime > 0 && m_TransStopTime < m_rtDur )
								{
									m_rtDur = m_TransStopTime/* - 8000000*/ ;
								}
								else
								{
									m_rtDur = m_rtDur/* - 5000000*/ ;
								}
#ifdef DEBUG_MUX
								wchar_t str1[100];
								swprintf(str1, L"m_rtDur : %lld\n", m_rtDur);
								OutputDebugString(str1); 
#endif
							}				
						}

						while(nVideoPackets-- > 0)
						{
							video = pVideoPin->m_block.RemoveHead();
							VideoBlock.AddHead(video);
						}
					}

					if(AudioBlock.GetCount() > 0)
					{
					}
					else
					{		
						CAutoLock cLock(&m_csQueue);
						nAudioPackets = pAudioPin->m_block.GetCount();
						if(nAudioPackets > 0)
						{
							atemp =pAudioPin->m_block.GetHead()->start;
						}
						else
						{
							//mPackets++;
						}
						while(nAudioPackets-- > 0)
						{
							audio = pAudioPin->m_block.RemoveHead();
							AudioBlock.AddHead(audio);
						}
					}

					//CAutoPtr<BLOCK> video,audio;
					size_t m = VideoBlock.GetCount();
					size_t n = AudioBlock.GetCount();

					if(m > 0 && n >0)
					{
						if(atemp >  vtemp)
						{
							while(m-- >0)
							{
								video = VideoBlock.RemoveTail();		
								double Progress = ((double)video->start/(double)m_rtDur) *10000;
								
								if(m_Progress >= 10000)
								{
									m_Progress = 10000;
									pVideoPin->m_fEndOfStreamReceived  = TRUE;
								}
								if(m_Progress < 0)
								{
									m_Progress = 0;
								}
								if(m_QvodCallBack)
								{	
									if(Progress > m_Progress)
									{
										m_QvodCallBack->EventNotify_(QTC_MSG_PROGRESS,0,Progress);

										m_Progress = Progress;
									}								
								}	
							}
						}
						else 
						{
							while(n-->0)
							{
								audio = AudioBlock.RemoveTail();	
								double Progress = ((double)audio->start/(double)m_rtDur) *10000;
								if(m_Progress >= 10000)
								{
									m_Progress = 10000;
									pAudioPin->m_fEndOfStreamReceived  = TRUE;
								}
								if(m_Progress < 0)
								{
									m_Progress = 0;
								}
								if(m_QvodCallBack)
								{
									if(Progress > m_Progress)
									{
										m_QvodCallBack->EventNotify_(QTC_MSG_PROGRESS,0,Progress);

										m_Progress = Progress;
									}
								}	
							}
						}
					}	
					else
					{			
						if(pAudioPin->m_fEndOfStreamReceived || pVideoPin->m_fEndOfStreamReceived )
						{
							while(n-->0)
							{
								audio = AudioBlock.RemoveTail();	
								double Progress = ((double)audio->start/(double)m_rtDur) *10000;
								if(m_QvodCallBack)
								{
									if(Progress > m_Progress)
									{
										m_QvodCallBack->EventNotify_(QTC_MSG_PROGRESS,0,Progress);
										m_Progress = Progress;
									}
#ifdef DEBUG_MUX
										//wchar_t str1[100];
										//swprintf(str1, L"audio m_Progress : %llf\n", m_Progress);
										//OutputDebugString(str1); 
#endif
								}
								if(m_Progress >= 10000)
								{
									m_Progress = 10000;
									pAudioPin->m_fEndOfStreamReceived  = TRUE;
								}
								//nPackets = 0;
							}

							while(m-->0)
							{
								video = VideoBlock.RemoveTail();
								double Progress = ((double)video->start/(double)m_rtDur) *10000;

								if(m_Progress >= 10000)
								{
									m_Progress = 10000;
									pVideoPin->m_fEndOfStreamReceived  = TRUE;
								}
								if(m_Progress < 0)
								{
									m_Progress = 0;
								}
								if(m_QvodCallBack)
								{
									if(Progress > m_Progress)
									{
										m_QvodCallBack->EventNotify_(QTC_MSG_PROGRESS,0,Progress);
										m_Progress = Progress;
									}								
#ifdef DEBUG_MUX
									//wchar_t str1[100];
									//swprintf(str1, L"video m_Progress : %llf\n", m_Progress);
									//OutputDebugString(str1); 
#endif
								}
							}			
							continue;
						}
						else
						{
							Sleep(1);
						}
					}	
				}
				else if(pAudioPin && pVideoPin == NULL)//单音频流
				{
					size_t nAudioPackets = 0;
					CAutoPtr<BLOCK> audio;
					if(AudioBlock.GetCount() > 0)
					{
					}
					else
					{
						
						CAutoLock cLock(&m_csQueue);
						nAudioPackets = pAudioPin->m_block.GetCount();
	
						while(nAudioPackets-- > 0)
						{
							audio = pAudioPin->m_block.RemoveHead();
							AudioBlock.AddHead(audio);
						}
					}
					size_t n = AudioBlock.GetCount();
					if(n>0)
					{
						while(n-->0)
						{
							audio = AudioBlock.RemoveTail();	
							m_Progress = ((double)audio->start/(double)m_rtDur) *10000;
							if(m_Progress >= 10000)
							{
								m_Progress = 10000;
								pAudioPin->m_fEndOfStreamReceived  = TRUE;
							}
							if(m_Progress < 0)
							{
								m_Progress = 0;
							}

							if(m_QvodCallBack)
							{
								m_QvodCallBack->EventNotify_(QTC_MSG_PROGRESS,0,m_Progress);
							}
						}
					}
					else
					{
						Sleep(1);
					}
				}
				else if(pVideoPin && pAudioPin == NULL)
				{
					CAutoLock cVideoLock(&m_csQueue);
					if(int n= pVideoPin->m_block.GetCount() > 0)
					{
						while(n-- > 0)
						{
							CAutoPtr<BLOCK> video;
							video = pVideoPin->m_block.RemoveHead();									
							m_Progress = ((double)video->start/(double)m_rtDur) *10000;
							if(m_QvodCallBack)
							{
								m_QvodCallBack->EventNotify_(QTC_MSG_PROGRESS,0,m_Progress);
							}					
						}
					}
				}
			}
		}
	}
	ASSERT(0); 
	CAMThread::m_hThread = NULL;
	return 0;
}

HRESULT CFFmpegMuxFilter::OutputFile(const char *filename)
{
	AVFormatContext *oc;
	int err;

	AVFormatParameters params, *ap = &params;
	AVOutputFormat *file_oformat;

	if (!strcmp(filename, "-"))
		filename = "pipe:";

	oc = avformat_alloc_context();
	if (!oc) {
#ifdef DEBUG_MUX
		OutputDebugString(L"avformat_alloc_context fail\n");
#endif
		return E_FAIL;
	}


	file_oformat = av_guess_format(NULL, filename, NULL);

	if (!file_oformat) {
#ifdef DEBUG_MUX
		OutputDebugString(L"av_guess_format fail\n");
#endif
		return E_FAIL;
	}

	oc->oformat = file_oformat;
	av_strlcpy(oc->filename, filename, sizeof(oc->filename));

	/* check filename in case of an image number is expected */
	if (oc->oformat->flags & AVFMT_NEEDNUMBER) {
		if (!av_filename_number_test(oc->filename)) {
#ifdef DEBUG_MUX
			OutputDebugString(L"av_filename_number_test fail\n");
#endif
			return E_FAIL;
		}
	}

	if (!(oc->oformat->flags & AVFMT_NOFILE)) 
	{
		/* open the file */
		if ((err = avio_open(&oc->pb, filename, AVIO_FLAG_WRITE)) < 0/*err = url_fopen(&oc->pb, filename, URL_WRONLY)) < 0*/) {
#ifdef DEBUG_MUX
			OutputDebugString(L"avio_open fail\n");
#endif
			return E_FAIL;
		}
	}

	memset(ap, 0, sizeof(*ap)); 
	if (av_set_parameters(oc, ap) < 0) {
#ifdef DEBUG_MUX
		OutputDebugString(L"av_set_parameters fail\n");
#endif
		return E_FAIL;
	}

	oc->preload= (int)(mux_preload*AV_TIME_BASE);
	oc->max_delay= (int)(mux_max_delay*AV_TIME_BASE);
	oc->loop_output = loop_output;
	oc->flags |= AVFMT_FLAG_NONBLOCK;
	m_oc = oc;
	return S_OK;
}


void CFFmpegMuxFilter::SetWidth(int *width)
{
	m_Width = *width;
}
void CFFmpegMuxFilter::SetHeight(int *height)
{
	m_Height = *height;
}

void CFFmpegMuxFilter::GetFileDuration(REFERENCE_TIME *ll)
{
	m_rtDur = *ll;
}

void CFFmpegMuxFilter::AddStream()
{
	m_nStreamNum++;
}

int  CFFmpegMuxFilter::GetStreamCount()
{
	return m_nStreamNum;
}

void CFFmpegMuxFilter::NewVideoStream(AVFormatContext *oc)
{
	AVStream *st;
	AVCodecContext *video_enc;
	enum CodecID codec_id;
	AVCodec *codec= NULL;

	st = av_new_stream(oc, oc->nb_streams);
	if (!st) 
	{
		return;
	}
	avcodec_get_context_defaults2(st->codec, AVMEDIA_TYPE_VIDEO);
	avcodec_thread_init(st->codec,1);


	video_enc = st->codec;
	
	if(   (video_global_header&1)
		|| (video_global_header==0 && (oc->oformat->flags & AVFMT_GLOBALHEADER)))
	{
			video_enc->flags |= CODEC_FLAG_GLOBAL_HEADER;
	}

	if (video_stream_copy) 
	{
		st->stream_copy = 1;
		video_enc->codec_type = AVMEDIA_TYPE_VIDEO;
		video_enc->sample_aspect_ratio =
			st->sample_aspect_ratio = av_d2q(frame_aspect_ratio*frame_height/frame_width, 255);
	} 
	else 
	{
		AVRational fps= frame_rate.num ? frame_rate : _AVRational(25,1);
		{
			codec_id = av_guess_codec(oc->oformat, NULL, oc->filename, NULL, AVMEDIA_TYPE_VIDEO);
			codec = avcodec_find_encoder(codec_id);
		}

		video_enc->codec_id = codec_id;

		if (codec && codec->supported_framerates )
			fps = codec->supported_framerates[av_find_nearest_q_idx(fps, codec->supported_framerates)];
		video_enc->time_base.den = fps.num;
		video_enc->time_base.num = fps.den;

		video_enc->width = 400/*frame_width + frame_padright + frame_padleft*/;
		video_enc->height = 300/*frame_height + frame_padtop + frame_padbottom*/;
		video_enc->sample_aspect_ratio = av_d2q(frame_aspect_ratio*video_enc->height/video_enc->width, 255);
		video_enc->pix_fmt = frame_pix_fmt;
		st->sample_aspect_ratio = video_enc->sample_aspect_ratio;
		choose_pixel_fmt(st, codec);

		if (!video_enc->rc_initial_buffer_occupancy)
			video_enc->rc_initial_buffer_occupancy = video_enc->rc_buffer_size*3/4;
		video_enc->me_threshold= me_threshold;
		video_enc->intra_dc_precision= intra_dc_precision - 8;
	}

	/* reset some key parameters */
	video_able = 0;
	//video_stream_copy = 0;
	frame_pix_fmt = PIX_FMT_NONE;
}

void CFFmpegMuxFilter::NewAudioStream(AVFormatContext *oc)
{
	AVStream *st;
	AVCodecContext *audio_enc;
	enum CodecID codec_id;

	st = av_new_stream(oc, oc->nb_streams);
	if (!st) 
	{
		return ;
	}
	avcodec_get_context_defaults2(st->codec, AVMEDIA_TYPE_AUDIO);

	avcodec_thread_init(st->codec,1);

	audio_enc = st->codec;
	audio_enc->codec_type = AVMEDIA_TYPE_AUDIO;

	if (oc->oformat->flags & AVFMT_GLOBALHEADER) 
	{
		audio_enc->flags |= CODEC_FLAG_GLOBAL_HEADER;
	}
	{
		AVCodec *codec;
		{
			codec_id = av_guess_codec(oc->oformat, NULL, oc->filename, NULL, AVMEDIA_TYPE_AUDIO);
			codec = avcodec_find_encoder(codec_id);
		}
		audio_enc->codec_id = codec_id;

		audio_enc->channels = audio_channels;
		audio_enc->sample_fmt = audio_sample_fmt;
		audio_enc->sample_rate = audio_sample_rate;
		audio_enc->channel_layout = channel_layout;
		if (avcodec_channel_layout_num_channels(channel_layout) != audio_channels)
			audio_enc->channel_layout = 0;
		choose_sample_fmt(st, codec);
		choose_sample_rate(st, codec);
	}

	audio_enc->time_base= _AVRational(1, audio_sample_rate);

	audio_able = 0;

	audio_stream_copy = 0;
}

void CFFmpegMuxFilter::choose_sample_fmt(AVStream *st, AVCodec *codec)
{
	if(codec && codec->sample_fmts)
	{
		const enum SampleFormat *p= codec->sample_fmts;
		for(; *p!=-1; p++)
		{
			if(*p == st->codec->sample_fmt)
				break;
		}
		if(*p == -1)
			st->codec->sample_fmt = codec->sample_fmts[0];
	}
}

void CFFmpegMuxFilter::choose_sample_rate(AVStream *st, AVCodec *codec)
{
	if(codec && codec->supported_samplerates)
	{
		const int *p= codec->supported_samplerates;
		int best;
		int best_dist=INT_MAX;
		for(; *p; p++)
		{
			int dist= abs(st->codec->sample_rate - *p);
			if(dist < best_dist)
			{
				best_dist= dist;
				best= *p;
			}
		}
		st->codec->sample_rate= best;
	}
}

enum CodecID CFFmpegMuxFilter::find_codec_or_die(const char *name, int type, int encoder, int s)
{
	const char *codec_string = encoder ? "encoder" : "decoder";
	AVCodec *codec;

	if(!name)
		return CODEC_ID_NONE;
	codec = encoder ?
		avcodec_find_encoder_by_name(name) :
	avcodec_find_decoder_by_name(name);
	if(!codec) {
		return CODEC_ID_NONE;
	}
	if(codec->type != type) {
		return CODEC_ID_NONE;
	}
	if(codec->capabilities & CODEC_CAP_EXPERIMENTAL &&
		s > FF_COMPLIANCE_EXPERIMENTAL) {
			codec = encoder ?
				avcodec_find_encoder(codec->id) :
			avcodec_find_decoder(codec->id);
			if (!(codec->capabilities & CODEC_CAP_EXPERIMENTAL))
			{
				return CODEC_ID_NONE;
			}
	}
	return codec->id;
}

void CFFmpegMuxFilter::choose_pixel_fmt(AVStream *st, AVCodec *codec)
{
	if(codec && codec->pix_fmts){
		const enum PixelFormat *p= codec->pix_fmts;
		for(; *p!=-1; p++){
			if(*p == st->codec->pix_fmt)
				break;
		}
		if(*p == -1
			&& !(   st->codec->codec_id==CODEC_ID_MJPEG
			&& st->codec->strict_std_compliance <= -1
			&& (   st->codec->pix_fmt == PIX_FMT_YUV420P
			|| st->codec->pix_fmt == PIX_FMT_YUV422P)))
			st->codec->pix_fmt = codec->pix_fmts[0];
	}
}




