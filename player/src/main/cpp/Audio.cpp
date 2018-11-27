//
// Created by Administrator on 2018/11/19 0019.
//

#include "Audio.h"

Audio::Audio(Status *status, int sample_rate, JavaInvoke *javaInvoke) {
    this->status = status;
    this->sample_rate = sample_rate;
    this->javaInvoke = javaInvoke;
    queue = new PacketQueue(status);
    buffer = (uint8_t *) (av_malloc(sample_rate * 2 * 2));
}

Audio::~Audio() {

}

void *decodePlay(void *data) {
    Audio *audio = (Audio *) data;
    audio->initOpenSLES();
    pthread_exit(&audio->thread_play);
}

void Audio::play() {
    pthread_create(&thread_play, NULL, decodePlay, this);
}

//FILE *outFile = fopen("/storage/emulated/0/Music/Akon.pcm", "w");

int Audio::reSampleAudio() {
    while (status != NULL && !status->exit) {

        if (queue->size() == 0) {
            if (!status->load) {
                status->load = true;
                javaInvoke->onLoad(childThread, true);
            }
            continue;
        } else {
            if (status->load) {
                status->load = false;
                javaInvoke->onLoad(childThread, false);
            }
        }


        avPacket = av_packet_alloc();
        if (queue->get(avPacket) != 0) {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }
        ret = avcodec_send_packet(avCodecContext, avPacket);
        if (ret != 0) {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }

        avFrame = av_frame_alloc();
        ret = avcodec_receive_frame(avCodecContext, avFrame);
        if (ret == 0) {
            if (avFrame->channels > 0 && avFrame->channel_layout == 0) {
                avFrame->channel_layout = av_get_default_channel_layout(avFrame->channels);
            } else if (avFrame->channels == 0 && avFrame->channel_layout > 0) {
                avFrame->channel_layout = av_get_channel_layout_nb_channels(
                        avFrame->channel_layout);
            }

            SwrContext *swrContext;
            swrContext = swr_alloc_set_opts(
                    NULL,
                    AV_CH_LAYOUT_STEREO,
                    AV_SAMPLE_FMT_S16,
                    avFrame->sample_rate,
                    avFrame->channel_layout,
                    (AVSampleFormat) avFrame->format,
                    avFrame->sample_rate,
                    NULL, NULL
            );
            if (!swrContext || swr_init(swrContext) < 0) {
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;
                av_frame_free(&avFrame);
                av_free(avFrame);
                avFrame = NULL;
                swr_free(&swrContext);
                continue;
            }

            int nb = swr_convert(swrContext,
                                 &buffer,
                                 avFrame->nb_samples,
                                 (const uint8_t **) (avFrame->data),
                                 avFrame->nb_samples);
            int out_channel = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
            data_size = nb * out_channel * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
//            fwrite(buffer, 1, data_size, outFile);
            current_frame_time = avFrame->pts * av_q2d(time_base);
            if (current_frame_time < clock) {
                current_frame_time = clock;
            }
            clock = current_frame_time;

            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;
            swr_free(&swrContext);
            break;
        } else {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;
            continue;
        }


    }
//    fclose(outFile);
    return data_size;
}


void pcmBufferCallBack(SLAndroidSimpleBufferQueueItf bf, void *context) {
    Audio *audio = (Audio *) context;
    if (audio != NULL) {
        int size = audio->reSampleAudio();
        if (size > 0) {
            audio->clock += size / ((double) (audio->sample_rate) * 2 * 2);
            if (audio->clock - audio->last_time > 0.5) {
                audio->last_time = audio->clock;
                audio->javaInvoke->onPlaying(childThread, (int) (audio->clock), audio->duration);
            }
            (*audio->pcmBufferQueue)->Enqueue(audio->pcmBufferQueue,/* (char *)*/
                                              audio->buffer, size);
        }
    }


}


