#include <windows.h>
#include "SDK/Minecraft.h"
#include <chrono>
#include <thread>
#include <iostream>

FILE* file = nullptr;
JNIEnv* env = nullptr;

extern void SetupGLHook();

void init(void* instance)
{
    jsize count;
    if (JNI_GetCreatedJavaVMs(&lc->vm, 1, &count) != JNI_OK || count == 0) {
        std::cerr << "Failed to get Java VM" << std::endl;
        return;
    }

    jint res = lc->vm->GetEnv((void**)&lc->env, JNI_VERSION_1_8);
    if (res == JNI_EDETACHED)
    {
        if (lc->vm->AttachCurrentThread((void**)&lc->env, nullptr) != JNI_OK) {
            std::cerr << "Failed to attach thread to Java VM" << std::endl;
            return;
        }
    }

    if (lc->env == nullptr)
    {
        std::cerr << "JNIEnv is null" << std::endl;
        return;
    }

    lc->GetLoadedClasses();
    std::cout << "Classes loaded successfully!" << std::endl;

    std::thread([]() {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        SetupGLHook();
        }).detach();

    while (true)
    {
        if (GetAsyncKeyState(VK_END) & 1)
            break;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    fclose(file);
    FreeConsole();
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 0);
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved)
{
    DisableThreadLibraryCalls(hModule);
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        AllocConsole();
        freopen_s(&file, "CONOUT$", "w", stdout);
        CloseHandle(CreateThread(nullptr, 0,
            reinterpret_cast<LPTHREAD_START_ROUTINE>(init),
            hModule, 0, nullptr));
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}