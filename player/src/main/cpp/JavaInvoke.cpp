/**
 *主线程中 C++调用Java方法
 * 1.获取对象类  jclass cls = env->GetObjectClass(obj);
 * 2.获取方法id      jmid = env->GetMethodID(cls, "onError", "(ILjava/lang/String;)V");
 * 3.调用对应方法env->CallVoidMethod(jobj, jmid, code, stringUTF);
 *
 * jniEnv是线程相关的，所以不能使用创建线程的jniEnv,但JVM是进程相关的 通过JVM获取子线程的jniEnv
 * 1.获取JVM对象 JNI_OnLoad
 * 2.        jvm->AttachCurrentThread(&env, NULL);

            和主线程一样的方法调用
            jvm->DetachCurrentThread();

 */

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
    this->methodOnErrorId = env->GetMethodID(cls, "onError", "(ILjava/lang/String;)V");
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

void JavaInvoke::onError(int threadType, int code, const char *msg) {
    if (mainThread == threadType) {
        jstring str = env->NewStringUTF(msg);
        env->CallVoidMethod(jobj, methodOnErrorId,code,str);
        env->DeleteLocalRef(str);

    } else if (childThread == threadType) {

        JNIEnv *jniEnv;
        if(jvm->AttachCurrentThread(&jniEnv,0)!=JNI_OK){
            LOGE("get child thread wrong")
            return ;
        }
        jstring str = jniEnv->NewStringUTF(msg);
        jniEnv->CallVoidMethod(jobj, methodOnErrorId,code,str);
        jniEnv->DeleteLocalRef(str);
        jvm->DetachCurrentThread();

    }
}


