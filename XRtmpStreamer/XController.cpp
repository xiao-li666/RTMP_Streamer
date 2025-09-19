#include "XController.h"
#include "XVideoCapture.h"
#include "XAudioRecord.h"
#include "XMediaEncode.h"
#include "XRtmp.h"
#include <iostream>

bool XController::StartController()
{
	//��ӹ�����
	XVideoCapture::Get()->AddFilter(XFilter::Get(XBILATERAL));

	if (videoIndex >= 0)
	{
		if (!XVideoCapture::Get()->Init(0))
		{
			err = "������ͷʧ��";
			std::cout << err << std::endl;
			return false;
		}

	}
	else if (!inUrl.empty())
	{
		if (!XVideoCapture::Get()->Init(inUrl.c_str()))
		{
			err = "������ͷʧ��";
			std::cout << err << std::endl;
			return false;
		}
	}
	else
	{
		err = "�������������";
		std::cout << err << std::endl;
		return false;
	}
	std::cout << "����򿪳ɹ�" << std::endl;

	if (!XAudioRecord::Get()->Init())
	{
		err = "����˷�ʧ��";
		std::cout << err << std::endl;
		return false;
	}
	std::cout << "����˷�ɹ�" << std::endl;

	// ��ʼ������Ƶ������
	XMediaEncode::Get()->inWidth = XVideoCapture::Get()->width;
	XMediaEncode::Get()->inHeight = XVideoCapture::Get()->height;
	XMediaEncode::Get()->outWidth = XVideoCapture::Get()->width;
	XMediaEncode::Get()->outHeight = XVideoCapture::Get()->height;
	XMediaEncode::Get()->fps = XVideoCapture::Get()->fps;
	//��Ƶ���ظ�ʽת��
	if (!XMediaEncode::Get()->InitScale())
	{
		err = "��Ƶ���ظ�ʽת��ʧ��";
		std::cout << err << std::endl;
		return false;
	}
	std::cout << "��Ƶ���ظ�ʽת���ɹ�" << std::endl;
	//��Ƶ�ز���
	XMediaEncode::Get()->channels = XAudioRecord::Get()->channels;
	XMediaEncode::Get()->sampleRate = XAudioRecord::Get()->SampleRate;
	XMediaEncode::Get()->nbSamples = XAudioRecord::Get()->nbSamples;
	XMediaEncode::Get()->inSampleFmt = AVSampleFMT::X_S16;
	XMediaEncode::Get()->outSampleFmt = AVSampleFMT::X_FLTP;
	if (!XMediaEncode::Get()->InitResample())
	{
		err = "��Ƶ�ز���ʧ��";
		std::cout << err << std::endl;
		return false;
	}
	std::cout << "��Ƶ�ز����ɹ�" << std::endl;
	if (!XMediaEncode::Get()->InitVideoEncode())
	{
		err = "��Ƶ��������ʼ��ʧ��";
		std::cout << err << std::endl;
		return false;
	}
	std::cout << "��Ƶ��������ʼ���ɹ�" << std::endl;
	if (!XMediaEncode::Get()->InitAudioEncode())
	{
		err = "��Ƶ��������ʼ��ʧ��";
		std::cout << err << std::endl;
		return false;
	}
	std::cout << "��Ƶ��������ʼ���ɹ�" << std::endl;

	if (!XRtmp::Get()->Init(outUrl.c_str()))
	{
		err = "rtmp��ʼ��ʧ��";
		std::cout << err << std::endl;
		return false;
	}	
	std::cout << "rtmp��ʼ���ɹ�" << std::endl;
	//��Ƶ������
	if ((videoStreamIndex = XRtmp::Get()->AddStream(XMediaEncode::Get()->ctx)) < 0)
	{
		err = "rtmp��Ƶ������ʧ��";
		std::cout << err << std::endl;
		return false;
	}
	std::cout << "rtmp��Ƶ�����óɹ�" << std::endl;
	//��Ƶ������
	if ((audioStreamIndex = XRtmp::Get()->AddStream(XMediaEncode::Get()->audioCodecCtx)) < 0)
	{
		err = "rtmp��Ƶ������ʧ��";
		std::cout << err << std::endl;
		return false;
	}
	std::cout << "rtmp��Ƶ�����óɹ�" << std::endl;
	//����ͷ��Ϣ
	if (!XRtmp::Get()->SendHead())
	{
		err = "rtmp����ͷ��Ϣʧ��";
		std::cout << err << std::endl;
		return false;
	}
	std::cout << "rtmp����ͷ��Ϣ�ɹ�" << std::endl;

	//�����߳�
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
		//������Ƶ����
		if (d.size > 0)
		{
			//�Ѿ�����һ֡��Ƶ���ݣ��ز���
			d.pts = d.pts - beginTime;
			XData pcm = XMediaEncode::Get()->StartResample(d);
			d.Drop();//����ռ�
			XData pkt = XMediaEncode::Get()->AudioEncode(pcm);
			if (pkt.size > 0)
			{
				if (XRtmp::Get()->SendFrame(pkt, audioStreamIndex))
				{
					cout << "#" << flush;
				}
			}
		}
		//������Ƶ����
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
