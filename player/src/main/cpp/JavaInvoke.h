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

public:
    JavaInvoke( JavaVM *jvm,JNIEnv *env,jobject  jobj);
    ~JavaInvoke();

    //解码准备完成的回调
    void onPrepared(int threadType);


    void onLoad(int threadType, bool loading);

    void onPlaying(int threadType, int current,int total);

};


#endif //MUSIC_JAVAINVOKE_H
