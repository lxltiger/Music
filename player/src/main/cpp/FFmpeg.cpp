//
// Created by Administrator on 2018/11/19 0019.
//

#include "FFmpeg.h"

FFmpeg::FFmpeg(Status *status, JavaInvoke *javaInvoke, const char *url) {
    this->status = status;
    this->javaInvoke = javaInvoke;
    this->url = url;
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

void FFmpeg::startDecodeThread() {
    LOGI("startDecodeThread")

    av_register_all();
    avformat_network_init();
    avFormatContext = avformat_alloc_context();
    if (avformat_open_input(&avFormatContext, url, NULL, NULL) != 0) {
        LOGE("fail to avformat_open_input:%s", url);
        return;
    }
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        LOGE("fail to find stream info %s", url);
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
            }
        }
    }


    AVCodec *pCodec = avcodec_find_decoder(audio->parameters->codec_id);
    if (!pCodec) {
        LOGE("can't find decoder");
        return;
    }
    audio->avCodecContext = avcodec_alloc_context3(pCodec);
    if (!audio->avCodecContext) {
        LOGE("can not alloc new decodecctx");
        return;
    }

    if (avcodec_parameters_to_context(audio->avCodecContext, audio->parameters) < 0) {
        LOGE("can not fill decodecctx");
        return;
    }

    if (avcodec_open2(audio->avCodecContext, pCodec, 0) != 0) {
        LOGE("cant not open audio strames");
        return;
    }

    javaInvoke->onPrepared(childThread);

}

void FFmpeg::start() {
    if (audio == NULL) {
        LOGE("audio is null");
        return;
    }
    audio->play();

    int count = 0;
    while (status != NULL && !status->exit) {
        AVPacket *pPacket = av_packet_alloc();
        if (av_read_frame(avFormatContext, pPacket) == 0) {
            if (pPacket->stream_index == audio->streamIndex) {
                count++;
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
        }
    }


    LOGI("done ")
}

FFmpeg::~FFmpeg() {

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
