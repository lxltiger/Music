//
// Created by Administrator on 2018/11/19 0019.
//

#ifndef MUSIC_AUDIO_H
#define MUSIC_AUDIO_H

#include "Status.h"
#include "PacketQueue.h"
#include "JavaInvoke.h"
#include "SoundTouch.h"

using namespace soundtouch;

extern "C" {
#include <libavutil/time.h>
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
    int ret = 0;
    uint8_t *buffer = NULL;
    int data_size = 0;
    int sample_rate = 0;

    int duration = 0;
    AVRational time_base;
    double clock;
    double current_frame_time;
    double last_time;

    int volume = 70;
    int mute = 2;

    /*
     * GLSE中的核心对象，主要功能
     * （1）管理 Audio Engine 的生命周期
     * （2）提供管理接口: SLEngineItf，该接口可以用来创建所有其他的 Object 对象
     * （3）提供设备属性查询接口：SLEngineCapabilitiesItf 和 SLAudioIODeviceCapabilitiesItf，这些接口可以查询设备的一些属性信息
     */
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine = NULL;


    SLObjectItf outputMixObject = NULL;
    SLEnvironmentalReverbItf outputEnviromentReverb;
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

    SLObjectItf pcmPlayObject = NULL;
    SLPlayItf pcmPlayerPlay = NULL;
    SLVolumeItf pcmVolumePlay = NULL;
    SLMuteSoloItf pcmMutePlay = NULL;

    SLAndroidSimpleBufferQueueItf pcmBufferQueue = NULL;

    SoundTouch *soundTouch = NULL;
    SAMPLETYPE *sampleBuffer = NULL;
    bool finished = true;
    uint8_t *out_buffer = NULL;
    int nb = 0;
    int num = 0;
    float pitch = 1.0f;
    float speed = 1.0f;

    bool  isRecording=false;

public:
    Audio(Status *status, int sample_rate, JavaInvoke *javaInvoke);

    ~Audio();

    void play();

    int reSampleAudio(void **pcmbuf);

    void initOpenSLES();

    int getCurrentSampleRate(int rate);

    void pause();

    void resume();

    void stop();

    void release();

    void setVolume(int vol);

    void setMute(int mute);

    int getSoundTouchData();

    void setPitch(float pitch);

    void setSpeed(float speed);

    void record(bool recording);
};


#endif //MUSIC_AUDIO_H
