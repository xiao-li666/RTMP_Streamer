#include "XController.h"
#include "XVideoCapture.h"
#include "XAudioRecord.h"
#include "XMediaEncode.h"
#include "XRtmp.h"
#include <iostream>

bool XController::StartController()
{
	//添加过滤器
	XVideoCapture::Get()->AddFilter(XFilter::Get(XBILATERAL));

	if (videoIndex >= 0)
	{
		if (!XVideoCapture::Get()->Init(0))
		{
			err = "打开摄像头失败";
			std::cout << err << std::endl;
			return false;
		}

	}
	else if (!inUrl.empty())
	{
		if (!XVideoCapture::Get()->Init(inUrl.c_str()))
		{
			err = "打开摄像头失败";
			std::cout << err << std::endl;
			return false;
		}
	}
	else
	{
		err = "请输入相机参数";
		std::cout << err << std::endl;
		return false;
	}
	std::cout << "相机打开成功" << std::endl;

	if (!XAudioRecord::Get()->Init())
	{
		err = "打开麦克风失败";
		std::cout << err << std::endl;
		return false;
	}
	std::cout << "打开麦克风成功" << std::endl;

	// 初始化音视频编码类
	XMediaEncode::Get()->inWidth = XVideoCapture::Get()->width;
	XMediaEncode::Get()->inHeight = XVideoCapture::Get()->height;
	XMediaEncode::Get()->outWidth = XVideoCapture::Get()->width;
	XMediaEncode::Get()->outHeight = XVideoCapture::Get()->height;
	XMediaEncode::Get()->fps = XVideoCapture::Get()->fps;
	//视频像素格式转换
	if (!XMediaEncode::Get()->InitScale())
	{
		err = "视频像素格式转换失败";
		std::cout << err << std::endl;
		return false;
	}
	std::cout << "视频像素格式转换成功" << std::endl;
	//音频重采样
	XMediaEncode::Get()->channels = XAudioRecord::Get()->channels;
	XMediaEncode::Get()->sampleRate = XAudioRecord::Get()->SampleRate;
	XMediaEncode::Get()->nbSamples = XAudioRecord::Get()->nbSamples;
	XMediaEncode::Get()->inSampleFmt = AVSampleFMT::X_S16;
	XMediaEncode::Get()->outSampleFmt = AVSampleFMT::X_FLTP;
	if (!XMediaEncode::Get()->InitResample())
	{
		err = "音频重采样失败";
		std::cout << err << std::endl;
		return false;
	}
	std::cout << "音频重采样成功" << std::endl;
	if (!XMediaEncode::Get()->InitVideoEncode())
	{
		err = "视频编码器初始化失败";
		std::cout << err << std::endl;
		return false;
	}
	std::cout << "视频编码器初始化成功" << std::endl;
	if (!XMediaEncode::Get()->InitAudioEncode())
	{
		err = "音频编码器初始化失败";
		std::cout << err << std::endl;
		return false;
	}
	std::cout << "音频编码器初始化成功" << std::endl;

	if (!XRtmp::Get()->Init(outUrl.c_str()))
	{
		err = "rtmp初始化失败";
		std::cout << err << std::endl;
		return false;
	}	
	std::cout << "rtmp初始化成功" << std::endl;
	//视频流配置
	if ((videoStreamIndex = XRtmp::Get()->AddStream(XMediaEncode::Get()->ctx)) < 0)
	{
		err = "rtmp视频流配置失败";
		std::cout << err << std::endl;
		return false;
	}
	std::cout << "rtmp视频流配置成功" << std::endl;
	//音频流配置
	if ((audioStreamIndex = XRtmp::Get()->AddStream(XMediaEncode::Get()->audioCodecCtx)) < 0)
	{
		err = "rtmp音频流配置失败";
		std::cout << err << std::endl;
		return false;
	}
	std::cout << "rtmp音频流配置成功" << std::endl;
	//发送头信息
	if (!XRtmp::Get()->SendHead())
	{
		err = "rtmp发送头信息失败";
		std::cout << err << std::endl;
		return false;
	}
	std::cout << "rtmp发送头信息成功" << std::endl;

	//启动线程
	XVideoCapture::Get()->Start();
	XAudioRecord::Get()->Start();
	XAudioRecord::Get()->Clear();
	XVideoCapture::Get()->Clear();
	XDataThread::Start();

	return true;
}


void XController::Stop()
{
	XDataThread::Stop();
	XVideoCapture::Get()->Stop();
	XAudioRecord::Get()->Stop();
	XMediaEncode::Get()->Close();
	XRtmp::Get()->Close();
	videoIndex = -1;
	inUrl = "";
}

bool XController::Set(std::string key, double val)
{
	if (key == "d")
	{
		XFilter::Get(XBILATERAL)->Set(key, val);
	}
	return true;
}

void XController::run()
{
	long long beginTime = GetCurrentTime();
	while (!isExit)
	{
		XData d = XAudioRecord::Get()->Pop();
		XData vd = XVideoCapture::Get()->Pop();
		if (d.size <= 0 && vd.size <= 0)
		{
			QThread::msleep(1);
			continue;
		}
		//处理音频数据
		if (d.size > 0)
		{
			//已经读了一帧音频数据，重采样
			d.pts = d.pts - beginTime;
			XData pcm = XMediaEncode::Get()->StartResample(d);
			d.Drop();//清除空间
			XData pkt = XMediaEncode::Get()->AudioEncode(pcm);
			if (pkt.size > 0)
			{
				if (XRtmp::Get()->SendFrame(pkt, audioStreamIndex))
				{
					cout << "#" << flush;
				}
			}
		}
		//处理视频数据
		if (vd.size > 0)
		{
			vd.pts = vd.pts - beginTime;
			XData yuv = XMediaEncode::Get()->RGBToYUV(vd);
			vd.Drop();
			XData pkt = XMediaEncode::Get()->VideoEncode(yuv);
			if (pkt.size > 0)
			{
				if (XRtmp::Get()->SendFrame(pkt, videoStreamIndex))
				{
					cout << "*" << flush;
				}
			}
		}
	}
}

XController* XController::Get()
{
	static XController c;
    return &c;
}

XController::XController()
{
}

XController::~XController()
{
}
