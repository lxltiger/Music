//
// Created by Administrator on 2018/11/19 0019.
//

#include "Audio.h"

Audio::Audio(Status *status) {
    this->status = status;
    queue = new PacketQueue(status);
    buffer = (uint8_t *) (av_malloc(FRAME_SIZE));
}

Audio::~Audio() {

}

void *decodePlay(void *data) {
    Audio *audio = (Audio *) data;
    audio->reSampleAudio();
    pthread_exit(&audio->thread_play);
}

void Audio::play() {
    pthread_create(&thread_play, NULL, decodePlay, this);
}

FILE *outFile = fopen("/storage/emulated/0/Music/Akon.pcm", "w");

int Audio::reSampleAudio() {
    while (status != NULL && !status->exit) {
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
            fwrite(buffer, 1, data_size, outFile);
            LOGE("data_size is %d", data_size);
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;
            swr_free(&swrContext);
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
    fclose(outFile);
    return data_size;
}
