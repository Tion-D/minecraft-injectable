#include "Player.h"
#include <Windows.h>
#include "..\ext\JNI\jni.h"

CPlayer::CPlayer(jobject instance)
{
    this->playerInstance = instance;
}

void CPlayer::Cleanup()
{
    if (this->playerInstance)
    {
        lc->env->DeleteLocalRef(this->playerInstance);
        this->playerInstance = nullptr;
    }
}

jclass CPlayer::GetClass()
{
    jclass clazz = lc->GetClass("net.minecraft.entity.Entity");
    if (!clazz)
    {
        std::cerr << "Failed to find Entity class" << std::endl;
        return nullptr;
    }
    return clazz;
}
