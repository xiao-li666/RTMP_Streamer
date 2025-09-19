#include "XAudioRecord.h"
#include "CXAudioRecord.h"

XAudioRecord::XAudioRecord()
{
}

XAudioRecord::~XAudioRecord()
{
}

XAudioRecord* XAudioRecord::Get(XAUDIOTYPE type, unsigned char index)
{
	static CXAudioRecord record[255];
	return &record[index];
}
