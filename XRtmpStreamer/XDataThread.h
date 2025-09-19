#pragma once
#include "QThread"
#include "XData.h"
#include <mutex>
#define  MAX_LIST 100
class XDataThread :
    public QThread
{
public:
	XDataThread();
	~XDataThread();

	virtual void Push(XData d);
	virtual XData Pop();

	//启动线程
	virtual void Start();
	//退出线程并等待线程结束
	virtual void Stop();
	//
	virtual void Clear();

protected:
	std::mutex mux;
	std::list<XData> datas;
	bool isExit = false;
};

