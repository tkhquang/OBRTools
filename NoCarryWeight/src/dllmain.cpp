/**
 * @file dllmain.cpp
 * @brief Entry point for ASI loader version
 */
#include "mod_core.hpp"

#include <windows.h>
#include <DetourModKit.hpp>

#include <obse64/PluginAPI.h>

// Define the global variables with null values for ASI loader
OBSEMessagingInterface *g_ObseMessage = nullptr;
PluginHandle g_PluginHandle = kPluginHandle_Invalid;
HMODULE g_DllHandle = nullptr;

// Thread function that initializes the mod
DWORD WINAPI InitThread(LPVOID)
{
    if (InitializeMod())
    {
        return TRUE;
    }
    return FALSE;
}

// DLL entry point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        g_DllHandle = hModule;
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, InitThread, NULL, 0, NULL);
        break;

    case DLL_PROCESS_DETACH:
        CleanupMod();
        DMKLogger::getInstance().log(DMKLogLevel::LOG_INFO, "DLL detached");
        break;
    }
    return TRUE;
}
