#include "CXMediaEncode.h"
#include <iostream>
using namespace std;

CXMediaEncode::CXMediaEncode()
{
}

CXMediaEncode::~CXMediaEncode()
{
}

bool CXMediaEncode::InitScale()
{
	//输出数据结构
	yuv = av_frame_alloc();//分配对象空间
	yuv->format = AV_PIX_FMT_YUV420P;
	yuv->width = inWidth;
	yuv->height = inHeight;
	yuv->pts = 0;
	int ret = av_frame_get_buffer(yuv, 32); //分配yuv空间
	if (ret != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(ret, buf, sizeof(buf) - 1);
		return false;
	}
	vsc = sws_getCachedContext(vsc, inWidth, inHeight, AV_PIX_FMT_BGR24, //输入
		inWidth, inHeight, AV_PIX_FMT_YUV420P, //输出
		SWS_BICUBIC, nullptr, nullptr, nullptr);
	if (!vsc)
	{
		std::cout << "sws_getCachedContext failed" << std::endl;
		return false;
	}
	return true;
}

bool CXMediaEncode::InitResample()
{
	//初始化上下文
	swrCtx = swr_alloc_set_opts(swrCtx, av_get_default_channel_layout(channels), (AVSampleFormat)outSampleFmt, sampleRate,
		av_get_default_channel_layout(channels), (AVSampleFormat)inSampleFmt, sampleRate, 0, nullptr);
	if (!swrCtx)
	{
		cout << "swr_alloc_set_opts failed" << endl;
		return false;
	}
	int ret = swr_init(swrCtx);
	if (ret < 0)
	{
		cout << "swr_init failed" << endl;
		return false;
	}
	//cout << "swr_init success" << endl;

	//
	pcm = av_frame_alloc();
	pcm->format = (AVSampleFormat)outSampleFmt;
	pcm->channels = channels;
	pcm->channel_layout = av_get_default_channel_layout(channels);
	pcm->sample_rate = sampleRate;
	pcm->nb_samples = nbSamples;// 一帧每通道1024个采样点
	ret = av_frame_get_buffer(pcm, 0);
	if (ret < 0)
	{
		cout << "av_frame_get_buffer failed" << endl;
		return false;
	}
	return true;
}

bool CXMediaEncode::InitVideoEncode()
{
	AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!codec)
	{
		std::cout << "avcodec_find_encoder h264 failed!" << std::endl;
		return false;
	}
	//2.创建编码器上下文
	ctx = avcodec_alloc_context3(codec);
	if (!ctx)
	{
		std::cout << "avcodec_alloc_context3 failed!" << std::endl;
		return false;
	}
	//3.设置编码器参数
	ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER; //全局参数
	ctx->codec_id = codec->id;
	ctx->thread_count = 16;
	ctx->codec_type = AVMEDIA_TYPE_VIDEO;
	ctx->bit_rate = 200 * 1024 * 8; //码率,越大越清晰,压缩后每秒视频的bit位大小（200kB）
	ctx->width = outWidth;
	ctx->height = outHeight;
	//ctx->time_base = AVRational{ 1,fps };//帧率的倒数
	ctx->framerate = AVRational{ fps,1 };//帧率
	ctx->gop_size = 10; //关键帧间隔,越大压缩率越高
	ctx->max_b_frames = 0; //最大B帧数，为0则Pts和dts显示时间一致
	ctx->pix_fmt = AV_PIX_FMT_YUV420P;
	ctx->time_base = { 1,1000000 };
	//4.打开编码器
	int ret = avcodec_open2(ctx, codec, nullptr);
	if (ret != 0)
	{
		std::cout << "avcodec_open2 failed!" << std::endl;
		return false;
	}
	std::cout << "avcodec open success" << std::endl;
	//av_init_packet(&pack);
	return true;
}

bool CXMediaEncode::InitAudioEncode()
{
	//初始化音频编码器
	AVCodec* audioCodec = avcodec_find_encoder(AV_CODEC_ID_AAC);
	if (!audioCodec)
	{
		cout << "avcodec_find_encoder failed" << endl;
		return false;
	}
	audioCodecCtx = avcodec_alloc_context3(audioCodec);
	if (!audioCodecCtx)
	{
		cout << "avcodec_alloc_context3 failed" << endl;
		return false;
	}
	audioCodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	audioCodecCtx->thread_count = 16;
	audioCodecCtx->bit_rate = 64000;
	audioCodecCtx->sample_rate = sampleRate;
	audioCodecCtx->sample_fmt = (AVSampleFormat)outSampleFmt;
	audioCodecCtx->channel_layout = av_get_default_channel_layout(channels);
	audioCodecCtx->channels = channels;
	audioCodecCtx->time_base = { 1,1000000 };

	int ret = avcodec_open2(audioCodecCtx, audioCodec, nullptr);
	if (ret < 0)
	{
		cout << "avcodec_open2 failed" << endl;
		return false;
	}
	return true;
}

