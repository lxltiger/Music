//
// Created by Administrator on 2018/11/19 0019.
//

#include "Audio.h"

Audio::Audio(Status *status) {
    this->status=status;
    queue = new PacketQueue(status);
    buffer = (uint8_t *)(av_malloc(FRAME_SIZE));
}

Audio::~Audio() {

}

void *decodePlay(void *data){
    Audio *audio = (Audio *) data;
    audio->reSampleAudio();
    pthread_exit(&audio->thread_play);
}

void Audio::play() {
    pthread_create(&thread_play,NULL,decodePlay,this);
}

int Audio::reSampleAudio() {
    
    return 0;
}
