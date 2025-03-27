#pragma once
#include "Player.h"
#include "..\ext\JNI\jni.h"
#include <vector>

class CMinecraft
{
public:
    jclass GetClass();
    jobject GetInstance();

    CPlayer GetLocalPlayer();
    std::vector<jobject> GetEntities();
};

jclass getMinecraftClass();
jobject getMinecraft();
