#pragma once
#include "XFilter.h"
class XFiltBilater :
    public XFilter
{
public:
	XFiltBilater();
	~XFiltBilater();
	//virtual XData Process(XData d);
	virtual bool Filter(cv::Mat* src, cv::Mat* dst);
};

