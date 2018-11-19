//
// Created by Administrator on 2018/11/16 0016.
//
#include "JavaListener.h"

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



JavaListener::JavaListener(JavaVM *vm, JNIEnv *env, jobject obj) {
    jvm = vm;
    jniEnv = env;
    jobj = obj;

    jclass cls = env->GetObjectClass(obj);
    if (!cls) {
        return;
    }
    jmid = env->GetMethodID(cls, "onError", "(ILjava/lang/String;)V");

    if (!jmid) {
        return;
    }

}

JavaListener::~JavaListener() {

}

void JavaListener::onError(int threadType, int code, const char *msg) {
    if (threadType == 0) {
        JNIEnv *env;
        jvm->AttachCurrentThread(&env, NULL);

        jstring stringUTF = env->NewStringUTF(msg);
        env->CallVoidMethod(jobj, jmid, code, stringUTF);
        env->DeleteLocalRef(stringUTF);

        jvm->DetachCurrentThread();

    } else if (threadType == 1) {
        jstring stringUTF = jniEnv->NewStringUTF(msg);
        jniEnv->CallVoidMethod(jobj, jmid, code, stringUTF);
        jniEnv->DeleteLocalRef(stringUTF);
    }

}
