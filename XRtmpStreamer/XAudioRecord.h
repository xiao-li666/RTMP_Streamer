#pragma once
#include "XDataThread.h"
enum XAUDIOTYPE
{
	X_AUDIO_QT = 0
};
class XAudioRecord : public XDataThread
{
public:
	XAudioRecord();
	~XAudioRecord();

	virtual bool Init() = 0;
	virtual void Stop() = 0;
	virtual void run() = 0;

	//工厂模式，获取对象
	static XAudioRecord* Get(XAUDIOTYPE type = X_AUDIO_QT,unsigned char index = 0);

public:
	int channels = 2;
	int SampleByte = 2; //样本字节大小
	int SampleRate = 44100;
	int nbSamples = 1024;//样本数

};

