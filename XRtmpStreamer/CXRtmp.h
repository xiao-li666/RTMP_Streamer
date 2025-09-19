#pragma once
#include "XRtmp.h"
#include <string>
class CXRtmp : public XRtmp
{
public:
	CXRtmp();
	~CXRtmp();
	//��ʼ����װ��������KO
	virtual bool Init(const char* url);
	virtual int AddStream(const AVCodecContext* codec);
	virtual bool SendHead();
	virtual bool SendFrame(XData d, int streamIdx);
	virtual void Close();

private:
	AVFormatContext* ofmt_ctx = nullptr;
	const AVCodecContext* vctx = nullptr;
	AVStream* stream = nullptr;
	const AVCodecContext* actx = nullptr;
	AVStream* astream = nullptr;
	std::string url = "";
};

