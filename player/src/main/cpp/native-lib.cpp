#include <jni.h>
#include <string>
#include <pthread.h>
#include <zconf.h>
//#include "queue"
//#include "JavaListener.h"
#include "JavaInvoke.h"
#include "FFmpeg.h"


extern "C"{

 #include <libavformat/avformat.h>

}

bool exiting=false;
//释放内存顺序  释放队列--释放OpenSl--释放audio--释放FFmepg

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
    javaInvoke->onLoad(mainThread, true);

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


extern "C"
JNIEXPORT void JNICALL
Java_kimascend_com_player_Player_pause(JNIEnv *env, jobject instance) {
    if (fFmpeg != NULL) {
        fFmpeg->pause();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_kimascend_com_player_Player_resume(JNIEnv *env, jobject instance) {
    if (fFmpeg != NULL) {
        fFmpeg->resume();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_kimascend_com_player_Player_stop(JNIEnv *env, jobject instance) {

    if (exiting) {
        return;
    }
    exiting=true;
    if (fFmpeg != NULL) {
        fFmpeg->release();
        delete (fFmpeg);
        fFmpeg=NULL;
    }

    if (javaInvoke != NULL) {
        delete (javaInvoke);
        javaInvoke = NULL;
    }
    if (status != NULL) {
        delete (status);
        status=NULL;
    }
    exiting=false;
}