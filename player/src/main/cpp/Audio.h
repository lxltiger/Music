//
// Created by Administrator on 2018/11/19 0019.
//

#ifndef MUSIC_AUDIO_H
#define MUSIC_AUDIO_H

extern "C" {
    #include <libavcodec/avcodec.h>
};

class Audio {
public:
    int streamIndex=-1;
    AVCodecContext *avCodecContext = NULL;
    AVCodecParameters *parameters=NULL;

public:
    Audio();
    ~Audio();
};


#endif //MUSIC_AUDIO_H
