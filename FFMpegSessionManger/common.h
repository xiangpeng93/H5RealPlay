#pragma once
#ifndef __COMMON__
#define __COMMON__

extern "C"
{
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h> 
#include "libavutil/avutil.h"  
#include "libavutil/pixfmt.h"  
#include "libswscale/swscale.h"  
#include "libavformat/avio.h"  
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#pragma comment (lib, "Ws2_32.lib")  
#pragma comment (lib, "avcodec.lib")
#pragma comment (lib, "avdevice.lib")
#pragma comment (lib, "avfilter.lib")
#pragma comment (lib, "avformat.lib")
#pragma comment (lib, "avutil.lib")
#pragma comment (lib, "swresample.lib")
#pragma comment (lib, "swscale.lib")
#pragma comment (lib, "postproc.lib")
}

#include <map>
#include <mutex>
#include <queue>
#include <thread>
using namespace std;

#define MAX_SESSION 1000

class AutoLock
{
public:
	AutoLock(std::mutex &mux)
	{
		m_mutex = &mux;
		(*m_mutex).lock();
	}
	~AutoLock()
	{
		(*m_mutex).unlock();
	}
private:
	std::mutex *m_mutex;
};

#endif
