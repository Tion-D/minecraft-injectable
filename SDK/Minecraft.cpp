#include "Minecraft.h"
#include <vector>
#include <iostream>

extern std::unique_ptr<Lunar> lc;

jclass getMinecraftClass()
{
    return lc->env->FindClass("net/minecraft/client/Minecraft");
}

jobject getMinecraft()
{
    jclass mcClass = getMinecraftClass();
    jmethodID getMC = lc->env->GetStaticMethodID(mcClass, "getMinecraft", "()Lnet/minecraft/client/Minecraft;");
    return lc->env->CallStaticObjectMethod(mcClass, getMC);
}

// CMinecraft methods:

jclass CMinecraft::GetClass()
{
    return lc->GetClass("net.minecraft.client.Minecraft");
}

jobject CMinecraft::GetInstance()
{
    jclass mcClass = this->GetClass();
    jmethodID getMinecraft = lc->env->GetStaticMethodID(mcClass, "getMinecraft", "()Lnet/minecraft/client/Minecraft;");
    jobject mcInstance = lc->env->CallStaticObjectMethod(mcClass, getMinecraft);
    return mcInstance;
}

CPlayer CMinecraft::GetLocalPlayer()
{
    jclass mcClass = this->GetClass();
    jobject mcInstance = this->GetInstance();
    if (!mcInstance) {
        std::cerr << "Failed to get Minecraft instance" << std::endl;
        return CPlayer(nullptr);
    }

    jfieldID playerField = lc->env->GetFieldID(mcClass, "thePlayer", "Lnet/minecraft/client/entity/EntityPlayerSP;");
    if (!playerField) {
        std::cerr << "Failed to find thePlayer field" << std::endl;
        return CPlayer(nullptr);
    }

    jobject playerObject = lc->env->GetObjectField(mcInstance, playerField);
    lc->env->DeleteLocalRef(mcInstance);

    return CPlayer(playerObject);
}

std::vector<jobject> CMinecraft::GetEntities()
{
    std::vector<jobject> result;

    jclass mcClass = this->GetClass();
    jobject mcInstance = this->GetInstance();
    if (!mcInstance) {
        std::cerr << "Failed to get MC instance for Entities" << std::endl;
        return result;
    }

    jfieldID worldField = lc->env->GetFieldID(mcClass, "world", "Lnet/minecraft/world/World;");
    if (!worldField) {
        std::cerr << "Failed to find 'world' field" << std::endl;
        lc->env->DeleteLocalRef(mcInstance);
        return result;
    }
    jobject worldObj = lc->env->GetObjectField(mcInstance, worldField);

    jclass worldClass = lc->GetClass("net.minecraft.world.World");
    if (!worldClass) {
        std::cerr << "Failed to get net.minecraft.world.World" << std::endl;
        lc->env->DeleteLocalRef(worldObj);
        lc->env->DeleteLocalRef(mcInstance);
        return result;
    }

    jmethodID getEntities = lc->env->GetMethodID(worldClass, "getLoadedEntityList", "()Ljava/util/List;");
    if (!getEntities) {
        std::cerr << "Failed to find getLoadedEntityList" << std::endl;
        lc->env->DeleteLocalRef(worldObj);
        lc->env->DeleteLocalRef(mcInstance);
        return result;
    }

    jobject entityList = lc->env->CallObjectMethod(worldObj, getEntities);
    if (!entityList) {
        lc->env->DeleteLocalRef(worldObj);
        lc->env->DeleteLocalRef(mcInstance);
        return result;
    }

    jclass listClass = lc->env->FindClass("java/util/List");
    jmethodID listSize = lc->env->GetMethodID(listClass, "size", "()I");
    jmethodID listGet = lc->env->GetMethodID(listClass, "get", "(I)Ljava/lang/Object;");

    jint size = lc->env->CallIntMethod(entityList, listSize);
    for (jint i = 0; i < size; i++)
    {
        jobject ent = lc->env->CallObjectMethod(entityList, listGet, i);
        result.push_back(ent);
    }

    lc->env->DeleteLocalRef(entityList);
    lc->env->DeleteLocalRef(worldObj);
    lc->env->DeleteLocalRef(mcInstance);

    return result;
}
