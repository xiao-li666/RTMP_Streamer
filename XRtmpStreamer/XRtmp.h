#pragma once
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}
#include "XData.h"
class XRtmp
{
public:
	static XRtmp* Get(unsigned char index = 0);//�������ڲ�������
	virtual bool Init(const char* url) = 0;
	//ʧ�ܷ���-1���ɹ�����������
	virtual int AddStream(const AVCodecContext* codec) = 0;//�����Ƶ���� ��Ƶ��
	virtual bool SendHead() = 0;//��rtmp����IO�����ͷ�װͷ
	virtual bool SendFrame(XData d, int streamIdx) = 0;
	virtual void Close() = 0;
	virtual ~XRtmp();

protected:
	XRtmp();


};

