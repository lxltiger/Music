//
// Created by Administrator on 2018/11/19 0019.
//

#include "PacketQueue.h"

PacketQueue::PacketQueue(Status *status) {
    this->status = status;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

}

PacketQueue::~PacketQueue() {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

int PacketQueue::put(AVPacket *avPacket) {
    pthread_mutex_lock(&mutex);
    queuePacket.push(avPacket);
//    LOGI("put one packet left %d",queuePacket.size())
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return 0;
}

int PacketQueue::get(AVPacket *avPacket) {
    pthread_mutex_lock(&mutex);
    while (status != NULL && !status->exit) {
        if (queuePacket.size() > 0) {
            AVPacket *packet = queuePacket.front();
            if (av_packet_ref(avPacket, packet) == 0) {
                queuePacket.pop();
            }
            av_packet_free(&packet);
            av_free(packet);
            packet = NULL;
//            LOGI("从队列里面取出一个AVpacket，还剩下 %d 个", queuePacket.size());
            break;
        } else {
            pthread_cond_wait(&cond, &mutex);
        }

    }
    pthread_mutex_unlock(&mutex);

    return 0;
}

int PacketQueue::size() {
    int size = 0;
    pthread_mutex_lock(&mutex);
    size = queuePacket.size();
    pthread_mutex_unlock(&mutex);
    return size;
}
