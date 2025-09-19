#include "XRtmp.h"
#include "CXRtmp.h"
//extern "C" {
//#include <libavformat/avformat.h>
//#include <libavcodec/avcodec.h>
//}
XRtmp* XRtmp::Get(unsigned char index)
{
	static bool isFirst = true;
	if (isFirst)
	{
		isFirst = false;
		avformat_network_init();
	}
	static CXRtmp cxr[255];
	return &cxr[index];
}

XRtmp::~XRtmp()
{
}

XRtmp::XRtmp()
{
}
