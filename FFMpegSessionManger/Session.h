#pragma once
#include "common.h"
#include "FFMpegSessionManger.h"
class Session
{
public:
	Session();
	~Session();
public:
	int InitSession();
	int FiniSession();
	int Start();	
	int Stop();
	int SetCallBackFunc(CallBackData callBack);
	int SetFileNameCallBackFunc(FileNameCallBack callBack);
	int SendDataToQueue(uint8_t* buffer, int len);
	
public:
	//码流数据缓存
	queue<pair<uint8_t *, int>> m_dataQueue;
	//mutex 锁 支持多线程
	mutex m_lockMutex;

private:
	static int read_data(void *opaque, uint8_t *buf, int buf_size);
	int decode_packet(int *got_frame, int cached, AVCodecContext *video_dec_ctx, AVFrame *frame, AVPacket &pkt);
	int encode_frame(AVCodecContext *decodeContext, AVFrame *frame);
	void initEncode(AVCodecContext *decodecContext);


	static int wirte_data(void * opaque, uint8_t *buf, int bufsize);
	void mainThread();
private:
	//read runtime data
	AVFormatContext *m_ifmt_ctx;
	AVIOContext *m_iavio_ctx;
	uint8_t *m_avio_ctx_buffer;
	int m_avio_ctx_buffer_size;
	int m_videoindex;

	//mp4 file
	AVFormatContext* pMp4Format;
	AVOutputFormat* pMp4OFormat;
	//encode 
	AVCodec *m_encodec;
	AVCodecContext *m_encodecC;
	AVFormatContext *m_ofmt_ctx;
	AVIOContext *m_encodeAvio;

	//传入回调
	CallBackData m_callbackFunc;
	FileNameCallBack m_fileNameCallbackFunc;
	
	thread m_Thread;
	bool m_exitThread;
	time_t m_timeStart;
	int64_t m_pts;
	int64_t m_dts;
	int64_t m_durtion;
	int64_t m_pos;

	AVStream *m_out_stream;

	string m_fileName;
};

