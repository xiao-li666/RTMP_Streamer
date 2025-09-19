#include "XFiltBilater.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
using namespace cv;

XFiltBilater::XFiltBilater()
{
	params.insert(std::make_pair("d", 5));//¡Ï”Ú÷±æ∂
}

XFiltBilater::~XFiltBilater()
{
}

bool XFiltBilater::Filter(cv::Mat* src, cv::Mat* dst)
{
	double d = params["d"];
	bilateralFilter(*src, *dst, d ,d * 2, d / 2);
	return true;
}
