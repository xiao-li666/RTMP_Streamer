#include "XData.h"
#include <string.h>


XData::XData(char* data, int size, long long pts)
{
	//Éî¿½±´

	this->data = new char[size];
	memcpy(this->data, data, size);
	this->size = size;
	this->pts = pts;
}

XData::~XData()
{
}
void XData::Drop()
{
	if (data)
	{
		delete data;
		data = nullptr;
	}
}

long long GetCurrentTime()
{
	return av_gettime();
}
