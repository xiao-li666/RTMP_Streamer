#pragma once
//����ģʽ�����ɵ�����ֱ�Ӵ�������
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavformat/avformat.h>	
#include <libavutil/avutil.h>
#include <libswresample/swresample.h>
}
#include "XData.h"

enum AVSampleFMT
{
	X_S16 = 1,
	X_FLTP = 8,
};

class XMediaEncode
{
public:
	static XMediaEncode* Get(unsigned char index = 0);//�������ڲ�������

	//��ʼ�����ظ�ʽת����������
	virtual bool InitScale() = 0;
	//��ʼ�ز�����������
	virtual bool InitResample() =0;
	virtual bool InitVideoEncode() = 0;
	virtual bool InitAudioEncode() = 0;
	virtual XData VideoEncode(XData d) = 0;
	virtual XData AudioEncode(XData d) = 0;

	virtual XData RGBToYUV(XData d) = 0;
	virtual XData StartResample(XData d) = 0;
	virtual void Close() = 0;

	virtual ~XMediaEncode();
protected:
	XMediaEncode();

public:
	//�������
	int inWidth = 1280;
	int inHeight = 720;
	int inPixSize = 3;
	int fps = 25;

	int channels = 2;
	int sampleRate = 44100;
	AVSampleFMT inSampleFmt = X_S16;

	//�������
	int outWidth = 1280;
	int outHeight = 720;
	AVSampleFMT outSampleFmt = X_FLTP;
	int nbSamples = 1024;



	AVCodecContext* ctx = nullptr;
	AVCodecContext* audioCodecCtx = nullptr;
//private:
	SwsContext* vsc = nullptr;
	SwrContext* swrCtx = nullptr;
	AVFrame* yuv = nullptr;
	AVFrame* pcm = nullptr;
	AVPacket pack = {0};
	AVPacket apkt = { 0 };
	int vpts = 0;
	int apts = 0;
};

