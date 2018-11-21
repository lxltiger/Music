#include <jni.h>
#include <string>
#include <pthread.h>
#include <zconf.h>
//#include "queue"
//#include "JavaListener.h"
#include "JavaInvoke.h"
#include "FFmpeg.h"

#define FRAME_SIZE 44100*2*2

extern "C"{
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
 #include <libavformat/avformat.h>

}


//#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,"ywl5320",FORMAT,##__VA_ARGS__);

/*

extern "C"
JNIEXPORT void JNICALL
Java_kimascend_com_player_Demo_testFfempg(JNIEnv *env, jobject instance) {

    av_register_all();
    AVCodec *c_temp = av_codec_next(NULL);
    while (c_temp != NULL)
    {
        switch (c_temp->type)
        {
            case AVMEDIA_TYPE_VIDEO:
                LOGI("[Video]:%s", c_temp->name);
                break;
            case AVMEDIA_TYPE_AUDIO:
                LOGI("[Audio]:%s", c_temp->name);
                break;
            default:
                LOGI("[Other]:%s", c_temp->name);
                break;
        }
        c_temp = c_temp->next;
    }

}

std::queue<int> queue;

pthread_t  producer;
pthread_t  customer;
pthread_mutex_t mutex;
pthread_cond_t cond;
void *produceCallBack(void *data){

    for (;;) {
        pthread_mutex_lock(&mutex);
        queue.push(1);
        LOGI("生产一个总%d个",queue.size());
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        sleep(5);
    }
    LOGI("thread exit");

    pthread_exit(&producer);
}


void *consumeCallBack(void *data){

    while (1) {
        pthread_mutex_lock(&mutex);
        if(queue.size()>0){
            queue.pop();
            LOGI("消费一个还剩%d",queue.size());
        }else{
            LOGI("waiting ")
            pthread_cond_wait(&cond, &mutex);
        }

        pthread_mutex_unlock(&mutex);
        usleep(1000 * 500);
    }
    LOGI("thread exit");

    pthread_exit(&customer);
}
extern "C"
JNIEXPORT void JNICALL
Java_kimascend_com_player_Demo_createThread(JNIEnv *env, jobject instance) {

    for (int i = 0; i < 10; i++) {
        queue.push(i);
    }

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_create(&producer,NULL,produceCallBack,NULL);
    pthread_create(&customer,NULL,consumeCallBack,NULL);


}

JavaListener *javaListener;
pthread_t  child;

void *callJaveInThread(void *data){
    JavaListener *listener= (JavaListener*)data;
    listener->onError(0, 101, "call from child thread ");
    pthread_exit(&child);
}

extern "C"
JNIEXPORT void JNICALL
Java_kimascend_com_player_Demo_callBackFromC(JNIEnv *env, jobject instance) {
    javaListener = new JavaListener(javaVM, env, env->NewGlobalRef(instance));
    //在主线程调用
//    javaListener->onError(1, 100, "c++ call java meid from main thread!");

     pthread_create(&child,NULL,callJaveInThread,javaListener);
}
*/

JavaVM *javaVM;
JavaInvoke *javaInvoke;
FFmpeg *fFmpeg;
Status *status;
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved){
    JNIEnv *env;
    javaVM=vm;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT void JNICALL
Java_kimascend_com_player_Player_prepare(JNIEnv *env, jobject instance, jstring source_) {
    const char *source = env->GetStringUTFChars(source_, 0);
    if (javaInvoke == NULL) {
        javaInvoke = new JavaInvoke(javaVM, env, instance);
    }

    if (status == NULL) {
        status = new Status();
    }

    if (fFmpeg == NULL) {
        fFmpeg = new FFmpeg(status,javaInvoke, source);
    }
    fFmpeg->prepare();

//    env->ReleaseStringUTFChars(source_, source);
}

extern "C"
JNIEXPORT void JNICALL
Java_kimascend_com_player_Player_start(JNIEnv *env, jobject instance) {

    if (fFmpeg != NULL) {
        fFmpeg->start();
    }

}

FILE *pcmFile=NULL;
uint8_t *out_buffer = NULL;

SLObjectItf engineObject=NULL;
SLEngineItf engineEngine=NULL;


SLObjectItf outputMixObject=NULL;
SLEnvironmentalReverbItf  outputEnviromentReverb;
SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

SLObjectItf pcmPlayObject=NULL;

SLPlayItf pcmPlayerPlay=NULL;

SLAndroidSimpleBufferQueueItf pcmBufferQueue=NULL;

SLVolumeItf  pcmPlayVolume=NULL;

void *buffer;

void getPcmData(void **pcm){
    while (!feof(pcmFile)) {
        fread(out_buffer, 1, FRAME_SIZE, pcmFile);
        if (out_buffer == NULL) {
            LOGI("read end");
            break;
        } else{
            LOGI("reading")
        }
        *pcm=out_buffer;
        break;
    }
}

void pcmBufferCallBack(SLAndroidSimpleBufferQueueItf bf, void *context){
    getPcmData(&buffer);
    if (NULL != buffer) {
        SLresult  result;
        result = (*pcmBufferQueue)->Enqueue(pcmBufferQueue, buffer, FRAME_SIZE);
    }

}


extern "C"
JNIEXPORT void JNICALL
Java_kimascend_com_player_Player_play(JNIEnv *env, jobject instance, jstring url_) {
    const char *url = env->GetStringUTFChars(url_, 0);

   pcmFile = fopen(url, "r");
    if (pcmFile == NULL) {
        LOGE("fail to open file:%s", url);
        return ;
    }
    out_buffer =(uint8_t *)malloc(FRAME_SIZE);

    SLresult result;

    slCreateEngine(&engineObject, 0, 0, 0, 0, 0);
    (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);

    const SLInterfaceID mixs[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, mixs, mreq);
    result=(*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    result=(*outputMixObject)->GetInterface(outputMixObject,SL_IID_ENVIRONMENTALREVERB,&outputEnviromentReverb);
    if (SL_RESULT_SUCCESS == result) {
        result = (*outputEnviromentReverb)->SetEnvironmentalReverbProperties(outputEnviromentReverb,
                                                                             &reverbSettings);
    }

    SLDataLocator_OutputMix outputMix={SL_DATALOCATOR_OUTPUTMIX,outputMixObject};
    SLDataLocator_AndroidSimpleBufferQueue android_queue={SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,2};
    SLDataFormat_PCM pcm={
            SL_DATAFORMAT_PCM,
            2,
            SL_SAMPLINGRATE_44_1,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT|SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN
    };
    SLDataSource slDataSource = {&android_queue, &pcm};
    SLDataSink dataSink = {&outputMix, NULL};
    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &pcmPlayObject, &slDataSource,
                                                &dataSink, 3, ids, req);
    (*pcmPlayObject)->Realize(pcmPlayObject, SL_BOOLEAN_FALSE);
    (*pcmPlayObject)->GetInterface(pcmPlayObject, SL_IID_PLAY, &pcmPlayerPlay);

    (*pcmPlayObject)->GetInterface(pcmPlayObject, SL_IID_BUFFERQUEUE, &pcmBufferQueue);

    (*pcmBufferQueue)->RegisterCallback(pcmBufferQueue, pcmBufferCallBack, NULL);
    (*pcmPlayObject)->GetInterface(pcmPlayObject, SL_IID_VOLUME, &pcmPlayVolume);

    (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);


    pcmBufferCallBack(pcmBufferQueue, NULL);

    env->ReleaseStringUTFChars(url_, url);
}