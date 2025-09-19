#include "XFilter.h"
#include "XFiltBilater.h"
#include <iostream>

XFilter::~XFilter()
{
}

XFilter* XFilter::Get(XFilterType t)
{
	static XFiltBilater bilat;
	switch (t)
	{
	case XBILATERAL:
		return &bilat;

	default:
		break;
	}
	return nullptr;
}

bool XFilter::Set(std::string key, double value)
{
	if (params.find(key) == params.end())
	{
		std::cout << "XFilter::Set failed, not found key=" << key << std::endl;
	}
	params[key] = value;
	return true;
}

XFilter::XFilter()
{
}
