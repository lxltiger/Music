//
// Created by Administrator on 2018/11/19 0019.
//

#ifndef MUSIC_PACKETQUEUE_H
#define MUSIC_PACKETQUEUE_H

extern "C"{
#include <libavcodec/avcodec.h>
};
#include "queue"
#include "Status.h"
#include "pthread.h"
#include "AndroidLog.h"
class PacketQueue {

public:
    std::queue<AVPacket *> queuePacket;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    Status *status;

    PacketQueue(Status *status);
    ~PacketQueue();

    int put(AVPacket *avPacket);

    int get(AVPacket *avPacket);

    int size();


    void clear();
};


#endif //MUSIC_PACKETQUEUE_H
