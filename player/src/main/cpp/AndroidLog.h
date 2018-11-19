//
// Created by Administrator on 2018/11/19 0019.
//

#ifndef MUSIC_ANDROIDLOG_H
#define MUSIC_ANDROIDLOG_H

#endif //MUSIC_ANDROIDLOG_H
#include "android/log.h"

#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,"lxl",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,"lxl",FORMAT,##__VA_ARGS__);
