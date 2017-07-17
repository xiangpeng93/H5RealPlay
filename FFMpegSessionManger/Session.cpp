#include "stdafx.h"
#include "Session.h"



Session::Session() :
m_videoindex(-1),
m_avio_ctx_buffer(NULL),
m_ifmt_ctx(NULL),
m_iavio_ctx(NULL),
m_encodec(NULL),
m_encodecC(NULL),
m_ofmt_ctx(NULL),
m_encodeAvio(NULL),
m_exitThread(false)
{
	InitSession();
};

Session::~Session()
{
	
};

int Session::InitSession()
{
	int ret = 0;
	if (!(m_ifmt_ctx = avformat_alloc_context())) {
		ret = AVERROR(ENOMEM);
		goto end;
	}
	m_avio_ctx_buffer_size = 20 * 1024;
	m_avio_ctx_buffer = (uint8_t *)av_malloc(m_avio_ctx_buffer_size);
	if (!m_avio_ctx_buffer) {
		ret = AVERROR(ENOMEM);
		goto end;
	}
	m_iavio_ctx = avio_alloc_context(m_avio_ctx_buffer, m_avio_ctx_buffer_size,
		0, NULL, &Session::read_data, NULL, NULL);
	if (!m_iavio_ctx) {
		ret = AVERROR(ENOMEM);
		goto end;
	}
	m_ifmt_ctx->pb = m_iavio_ctx;
	m_ifmt_ctx->pb->opaque = this;


	pMp4Format = NULL;
	pMp4OFormat = NULL;

	m_timeStart = 0;
	m_pts = m_dts = 0;
end:
	return ret;
}

int Session::FiniSession()
{
	if (m_ifmt_ctx != NULL)
	{
		avformat_free_context(m_ifmt_ctx);
		m_ifmt_ctx = NULL;
	}

	if (m_iavio_ctx != NULL)
	{
		av_freep(&m_iavio_ctx->buffer);
		m_iavio_ctx->buffer = NULL;

		av_freep(&m_iavio_ctx);
		m_iavio_ctx = NULL;
	}
	printf("m_dataQueue num %d .\n", m_dataQueue.size());

	while (m_dataQueue.size() > 0)
	{
		delete[] m_dataQueue.front().first;
		m_dataQueue.pop();
	}

	if (m_encodeAvio != NULL)
	{
		av_freep(&m_encodeAvio->buffer);
		m_encodeAvio->buffer = NULL;

		av_freep(&m_encodeAvio);
		m_encodeAvio = NULL;
	}

	if (m_encodecC != NULL)
	{
		avcodec_free_context(&m_encodecC);
		m_encodecC = NULL;
	}

	if (pMp4Format->pb != NULL)
	{
		av_write_trailer(pMp4Format);
		avio_close(pMp4Format->pb);
		pMp4Format->pb = NULL;
		if (m_fileNameCallbackFunc)
			m_fileNameCallbackFunc(m_fileName.c_str());
	}
	if (pMp4Format != NULL)
	{
		avformat_free_context(pMp4Format);
		pMp4Format = NULL;
	}

	/*if (m_ofmt_ctx != NULL)
	{
		avformat_free_context(m_ofmt_ctx);
		m_ofmt_ctx = NULL;
	}*/

	
	

	return 0;
};

int Session::Start()
{
	printf("开始会话. \n");

	m_Thread = std::thread(&Session::mainThread, this);
	m_Thread.detach();
	return 0;
};

int Session::Stop()
{
	printf("停止会话. \n");
	m_exitThread = true;

	do{
		Sleep(100);
	} while (m_exitThread == true);
	/*m_iavio_ctx->read_packet = NULL;
	m_encodeAvio->write_packet = NULL;*/

	printf("停止线程. \n");
	FiniSession();
	return 0;
}

int Session::SetCallBackFunc(CallBackData callBack)
{
	if (callBack != NULL)
		m_callbackFunc = callBack;
	return 0;
};


int Session::SetFileNameCallBackFunc(FileNameCallBack callBack)
{
	if (callBack != NULL)
		m_fileNameCallbackFunc = callBack;
	return 0;
};


