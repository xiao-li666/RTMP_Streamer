#include "CXAudioRecord.h"
#include <QAudioInput>
#include <iostream>
#include <QThread>

using namespace std;

CXAudioRecord::CXAudioRecord()
{
}

CXAudioRecord::~CXAudioRecord()
{
}

bool CXAudioRecord::Init()
{
	//1.音频录制
	Stop();
	QAudioFormat format;
	format.setSampleRate(SampleRate);
	format.setChannelCount(channels);
	format.setSampleSize(SampleByte * 8);
	format.setCodec("audio/pcm");
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setSampleType(QAudioFormat::UnSignedInt);
	QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
	if (!info.isFormatSupported(format))
	{
		cout << "Default format not supported - trying to use nearest" << endl;
		format = info.nearestFormat(format);
	}

	audioInput = new QAudioInput(format);
	//已经开始录音
	audioDevice = audioInput->start();
	if (!audioDevice)
	{
		cout << "Failed to start audio input" << endl;
		return false;
	}
	else
	{
		cout << "Audio input started" << endl;
	}
	return true;
}

void CXAudioRecord::Stop()
{
	XDataThread::Stop();
	if (audioInput)
	{
		audioInput->stop();
	}
	if (audioDevice)
	{
		audioDevice->close();
	}
	audioInput = nullptr;
	audioDevice = nullptr;
}

void CXAudioRecord::run()
{
	//一次读取一帧音频数据的字节数
	int readSize = nbSamples * channels * SampleByte;
	char* buf = new char[readSize];
	while (!isExit)
	{
		//读取音频
		if (audioInput->bytesReady() < readSize)
		{
			QThread::msleep(1);
			continue;
		}
		int size = 0;
		while (size != readSize)
		{
			if (!audioDevice || !audioDevice->isOpen()) {
				break;
			}
			int len = audioDevice->read(buf + size, static_cast<qint64>(readSize) - size);
			if (len < 0)
			{
				cout << "audioDevice read failed" << endl;
				break;
			}
			size += len;
		}
		if (size != readSize)
		{
			cout << "audioDevice read size != readSize" << endl;
			continue;
		}
		//以读取一帧
		Push(XData(buf, readSize, GetCurrentTime()));
	}
	delete[] buf;
}
