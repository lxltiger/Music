//
// Created by Administrator on 2018/11/19 0019.
//

#ifndef MUSIC_AUDIO_H
#define MUSIC_AUDIO_H

#include "Status.h"
#include "PacketQueue.h"
#include "JavaInvoke.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
};


class Audio {
public:
    int streamIndex = -1;
    AVCodecContext *avCodecContext = NULL;
    AVCodecParameters *parameters = NULL;
    Status *status;
    PacketQueue *queue;
    JavaInvoke *javaInvoke;

    pthread_t thread_play;
    AVPacket *avPacket;
    AVFrame *avFrame;
    int ret=0;
    uint8_t *buffer=NULL;
    int data_size=0;
    int sample_rate=0;

    SLObjectItf engineObject=NULL;
    SLEngineItf engineEngine=NULL;


    SLObjectItf outputMixObject=NULL;
    SLEnvironmentalReverbItf  outputEnviromentReverb;
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

    SLObjectItf pcmPlayObject=NULL;
    SLPlayItf pcmPlayerPlay=NULL;

    SLAndroidSimpleBufferQueueItf pcmBufferQueue=NULL;


public:
    Audio(Status *status,int sample_rate,JavaInvoke *javaInvoke);
    ~Audio();

    void play();

    int reSampleAudio();

    void initOpenSLES();

    int getCurrentSampleRate(int rate);

    void pause();

    void resume();

};


#endif //MUSIC_AUDIO_H
