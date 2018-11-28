//
// Created by Administrator on 2018/11/19 0019.
//

#ifndef MUSIC_FFMPEG_H
#define MUSIC_FFMPEG_H
//必须加 否则编译不过
extern "C"{
#include <libavformat/avformat.h>
#include <libavutil/time.h>

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

    pthread_mutex_t mutex_init;
    bool exit=false;

    int duration=0;
    pthread_mutex_t mutex_seek;
public:
    FFmpeg(Status *status,JavaInvoke *javaInvoke, const char *url);
    ~FFmpeg();

    void prepare();
    void startDecodeThread();
    void start();
    void pause();

    void resume();

    void release();

    void seek(int64_t second);

    void setVolume(int volume);

    void setMute(int mute);

    void setPitch(float pitch);

    void setSpeed(float speed);

    void record(bool recording);


};


#endif //MUSIC_FFMPEG_H
