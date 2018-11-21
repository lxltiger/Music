//
// Created by Administrator on 2018/11/19 0019.
//

#ifndef MUSIC_AUDIO_H
#define MUSIC_AUDIO_H

#include "Status.h"
#include "PacketQueue.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
};

#define FRAME_SIZE 44100*2*2

class Audio {
public:
    int streamIndex = -1;
    AVCodecContext *avCodecContext = NULL;
    AVCodecParameters *parameters = NULL;
    Status *status;
    PacketQueue *queue;

    pthread_t thread_play;
    AVPacket *avPacket;
    AVFrame *avFrame;
    int ret=0;
    uint8_t *buffer=NULL;
    int data_size=0;

public:
    Audio(Status *status);
    ~Audio();

    void play();

    int reSampleAudio();



};


#endif //MUSIC_AUDIO_H
