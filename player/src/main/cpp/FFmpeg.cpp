//
// Created by Administrator on 2018/11/19 0019.
//

#include "FFmpeg.h"

FFmpeg::FFmpeg(Status *status, JavaInvoke *javaInvoke, const char *url) {
    this->status = status;
    this->javaInvoke = javaInvoke;
    this->url = url;
    exit=false;
    pthread_mutex_init(&mutex_init, NULL);
    pthread_mutex_init(&mutex_seek, NULL);
}


void *startDecode(void *data) {
    FFmpeg *fFmpeg = (FFmpeg *) data;
    fFmpeg->startDecodeThread();
    pthread_exit(&fFmpeg->decodeThread);
}

void FFmpeg::prepare() {
    LOGI("prepare in FFmpeg")
    pthread_create(&decodeThread, NULL, startDecode, this);
}

int avFormat_callback(void *context){
    FFmpeg *fFmpeg=(FFmpeg *)context;
    if (fFmpeg->status->exit) {
        return AVERROR_EOF;
    }
    return 0;
}
void FFmpeg::startDecodeThread() {
    LOGI("startDecodeThread")
    //防止网络资源加载后资源被释放
    pthread_mutex_lock(&mutex_init);

    av_register_all();
    avformat_network_init();
    avFormatContext = avformat_alloc_context();

    avFormatContext->interrupt_callback.callback=avFormat_callback;
    avFormatContext->interrupt_callback.opaque=this;

    if (avformat_open_input(&avFormatContext, url, NULL, NULL) != 0) {
        LOGE("fail to avformat_open_input:%s", url);
        exit = true;
        javaInvoke->onError(childThread, 1000, "fail to avformat_open_input");
        pthread_mutex_unlock(&mutex_init);
        return;
    }
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        LOGE("fail to find stream info %s", url);
        exit = true;
        javaInvoke->onError(childThread, 1001, "fail to find stream info");
        pthread_mutex_unlock(&mutex_init);
        return;
    }
    //找到音频流和参数
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        if (avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (audio == NULL) {
                audio = new Audio(status,avFormatContext->streams[i]->codecpar->sample_rate,javaInvoke);
                audio->streamIndex = i;
                audio->parameters = avFormatContext->streams[i]->codecpar;
                audio->duration = avFormatContext->duration / AV_TIME_BASE;
                audio->time_base=avFormatContext->streams[i]->time_base;
                duration = audio->duration;
            }
        }
    }


    AVCodec *pCodec = avcodec_find_decoder(audio->parameters->codec_id);
    if (!pCodec) {
        LOGE("can't find decoder");
        exit = true;
        javaInvoke->onError(childThread, 1002, "can't find decoder");
        pthread_mutex_unlock(&mutex_init);
        return;
    }
    audio->avCodecContext = avcodec_alloc_context3(pCodec);
    if (!audio->avCodecContext) {
        LOGE("can not alloc new decodecctx");
        exit = true;
        javaInvoke->onError(childThread, 1003, "can not alloc new decodecctx");

        pthread_mutex_unlock(&mutex_init);
        return;
    }

    if (avcodec_parameters_to_context(audio->avCodecContext, audio->parameters) < 0) {
        LOGE("can not fill decodecctx");
        exit = true;
        javaInvoke->onError(childThread, 1004, "can not fill decodecctx");

        pthread_mutex_unlock(&mutex_init);
        return;
    }

    if (avcodec_open2(audio->avCodecContext, pCodec, 0) != 0) {
        LOGE("cant not open audio strames");
        exit = true;
        javaInvoke->onError(childThread, 1005, "cant not open audio strames");

        pthread_mutex_unlock(&mutex_init);
        return;
    }

    if (javaInvoke != NULL) {
        if (status != NULL && !status->exit) {
            javaInvoke->onPrepared(childThread);
        }else{
            exit=true;
        }
    }
    pthread_mutex_unlock(&mutex_init);
}

void FFmpeg::start() {
    if (audio == NULL) {
        LOGE("audio is null");
        return;
    }
    audio->play();

//    int count = 0;
    while (status != NULL && !status->exit) {
        if (status->seek) {
            continue;
        }
        if (audio->queue->size() > 40) {
            continue;
        }
        AVPacket *pPacket = av_packet_alloc();
        pthread_mutex_lock(&mutex_seek);
        int ret = av_read_frame(avFormatContext, pPacket);
        pthread_mutex_unlock(&mutex_seek);

        if (ret== 0) {
            if (pPacket->stream_index == audio->streamIndex) {
//                count++;
                audio->queue->put(pPacket);
            } else {
                av_packet_free(&pPacket);
                av_free(pPacket);
            }
        } else {
            LOGI("decode finished");
            av_packet_free(&pPacket);
            av_free(pPacket);
            while (status != NULL && !status->exit) {
                if (audio->queue->size() > 0) {
                    continue;
                } else {
                    status->exit = true;
                    break;
                }
            }
            break;
        }
    }

    if (javaInvoke != NULL) {
        javaInvoke->onComplete(childThread);
    }

    exit=true;
    LOGI("done ")
}

FFmpeg::~FFmpeg() {
    pthread_mutex_destroy(&mutex_init);
    pthread_mutex_destroy(&mutex_seek);
}

void FFmpeg::pause() {
    if (audio != NULL) {
        audio->pause();
    }
}

void FFmpeg::resume() {
    if (audio != NULL) {
        audio->resume();
    }
}

void FFmpeg::release() {
    /*if (status->exit) {
        return;
    }*/
    LOGE("开始释放Ffmpe");
    status->exit=true;

    pthread_mutex_lock(&mutex_init);
    int sleepCount=0;

    while (!exit) {
        if (sleepCount < 1000) {
            sleepCount++;
            LOGE("wait ffmpeg  exit %d", sleepCount);
        }else{
            exit=true;
        }

        av_usleep(100 * 1000);
    }

    if (audio != NULL) {
        audio->release();
        delete (audio);
        audio=NULL;
    }

    if (avFormatContext != NULL) {
        avformat_close_input(&avFormatContext);
        avformat_free_context(avFormatContext);
        avFormatContext=NULL;
    }

    if (javaInvoke != NULL) {
        javaInvoke = NULL;
    }

    if (status != NULL) {
        status = NULL;
    }


    pthread_mutex_unlock(&mutex_init);



}

void FFmpeg::seek(int64_t second) {
    if (duration < 0) {
        return;
    }
    if (second > 0 && second < duration) {
        if (audio != NULL) {
            status->seek=true;
            audio->queue->clear();
            audio->clock=0;
            audio->last_time=0;
            pthread_mutex_lock(&mutex_seek);
            int64_t rel = second * AV_TIME_BASE;
            avformat_seek_file(avFormatContext, -1, INT64_MIN, rel, INT64_MAX, 0);
            pthread_mutex_unlock(&mutex_seek);
            status->seek = false;
        }
    }
}

void FFmpeg::setVolume(int volume) {
    if (audio != NULL) {
        audio->setVolume(volume);
    }
}

void FFmpeg::setMute(int mute) {
    if (audio != NULL) {
        audio->setMute(mute);
    }
}

void FFmpeg::setPitch(float pitch) {
    if (audio != NULL) {
        audio->setPitch(pitch);
    }
}

void FFmpeg::setSpeed(float speed) {
    if (audio != NULL) {
        audio->setSpeed(speed);
    }
}


