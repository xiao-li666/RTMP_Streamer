#pragma once
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}
#include "XData.h"
class XRtmp
{
public:
	static XRtmp* Get(unsigned char index = 0);//对向由内部来创建
	virtual bool Init(const char* url) = 0;
	//失败返回-1，成功返回流索引
	virtual int AddStream(const AVCodecContext* codec) = 0;//添加视频或者 音频流
	virtual bool SendHead() = 0;//打开rtmp网络IO，发送封装头
	virtual bool SendFrame(XData d, int streamIdx) = 0;
	virtual void Close() = 0;
	virtual ~XRtmp();

protected:
	XRtmp();


};

