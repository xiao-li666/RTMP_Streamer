#include "XVideoCapture.h"
#include "CXVideoCapture.h"

XVideoCapture::~XVideoCapture()
{
}

XVideoCapture* XVideoCapture::Get(unsigned char index)
{
	static CXVideoCapture cap[255];
	return &cap[index];
}

XVideoCapture::XVideoCapture()
{
}
