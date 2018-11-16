//
// Created by Administrator on 2018/11/16 0016.
//
#include "JavaListener.h"


JavaListener::JavaListener(JavaVM *vm, JNIEnv *env, jobject obj) {
    jvm=vm;
    jniEnv=env;
    jobj=obj;

    jclass cls = env->GetObjectClass(obj);
    if (!cls) {
        return ;
    }
    jmid = env->GetMethodID(cls, "onError", "(ILjava/lang/String;)V");

    if (!jmid) {
        return ;
    }

}

JavaListener::~JavaListener() {

}

void JavaListener::onError(int threadType, int code, const char *msg) {
    if (threadType == 0) {
        JNIEnv *env;
        jvm->AttachCurrentThread(&env, NULL);

        jstring stringUTF = jniEnv->NewStringUTF(msg);
        jniEnv->CallVoidMethod(jobj, jmid, code, stringUTF);
        jniEnv->DeleteLocalRef(stringUTF);

        jvm->DetachCurrentThread();

    }else if (threadType==1){
        jstring stringUTF = jniEnv->NewStringUTF(msg);
        jniEnv->CallVoidMethod(jobj, jmid, code, stringUTF);
        jniEnv->DeleteLocalRef(stringUTF);
    }

}
