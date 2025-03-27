#pragma once
#include "../Java.h"
#include <../ext/JNI/jni.h>

class CPlayer
{
public:
    CPlayer(jobject instance);
    jclass GetClass();
    void Cleanup();

private:
    jobject playerInstance;
};
