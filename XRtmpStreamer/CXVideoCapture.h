#pragma once
#include "XVideoCapture.h"
#include <mutex>
#include <list>
class CXVideoCapture :
    public XVideoCapture
{
public:
	CXVideoCapture();
	~CXVideoCapture();

	virtual bool Init(int camIdx = 0);
	virtual bool Init(const char* url);
	virtual void Stop();
	void run();

	virtual bool AddFilter(XFilter* filter);

private:
	VideoCapture cam;
	std::mutex mux;
};

