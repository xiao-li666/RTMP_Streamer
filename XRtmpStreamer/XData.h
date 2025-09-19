#pragma once
extern "C"
{
#include <libavutil/time.h>
}
class XData
{
public:
	XData() {}
	//�����ռ䣬����������
	XData(char *data, int size, long long pts);
	~XData();
	void Drop();

	char* data = nullptr;
	int size = 0;
	long long pts = 0;
};

//��ȡ��ǰʱ���
long long GetCurrentTime();