int Session::SendDataToQueue(uint8_t* buffer, int len)
{
	AutoLock lock(m_lockMutex);
	if (m_dataQueue.size() > 1000)
		printf("m_dataQueue is overload. \n");
	
	uint8_t *temp = new uint8_t[len];
	memcpy(temp, buffer, len);
	m_dataQueue.push(pair<uint8_t *, int>(temp, len));
	return 0;
}


int Session::read_data(void *opaque, uint8_t *buf, int buf_size)
{
	Session *session = (Session*)opaque;	

head:
	if (session->m_exitThread)
	{
		printf("接收退出消息，返回 -1. \n");
		return 0;
	}

	if (!session->m_dataQueue.empty())
	{
		AutoLock lock(session->m_lockMutex);
		memcpy(buf, session->m_dataQueue.front().first, session->m_dataQueue.front().second);
		buf_size = session->m_dataQueue.front().second;

		delete[] session->m_dataQueue.front().first;
		session->m_dataQueue.pop();

		return buf_size;
	}
	else
	{
		Sleep(50);
		goto head;
	}
	return 0;
}


void Session::mainThread()
{
	int ret = 0;
	AVCodecContext  *pCodecCtx = NULL;
	//AVFrame * frame = NULL;
	//打开输入,开始执行解析码流
	printf("打开输入流. \n");
	ret = avformat_open_input(&m_ifmt_ctx, NULL, NULL, NULL);
	if (ret < 0) {
		fprintf(stderr, "Could not open input. (无法打开输入)\n");
		goto end;
	}
	//寻找码流信息
	ret = avformat_find_stream_info(m_ifmt_ctx, NULL);
	if (ret < 0) {
		fprintf(stderr, "Could not find stream information. (未找到流信息)\n");
		goto end;
	}
	//输出码流信息
	av_dump_format(m_ifmt_ctx, 0, NULL, 0);

	for (int i = 0; i < (int)m_ifmt_ctx->nb_streams; i++) //一般情况下，一个媒体只有两个流，视频和音频流即streams[0],stream[1]  
	{
		if (m_ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			printf("找到视频流, %d\n", i);
			m_videoindex = i;//在nb_streams视频流的索引  
			break;
		}
	}

	if (m_videoindex == -1)
	{
		printf("Didn't find a video stream.（没有找到视频流）\n");
		goto end;
	}

	pCodecCtx = m_ifmt_ctx->streams[m_videoindex]->codec;
	AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL)
	{
		printf("Codec not found.（没有找到解码器）\n");
		goto end;
	}
	else
	{
		printf("找到解码器, %d .\n", pCodecCtx->codec_id);
	}

	if (avcodec_open2(pCodecCtx, pCodec, NULL)<0)
	{
		printf("Could not open codec.（无法打开解码器）\n");
		goto end;
	}
	else
	{
		printf("Open Decode Success. (打开解码器成功)! \n");
	}

	//frame = av_frame_alloc();
	//if (!frame) {
	//	fprintf(stderr, "Could not allocate frame. (无法进行分配)\n");
	//	ret = AVERROR(ENOMEM);
	//	goto end;
	//}

	/* initialize packet, set data to NULL, let the demuxer fill it */
	AVPacket g_pkt;
	av_init_packet(&g_pkt);
	g_pkt.data = NULL;
	g_pkt.size = 0;

	/* read frames from the memory */
	while (av_read_frame(m_ifmt_ctx, &g_pkt) >= 0) {
		//线程退出标记
		
		if (this->m_exitThread == true)
		{
			printf("退出线程. \n");
			break;
		}
		do {

			if (pMp4OFormat == NULL)
			{
				initEncode(pCodecCtx);
			}
			
			time_t timeNow = time(0);
			if (timeNow - m_timeStart > 5 && g_pkt.flags == 1)
			{
				m_pts = 0;
				m_dts = 0;
				m_timeStart = 0;
				if (pMp4Format->pb != NULL)
				{
					av_write_trailer(pMp4Format);
					avio_close(pMp4Format->pb);
					pMp4Format->pb = NULL;
					if (m_fileNameCallbackFunc)
						m_fileNameCallbackFunc(m_fileName.c_str());
				}

				m_timeStart = time(0);
				AVIOContext *pb = NULL;
				char fileName[256] = { 0 };
				sprintf(fileName, "%d.mp4", m_timeStart);
				m_fileName = fileName;
				ret = avio_open(&pb, fileName, AVIO_FLAG_READ_WRITE);
				if (ret < 0){
					printf("avio_open error");
				}
				else
				{
					//printf("avio_open ------------------------%s \r\n", fileName);
				}
				pMp4Format->pb = pb;
				if (avformat_write_header(pMp4Format, NULL) < 0)
				{
				}
			}
			AVStream *in_stream = m_ifmt_ctx->streams[m_videoindex];
			AVStream *out_stream = pMp4Format->streams[m_videoindex];

			g_pkt.pts = av_rescale_q_rnd(g_pkt.pts, in_stream->time_base, out_stream->time_base, AV_ROUND_DOWN);
			g_pkt.dts = av_rescale_q_rnd(g_pkt.dts, in_stream->time_base, out_stream->time_base, AV_ROUND_DOWN);
			g_pkt.duration = av_rescale_q(g_pkt.duration, in_stream->time_base, out_stream->time_base);
			g_pkt.pos = -1;

			if (m_pts == 0)
			{
				m_pts = g_pkt.pts;
				m_dts = g_pkt.dts;
				m_durtion = g_pkt.duration;
				m_pos = g_pkt.pos;
				pMp4Format->streams[0]->cur_dts = pMp4Format->streams[0]->first_dts;
				pMp4Format->streams[0]->nb_frames = 0;
			}

			g_pkt.pts -= m_pts;
			g_pkt.dts -= m_dts;

			if (pMp4Format->pb != NULL)
				ret = av_interleaved_write_frame(pMp4Format, &g_pkt);
			if (ret < 0)
			{
				//printf("av_interleaved_write_frame ------------------------%d \r\n", ret);
			}
			
			/*int got_frame = 0;
			ret = decode_packet(&got_frame, 0, pCodecCtx, frame, g_pkt);
			if (ret < 0)
				break;
			g_pkt.data += ret;
			g_pkt.size -= ret;
			*/
			
		} while (g_pkt.size > 0);
		av_packet_unref(&g_pkt);
	}
	
