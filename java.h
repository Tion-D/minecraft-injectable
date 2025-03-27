#pragma once
#include "../ext/JNI/jni.h"
#include "../ext/JNI/jvmti.h"
#include <mutex>
#include <unordered_map>
#include <string>
#include <iostream>

class Lunar
{
public:
    JNIEnv* env;
    JavaVM* vm;

    Lunar() : env(nullptr), vm(nullptr) {}

    void GetLoadedClasses()
    {
        jvmtiEnv* jvmti;
        if (vm->GetEnv((void**)&jvmti, JVMTI_VERSION_1_2) != JNI_OK) return;

        jclass lang = env->FindClass("java/lang/Class");
        jmethodID getName = env->GetMethodID(lang, "getName", "()Ljava/lang/String;");

        jclass* classesPtr;
        jint amount;

        jvmti->GetLoadedClasses(&amount, &classesPtr);

        for (int i = 0; i < amount; i++)
        {
            jstring name = (jstring)env->CallObjectMethod(classesPtr[i], getName);
            const char* className = env->GetStringUTFChars(name, 0);
            env->ReleaseStringUTFChars(name, className);

            // Print out or store the class name
            std::cout << className << std::endl;

            classes.emplace(std::make_pair(std::string(className), classesPtr[i]));
        }
    }

    jclass GetClass(const std::string& classname)
    {
        if (classes.contains(classname))
            return classes.at(classname);
        return nullptr;
    }
private:
    std::unordered_map<std::string, jclass> classes;
};





inline auto lc = std::make_unique<Lunar>();
