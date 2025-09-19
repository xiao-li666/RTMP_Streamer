#pragma once
#include "XMediaEncode.h"
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavformat/avformat.h>	
#include <libavutil/avutil.h>
}
#include "XData.h"

class CXMediaEncode : public XMediaEncode
{
public:
	CXMediaEncode();
	~CXMediaEncode();

	virtual bool InitScale();
	virtual bool InitResample();
	virtual bool InitVideoEncode();
	virtual bool InitAudioEncode();
	virtual XData RGBToYUV(XData d);
	virtual XData StartResample(XData d);
	virtual XData VideoEncode(XData d);
	virtual XData AudioEncode(XData d);
	virtual void Close();


};

