#pragma once
extern "C"
{
#include <libavutil/time.h>
}
class XData
{
public:
	XData() {}
	//创建空间，并复制数据
	XData(char *data, int size, long long pts);
	~XData();
	void Drop();

	char* data = nullptr;
	int size = 0;
	long long pts = 0;
};

//获取当前时间戳
long long GetCurrentTime();

