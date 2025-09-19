#pragma once
#include <string>
#include <map>
namespace cv
{
	class Mat;
}

enum XFilterType
{
	XBILATERAL //Ë«±ßÂË²¨
};
class XFilter
{
public:
	
	virtual ~XFilter();
	static XFilter* Get(XFilterType t = XBILATERAL);
	virtual bool Filter(cv::Mat *src, cv::Mat *dst)=0;
	//²ÎÊıÉèÖÃ
	virtual bool Set(std::string key, double value);
protected:
	XFilter();

	std::map<std::string, double> params;
};