void Audio::initOpenSLES() {
    SLresult result;

    slCreateEngine(&engineObject, 0, 0, 0, 0, 0);
    (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);

    const SLInterfaceID mixs[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, mixs, mreq);
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                              &outputEnviromentReverb);
    if (SL_RESULT_SUCCESS == result) {
        result = (*outputEnviromentReverb)->SetEnvironmentalReverbProperties(outputEnviromentReverb,
                                                                             &reverbSettings);
    }

    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataLocator_AndroidSimpleBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                            2};
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,
            2,
            (SLuint32) getCurrentSampleRate(sample_rate),
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN
    };
    SLDataSource slDataSource = {&android_queue, &pcm};
    SLDataSink dataSink = {&outputMix, NULL};
    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_MUTESOLO};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &pcmPlayObject, &slDataSource,
                                                &dataSink, 3, ids, req);
    (*pcmPlayObject)->Realize(pcmPlayObject, SL_BOOLEAN_FALSE);
    (*pcmPlayObject)->GetInterface(pcmPlayObject, SL_IID_PLAY, &pcmPlayerPlay);

    (*pcmPlayObject)->GetInterface(pcmPlayObject, SL_IID_BUFFERQUEUE, &pcmBufferQueue);

    (*pcmPlayObject)->GetInterface(pcmPlayObject, SL_IID_VOLUME, &pcmVolumePlay);

    (*pcmPlayObject)->GetInterface(pcmPlayObject, SL_IID_MUTESOLO, &pcmMutePlay);

    setVolume(volume);

    (*pcmBufferQueue)->RegisterCallback(pcmBufferQueue, pcmBufferCallBack, this);

    (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);


    pcmBufferCallBack(pcmBufferQueue, this);
}

int Audio::getCurrentSampleRate(int sample_rate) {
    int rate = 0;
    switch (sample_rate) {
        case 8000:
            rate = SL_SAMPLINGRATE_8;
            break;
        case 11025:
            rate = SL_SAMPLINGRATE_11_025;
            break;
        case 12000:
            rate = SL_SAMPLINGRATE_12;
            break;
        case 16000:
            rate = SL_SAMPLINGRATE_16;
            break;
        case 22050:
            rate = SL_SAMPLINGRATE_22_05;
            break;
        case 24000:
            rate = SL_SAMPLINGRATE_24;
            break;
        case 32000:
            rate = SL_SAMPLINGRATE_32;
            break;
        case 44100:
            rate = SL_SAMPLINGRATE_44_1;
            break;
        case 48000:
            rate = SL_SAMPLINGRATE_48;
            break;
        case 64000:
            rate = SL_SAMPLINGRATE_64;
            break;
        case 88200:
            rate = SL_SAMPLINGRATE_88_2;
            break;
        case 96000:
            rate = SL_SAMPLINGRATE_96;
            break;
        case 192000:
            rate = SL_SAMPLINGRATE_192;
            break;
        default:
            rate = SL_SAMPLINGRATE_44_1;
    }
    return rate;
}

void Audio::pause() {

    if (pcmPlayerPlay != NULL) {
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PAUSED);
    }
}

void Audio::resume() {
    if (pcmPlayerPlay != NULL) {
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);
    }
}

void Audio::stop() {
    if (pcmPlayerPlay != NULL) {
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_STOPPED);
    }
}

void Audio::release() {
    if (queue != NULL) {
        delete (queue);
        queue = NULL;
    }
    if (pcmPlayObject != NULL) {
        (*pcmPlayObject)->Destroy(pcmPlayObject);
        pcmPlayObject = NULL;
        pcmPlayerPlay = NULL;
        pcmBufferQueue = NULL;
    }

    if (outputMixObject != NULL) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = NULL;
        outputEnviromentReverb = NULL;
    }

    if (engineObject != NULL) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }

    if (buffer != NULL) {
        free(buffer);
        buffer = NULL;
    }

    if (avCodecContext != NULL) {
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
        avCodecContext = NULL;
    }

    if (status != NULL) {
        status = NULL;
    }
    if (javaInvoke != NULL) {
        javaInvoke = NULL;
    }


}

void Audio::setVolume(int percent) {
    volume = percent;
    if (pcmVolumePlay != NULL) {
        if (percent > 30) {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -20);
        } else if (percent > 25) {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -22);
        } else if (percent > 20) {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -25);
        } else if (percent > 15) {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -28);
        } else if (percent > 10) {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -30);
        } else if (percent > 5) {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -34);
        } else if (percent > 3) {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -37);
        } else if (percent > 0) {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -40);
        } else {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -100);
        }
    }
}

void Audio::setMute(int mute) {
    this->mute = mute;
    switch (mute) {
        //right
        case 0:
            (*pcmMutePlay)->SetChannelMute(pcmMutePlay, 1, false);
            (*pcmMutePlay)->SetChannelMute(pcmMutePlay, 0, true);
            break;
//left
        case 1:
            (*pcmMutePlay)->SetChannelMute(pcmMutePlay, 1, true);
            (*pcmMutePlay)->SetChannelMute(pcmMutePlay, 0, false);
            break;
        case 2:
        default:
            (*pcmMutePlay)->SetChannelMute(pcmMutePlay, 1, false);
            (*pcmMutePlay)->SetChannelMute(pcmMutePlay, 0, false);

            break;
    }
}
