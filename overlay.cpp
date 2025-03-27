#include <windows.h>
#include <gl/GL.h>
#include <thread>
#include <iostream>
#include <include/MinHook.h>
#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_opengl3.h"
#include "ext/JNI/jni.h"
#include "java.h"
#include <backends/imgui_impl_win32.cpp>

typedef BOOL(WINAPI* twglSwapBuffers)(HDC hdc);
twglSwapBuffers oWglSwapBuffers = nullptr;

bool g_ShowMenu = false;

WNDPROC oWndProc = nullptr;
LRESULT CALLBACK WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (g_ShowMenu && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
    return CallWindowProc(oWndProc, hWnd, msg, wParam, lParam);
}

void SetMouseGrabbed(bool grabbed)
{
    JNIEnv* env = lc->env;
    if (!env) return;

    jclass mouseClass = env->FindClass("org/lwjgl/input/Mouse");
    if (!mouseClass) return;

    jmethodID setGrabbed = env->GetStaticMethodID(mouseClass, "setGrabbed", "(Z)V");
    if (!setGrabbed) return;

    env->CallStaticVoidMethod(mouseClass, setGrabbed, grabbed ? JNI_TRUE : JNI_FALSE);
}

BOOL WINAPI hkWglSwapBuffers(HDC hdc)
{
    static bool g_ImGuiInitialized = false;
    static bool lastMenuState = false;

    if (!g_ImGuiInitialized)
    {
        HWND gameWindow = WindowFromDC(hdc);
        if (!gameWindow) return oWglSwapBuffers(hdc);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        if (!ImGui_ImplWin32_Init(gameWindow)) return oWglSwapBuffers(hdc);
        if (!ImGui_ImplOpenGL3_Init("#version 130")) return oWglSwapBuffers(hdc);

        oWndProc = (WNDPROC)SetWindowLongPtr(gameWindow, GWLP_WNDPROC, (LONG_PTR)WndProcHook);

        g_ImGuiInitialized = true;
    }

    if (GetAsyncKeyState(VK_INSERT) & 1 || GetAsyncKeyState(VK_RCONTROL) & 1)
    {
        g_ShowMenu = !g_ShowMenu;
    }

    if (g_ShowMenu)
        SetMouseGrabbed(false);
    else if (g_ShowMenu != lastMenuState)
        SetMouseGrabbed(true);

    lastMenuState = g_ShowMenu;

    if (g_ShowMenu)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Player ESP Menu");
        static bool espEnabled = false;
        ImGui::Checkbox("Enable Player ESP", &espEnabled);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    return oWglSwapBuffers(hdc);
}

void SetupGLHook()
{
    MH_Initialize();
    HMODULE hMod = GetModuleHandleA("gdi32.dll");
    FARPROC pSwapBuffers = GetProcAddress(hMod, "SwapBuffers");
    MH_CreateHook(pSwapBuffers, &hkWglSwapBuffers, (LPVOID*)&oWglSwapBuffers);
    MH_EnableHook(pSwapBuffers);
}
