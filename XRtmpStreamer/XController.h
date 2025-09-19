#pragma once
#include <QThread>
#include "XDataThread.h"
#include <string>
class XController :
    public XDataThread
{
public:
    virtual bool StartController();
	virtual void Stop();
	//�������ղ���
	virtual bool Set(std::string key, double val);

	void run();

	//����ģʽ
	static XController* Get();

	std::string outUrl;
	int videoIndex = -1;
	std::string inUrl = "";
	std::string err = "";
	int videoStreamIndex = 0;
	int audioStreamIndex = 0;
	virtual ~XController();

protected:
	XController();

};