end:
	if (&g_pkt)
	{
		av_free_packet(&g_pkt);
	}
	if (pCodecCtx)
		avcodec_close(pCodecCtx);
	/*if (frame)
		av_frame_free(&frame);*/
	if (m_ifmt_ctx->iformat)
		avformat_close_input(&m_ifmt_ctx);
	printf("释放线程资源. \n");
	this->m_exitThread = false;
	return;
};


int Session::decode_packet(int *got_frame, int cached, AVCodecContext *video_dec_ctx, AVFrame *frame, AVPacket &pkt)
{
	int ret = 0;
	int decoded = pkt.size;

	*got_frame = 0;

	if (pkt.stream_index == m_videoindex) {
		/* decode video frame */
		ret = avcodec_decode_video2(video_dec_ctx, frame, got_frame, &pkt);
		if (ret < 0) {
			printf("Error decoding video frame\n");
			return ret;
		}
		if (*got_frame) {
			if (frame->width != video_dec_ctx->width || frame->height != video_dec_ctx->height ||
				frame->format != video_dec_ctx->pix_fmt) {
				/* To handle this change, one could call av_image_alloc again and
				* decode the following frames into another rawvideo file. */
				fprintf(stderr, "Error: Width, height and pixel format have to be "
					"constant in a rawvideo file, but the width, height or "
					"pixel format of the input video changed:\n"
					"old: width = %d, height = %d, format = %s\n"
					"new: width = %d, height = %d, format = %s\n",
					video_dec_ctx->width, video_dec_ctx->height, av_get_pix_fmt_name(video_dec_ctx->pix_fmt),
					frame->width, frame->height,
					av_get_pix_fmt_name((AVPixelFormat)frame->format));
				return -1;
			}

			/*if (m_pts == 0)
			{
				m_pts = frame->pts;
				m_dts = frame->pkt_dts;
			}
			frame->pts -= m_pts;
			frame->pkt_pts -= m_pts;
			frame->pkt_dts -= m_dts;
			frame->pkt_pos = -1;
			frame->pkt_duration = 0;*/

			//encode_frame(video_dec_ctx, frame);
		}
	}

	/* If we use frame reference counting, we own the data and need
	* to de-reference it when we don't use it anymore */
	if (*got_frame)
		av_frame_unref(frame);

	return decoded;
};


