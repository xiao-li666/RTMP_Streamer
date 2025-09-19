#include "CXRtmp.h"
#include <iostream>
extern "C" {
#include <libavformat/avformat.h>
}

CXRtmp::CXRtmp()
{
}

CXRtmp::~CXRtmp()
{
}

bool CXRtmp::Init(const char* url)
{
	int ret = avformat_alloc_output_context2(&ofmt_ctx, nullptr, "flv", url);
	if (ret != 0)
	{
		std::cout << "avformat_alloc_output_context2 failed!" << std::endl;
		return false;

	}
	this->url = url;
	return true;
}

int CXRtmp::AddStream(const AVCodecContext* ctx)
{
	if (!ofmt_ctx)
	{
		std::cout << "ofmt_ctx is null!" << std::endl;
		return -1;
	}

	AVStream *stm = avformat_new_stream(ofmt_ctx, nullptr);
	if (!stm)
	{
		std::cout << "avformat_new_stream failed!" << std::endl;
		return -1;
	}
	stm->codecpar->codec_tag = 0;
	int ret = avcodec_parameters_from_context(stm->codecpar, ctx);
	if (ret != 0)
	{
		std::cout << "avcodec_parameters_from_context failed!" << std::endl;
		return -1;
	}
	if (ctx->codec_type == AVMEDIA_TYPE_VIDEO)
	{
		this->vctx = ctx;
		this->stream = stm;
	}
	else if (ctx->codec_type == AVMEDIA_TYPE_AUDIO)
	{
		this->actx = ctx;
		this->astream = stm;
	}
	return stm->index;
}

bool CXRtmp::SendHead()
{
	//3.打开输出IO
	av_dump_format(ofmt_ctx, 0, url.c_str(), 1);
	int ret = avio_open2(&ofmt_ctx->pb, url.c_str(), AVIO_FLAG_WRITE, nullptr, nullptr);
	if (ret != 0)
	{
		std::cout << "avio_open2 failed!" << std::endl;
		return false;
	}
	//4.写入头信息
	ret = avformat_write_header(ofmt_ctx, nullptr);
	if (ret != 0)
	{
		std::cout << "avformat_write_header failed!" << std::endl;
		return false;
	}
	return true;
}

bool CXRtmp::SendFrame(XData d, int streamIdx)
{
	if(!d.data ||  d.size <= 0)
	{
		return false;
	}
	AVPacket* pkt = (AVPacket*)d.data;
	pkt->stream_index = streamIdx;
	//推流
	if (astream && (pkt->stream_index == astream->index))
	{
		pkt->pts = av_rescale_q(pkt->pts, actx->time_base, astream->time_base);
		pkt->dts = av_rescale_q(pkt->dts, actx->time_base, astream->time_base);
		pkt->duration = av_rescale_q(pkt->duration, actx->time_base, astream->time_base);
	}
	else if (stream && (pkt->stream_index == stream->index))
	{
		pkt->pts = av_rescale_q(pkt->pts, vctx->time_base, stream->time_base);
		pkt->dts = av_rescale_q(pkt->dts, vctx->time_base, stream->time_base);
		pkt->duration = av_rescale_q(pkt->duration, vctx->time_base, stream->time_base);
	}
	else
	{
		return false;
	}
	int ret = av_interleaved_write_frame(ofmt_ctx, pkt); //自动释放pack
	if (ret == 0)
	{
		//std::cout << "#" << std::flush;
	}
	return true;
}

void CXRtmp::Close()
{
	if (ofmt_ctx)
	{
		avformat_free_context(ofmt_ctx);
		ofmt_ctx = nullptr;
		stream = nullptr;
	}
	vctx = nullptr;
	url = "";
}
