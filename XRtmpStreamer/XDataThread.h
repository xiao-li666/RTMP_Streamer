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

	//�����߳�
	virtual void Start();
	//�˳��̲߳��ȴ��߳̽���
	virtual void Stop();
	//
	virtual void Clear();

protected:
	std::mutex mux;
	std::list<XData> datas;
	bool isExit = false;
};