int Session::encode_frame(AVCodecContext *decodeContext, AVFrame *frame)
{
	int ret = 0;
	if (m_encodecC == NULL)
	{
		initEncode(decodeContext);
	}


	AVPacket *avpktencode = av_packet_alloc();
	av_init_packet(avpktencode);
	avpktencode->data = NULL;    // packet data will be allocated by the encoder
	avpktencode->size = 0;

	int got_frame = 0;
	ret = avcodec_encode_video2(m_encodecC, avpktencode, frame, &got_frame);
	if (ret < 0) {
		fprintf(stderr, "Error encoding frame\n");
		return -1;
	}

	if (got_frame) {
		
		m_callbackFunc(avpktencode->data, avpktencode->size);

		if (m_timeStart == 0)
		{
			m_timeStart = time(0);
			AVIOContext *pb = NULL;
			char fileName[256] = { 0 };
			sprintf(fileName, "%d.mp4", m_timeStart);
			ret = avio_open(&pb, fileName, AVIO_FLAG_READ_WRITE);
			if (ret < 0){
				printf("avio_open error");
			}
			else
			{
				printf("avio_open ------------------------%s \r\n", fileName);
			}
			pMp4Format->pb = pb;
			if (avformat_write_header(pMp4Format, NULL) < 0)
			{
			}
		}
	
		AVStream *in_stream = m_ifmt_ctx->streams[m_videoindex];
		AVStream *out_stream = pMp4Format->streams[m_videoindex];

		avpktencode->pts = av_rescale_q_rnd(avpktencode->pts, in_stream->time_base, out_stream->time_base, AV_ROUND_DOWN);
		avpktencode->dts = av_rescale_q_rnd(avpktencode->dts, in_stream->time_base, out_stream->time_base, AV_ROUND_DOWN);
		avpktencode->duration = av_rescale_q(avpktencode->duration, in_stream->time_base, out_stream->time_base);
		avpktencode->pos = -1;


		if (m_pts == 0)
		{
			m_pts = avpktencode->pts;
			m_dts = avpktencode->dts;
		}
		avpktencode->pts -= m_pts;
		avpktencode->dts -= m_dts;

		time_t timeNow = time(0);
		if (timeNow - m_timeStart > 3)
		{
			m_pts = 0;
			m_dts = 0;
			m_timeStart = 0;
			
			av_write_trailer(pMp4Format);
			avio_close(pMp4Format->pb);
			pMp4Format->pb = NULL;

			/*if (pMp4Format != NULL)
			{
				avformat_free_context(pMp4Format);
				pMp4Format = NULL;
			}

			if (avformat_alloc_output_context2(&pMp4Format, NULL, "mp4", NULL) < 0)
			{
				return -1;
			}
			pMp4OFormat = pMp4Format->oformat;
			m_out_stream = avformat_new_stream(pMp4Format, m_encodec);
			if (!m_out_stream) {
				return -1;
			}
			ret = avcodec_copy_context(m_out_stream->codec, m_encodecC);
			if (ret < 0) {
				fprintf(stderr, "Failed to copy context from input to output stream codec context\n");
				return -1;
			}*/
			out_stream->last_in_packet_buffer = NULL;
			out_stream->index = 0;
			memset(out_stream->info, 0, sizeof(out_stream->info));
			out_stream->cur_dts = out_stream->first_dts;
			out_stream->priv_pts = NULL;
			out_stream->nb_frames = 0;
			out_stream->pts_wrap_bits = 0;
		}
		//else
		{
			if (pMp4Format->pb != NULL)
				ret = av_interleaved_write_frame(pMp4Format, avpktencode);
			if (ret < 0)
			{
				printf("av_interleaved_write_frame ------------------------%d \r\n", ret);
			}
		}
		//ret = av_interleaved_write_frame(m_ofmt_ctx, avpktencode);
		av_packet_unref(avpktencode);
	}
	if (avpktencode)
	{
		av_free_packet(avpktencode);
	}
	return 0;
}