XData CXMediaEncode::RGBToYUV(XData d)
{
	//输入数据结构
	const uint8_t* inData[AV_NUM_DATA_POINTERS] = { 0 };
	inData[0] = (uint8_t*)d.data;
	int inLineSize[AV_NUM_DATA_POINTERS] = { 0 };
	//一行数据的字节数bgr24
	inLineSize[0] = inWidth * inPixSize;

	int h = sws_scale(vsc, inData, inLineSize, 0, inHeight, //输入
		yuv->data, yuv->linesize); //输出
	if (h <= 0)
	{
		return XData();
	}
	int* p = yuv->linesize;
	int size = 0;
	while ((*p))
	{
		size += (*p) * inHeight;
		p++;
	}
	XData r;
	yuv->pts = d.pts;
	r.data = (char*)yuv;
	r.size = size;
	r.pts = d.pts;
	return r;
}

XData CXMediaEncode::StartResample(XData d)
{
	const uint8_t* inData[AV_NUM_DATA_POINTERS] = { 0 };//输入数据
	inData[0] = (uint8_t*)d.data;
	//重采样
	int len = swr_convert(swrCtx, pcm->data, pcm->nb_samples, inData, pcm->nb_samples);
	if (len <= 0)
	{
		cout << "swr_convert failed" << endl;
		return XData();
	}
	pcm->pts = d.pts;//将每帧的pts修改为采集时的时间戳
	XData r;
	r.data = (char*)pcm;
	r.size = pcm->nb_samples * pcm->channels * 2;
	r.pts = d.pts;
	return r;
}

XData CXMediaEncode::VideoEncode(XData d)
{
	if (d.size <= 0 || !d.data)
	{
		return XData();
	}
	av_packet_unref(&apkt);

	AVFrame* p = (AVFrame*)d.data;
	//yuv->pts = vpts++;
	int ret = avcodec_send_frame(ctx, p);
	if (ret != 0)
	{
		return XData();
	}
	ret = avcodec_receive_packet(ctx, &pack);
	if (ret != 0 || pack.size <= 0)
	{
		return XData();
	}
	XData r; //这里是将AVFrame类作为数据，而size为字节数，如果使用拷贝构造就会出现异常，直接将指向AVFrame的指针传给r.data避免memcpy操作，
	r.data = (char*)&pack;
	r.size = pack.size;
	r.pts = d.pts;
	return r;
}

long long lasta = -1;
XData CXMediaEncode::AudioEncode(XData d)
{
	if (d.size <= 0 || !d.data)
	{
		return XData();
	}

	AVFrame* p = (AVFrame*)d.data;
	if (p->pts == lasta)
	{
		p->pts += 1200;
	}
	lasta = p->pts;
	//送编码器编码
	int ret = avcodec_send_frame(audioCodecCtx, p);
	if (ret < 0)
	{
		cout << "avcodec_send_frame failed" << endl;
		return XData();
	}
	av_packet_unref(&apkt);
	ret = avcodec_receive_packet(audioCodecCtx, &apkt);
	if (ret < 0)
	{
		cout << "avcodec_receive_packet failed" << endl;
		return XData();
	}
	XData r;
	r.data = (char*)&apkt;
	r.size = apkt.size;
	r.pts = d.pts;
	//cout << apkt.size << " " << flush;
	return r;
}

void CXMediaEncode::Close()
{
	if (vsc)
	{
		sws_freeContext(vsc);
		vsc = nullptr;
	}
	if (yuv)
	{
		av_frame_free(&yuv);
	}
	if (ctx)
	{
		avcodec_free_context(&ctx);
	}
	vpts = 0;
	av_packet_unref(&pack);
	if (swrCtx)
	{
		swr_free(&swrCtx);
	}
	if (pcm)
	{
		av_frame_free(&pcm);
	}
	if (audioCodecCtx)
	{
		avcodec_free_context(&audioCodecCtx);
	}
	apts = 0;
	av_packet_unref(&apkt);
}
