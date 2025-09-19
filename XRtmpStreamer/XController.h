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
	//设置美颜参数
	virtual bool Set(std::string key, double val);

	void run();

	//单例模式
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

