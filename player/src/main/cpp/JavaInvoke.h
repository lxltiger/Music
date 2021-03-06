//
// Created by Administrator on 2018/11/19 0019.
//

#ifndef MUSIC_JAVAINVOKE_H
#define MUSIC_JAVAINVOKE_H

#include <jni.h>
#include "AndroidLog.h"

#define mainThread 0
#define childThread 1
/**
 * C++对Java类的调用
 */
class JavaInvoke {

public:
    JavaVM *jvm;
    JNIEnv *env;
    jobject  jobj;

    jmethodID  methodPreparedId;
    jmethodID  methodLoadId;
    jmethodID  methodPlayingId;
    jmethodID  methodOnErrorId;
    jmethodID  methodOnCompleteId;
    jmethodID  methodPCM2AACId;

public:
    JavaInvoke( JavaVM *jvm,JNIEnv *env,jobject  jobj);
    ~JavaInvoke();

    //解码准备完成的回调
    void onPrepared(int threadType);
    void onComplete(int threadType);


    void onLoad(int threadType, bool loading);

    void onPlaying(int threadType, int current,int total);

    void onError(int threadType, int code, const char *msg);

    void pcm2aac(int threadType,int size,void *buffer);

};


#endif //MUSIC_JAVAINVOKE_H