void Session::initEncode(AVCodecContext *decodecContext)
{
	int ret;

	m_encodec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!m_encodec) {
		fprintf(stderr, "Codec not found\n");
		return;
	}
	m_encodecC = avcodec_alloc_context3(m_encodec);
	if (!m_encodecC) {
		fprintf(stderr, "Could not allocate video codec context\n");
		return;
	}
	m_encodecC->bit_rate = decodecContext->bit_rate;
	m_encodecC->width = decodecContext->width;
	m_encodecC->height = decodecContext->height;
	AVRational timebase = { 1, 25 };
	m_encodecC->time_base = decodecContext->time_base;;
	//m_encodecC->gop_size = decodecContext->gop_size;
	//m_encodecC->max_b_frames = decodecContext->max_b_frames;
	m_encodecC->pix_fmt = decodecContext->pix_fmt;

	AVDictionary * opts = NULL;
	//av_dict_set(&opts, "profile", "baseline", 0);

	if (avcodec_open2(m_encodecC, m_encodec, &opts) < 0) {
		fprintf(stderr, "Could not open codec\n");
		return;
	}

	//mp4 file
	if (avformat_alloc_output_context2(&pMp4Format, NULL, "mp4", NULL) < 0)
	{
		return;
	}
	pMp4OFormat = pMp4Format->oformat;
	m_out_stream = avformat_new_stream(pMp4Format, m_encodec);
	if (!m_out_stream) {
		return;
	}
	ret = avcodec_copy_context(m_out_stream->codec, m_encodecC);
	if (ret < 0) {
		fprintf(stderr, "Failed to copy context from input to output stream codec context\n");
		return;
	}

	/*if (avio_open(&(pMp4Format->pb), "outputmp4.mp4", AVIO_FLAG_READ_WRITE) < 0)
	{
		return;
	}*/

	/*if (avformat_write_header(pMp4Format, NULL) < 0)
	{
		return;
	}*/

	//avformat_alloc_output_context2(&m_ofmt_ctx, NULL, "mpegts", NULL);
	//avformat_alloc_output_context2(&m_ofmt_ctx, NULL, NULL, "nothing");
	//if (!m_ofmt_ctx) {
	//	printf("Could not create output context\n");
	//	ret = AVERROR_UNKNOWN;
	//	return;
	//}

	////根据输入流创建输出流（Create output AVStream according to input AVStream）
	//AVStream *out_stream = avformat_new_stream(m_ofmt_ctx, m_encodecC->codec);
	//if (!out_stream) {
	//	printf("Failed allocating output stream\n");
	//	ret = AVERROR_UNKNOWN;
	//}
	////复制AVCodecContext的设置（Copy the settings of AVCodecContext）
	//ret = avcodec_copy_context(out_stream->codec, m_encodecC);
	//if (ret < 0) {
	//	printf("Failed to copy context from input to output stream codec context\n");
	//}
	//out_stream->codec->codec_tag = 0;
	//if (m_ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
	//	out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
	//
	//uint8_t *recvBuff = (uint8_t *)av_malloc(50 * 1024);
	//m_encodeAvio = avio_alloc_context(recvBuff, 50 * 1024, 1, NULL, NULL, NULL, NULL);
	//
	////注意avio_open 该函数会造成句柄泄露
	////ret = avio_open(&m_encodeAvio, "nothing", AVIO_FLAG_READ_WRITE);

	//m_ofmt_ctx->pb = m_encodeAvio;
	//m_ofmt_ctx->pb->opaque = this;
	//m_ofmt_ctx->pb->write_packet = &Session::wirte_data;

	//ret = avformat_write_header(m_ofmt_ctx, NULL);
	//if (ret < 0) {
	//	printf("Error occurred when opening output URL\n");
	//}
}

int Session::wirte_data(void * opaque, uint8_t *buf, int bufsize){
	int nRet = 0;
	Session *session = (Session *)opaque;
	if (session->m_exitThread == true)
	{
		printf("该会话已经停止. \n");
		return 0;
	}
	(*session->m_callbackFunc)(buf, bufsize);
	return nRet;
}