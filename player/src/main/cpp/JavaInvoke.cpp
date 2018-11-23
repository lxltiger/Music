//
// Created by Administrator on 2018/11/19 0019.
//

#include "JavaInvoke.h"

JavaInvoke::JavaInvoke(JavaVM *jvm, JNIEnv *env, jobject jobj) {
    this->jvm = jvm;
    this->env=env;
    this->jobj=env->NewGlobalRef(jobj);

    jclass cls = env->GetObjectClass(jobj);
    if (!cls) {
        LOGE("fail to get cls ");
        return ;
    }
    //获取Java方法的method id
    this->methodPreparedId=env->GetMethodID(cls, "onPrepared", "()V");
    this->methodLoadId=env->GetMethodID(cls, "onLoad", "(Z)V");
    this->methodPlayingId = env->GetMethodID(cls, "onPlaying", "(II)V");
}

JavaInvoke::~JavaInvoke() {

}

void JavaInvoke::onPrepared(int threadType) {

    if (mainThread == threadType) {

        env->CallVoidMethod(jobj, methodPreparedId);

    } else if (childThread == threadType) {

        JNIEnv *jniEnv;
        if(jvm->AttachCurrentThread(&jniEnv,0)!=JNI_OK){
            LOGE("get child thread wrong")
            return ;
        }

        jniEnv->CallVoidMethod(jobj, methodPreparedId);
        jvm->DetachCurrentThread();

    }

}

void JavaInvoke::onLoad(int threadType, bool loading) {
    if (mainThread == threadType) {

        env->CallVoidMethod(jobj, methodLoadId,loading);

    } else if (childThread == threadType) {

        JNIEnv *jniEnv;
        if(jvm->AttachCurrentThread(&jniEnv,0)!=JNI_OK){
            LOGE("get child thread wrong")
            return ;
        }

        jniEnv->CallVoidMethod(jobj, methodLoadId,loading);
        jvm->DetachCurrentThread();

    }
}

void JavaInvoke::onPlaying(int threadType, int current, int total) {
    if (mainThread == threadType) {

        env->CallVoidMethod(jobj, methodPlayingId,current,total);

    } else if (childThread == threadType) {

        JNIEnv *jniEnv;
        if(jvm->AttachCurrentThread(&jniEnv,0)!=JNI_OK){
            LOGE("get child thread wrong")
            return ;
        }

        jniEnv->CallVoidMethod(jobj, methodPlayingId,current,total);
        jvm->DetachCurrentThread();

    }
}


