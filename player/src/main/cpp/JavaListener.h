//
// Created by Administrator on 2018/11/16 0016.
//

#ifndef MUSIC_JAVALISTENER_H
#define MUSIC_JAVALISTENER_H


#include <jni.h>

class JavaListener {
public:
    JavaVM *jvm;
    JNIEnv *jniEnv;
    jobject jobj;
    jmethodID jmid;
public:
    JavaListener(JavaVM *javaVM, JNIEnv *jniEnv, jobject jobj);

    ~JavaListener();

    void onError(int threadType, int code, const char *msg);
};


#endif //MUSIC_JAVALISTENER_H
