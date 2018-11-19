//
// Created by Administrator on 2018/11/19 0019.
//

#include "Audio.h"

Audio::Audio(Status *status) {
    this->status=status;
    queue = new PacketQueue(status);
}

Audio::~Audio() {

}
