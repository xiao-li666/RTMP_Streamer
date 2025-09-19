#pragma once
#include "XAudioRecord.h"
#include <mutex>
#include <list>
#include <condition_variable>
#define MAX_LIST 100
class QAudioInput;
class QIODevice;
class CXAudioRecord :
    public XAudioRecord
{
public:
    CXAudioRecord();
    ~CXAudioRecord();

    virtual bool Init();
    virtual void Stop();
    virtual void run();


private:
    QAudioInput* audioInput = nullptr;
    QIODevice* audioDevice = nullptr;
};

