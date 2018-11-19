//
// Created by Administrator on 2018/11/19 0019.
//

#ifndef MUSIC_FFMPEG_H
#define MUSIC_FFMPEG_H
//必须加 否则编译不过
extern "C"{
#include <libavformat/avformat.h>
};
#include "JavaInvoke.h"
#include "Audio.h"
#include "Status.h"
#include <pthread.h>

class FFmpeg {
public:
    JavaInvoke *javaInvoke=NULL;
    Audio *audio=NULL;
    pthread_t  decodeThread;
    AVFormatContext *avFormatContext=NULL;
    const char *url=NULL;
    Status *status;
public:
    FFmpeg(Status *status,JavaInvoke *javaInvoke, const char *url);
    ~FFmpeg();

    void prepare();
    void startDecodeThread();
    void start();
};


#endif //MUSIC_FFMPEG_H
