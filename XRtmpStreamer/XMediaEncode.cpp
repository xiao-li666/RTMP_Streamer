#include "XMediaEncode.h"
#include "CXMediaEncode.h"

XMediaEncode::XMediaEncode()
{
}

XMediaEncode* XMediaEncode::Get(unsigned char index)
{
	static CXMediaEncode cxm[255];
	return &cxm[index];
}

XMediaEncode::~XMediaEncode()
{
}
