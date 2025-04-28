/**
 * @file dllmain_obse64.cpp
 * @brief Entry point for OBSE loader version
 */
#include "mod_core.h"
#include "logger.h"

#include <windows.h>

#include <obse64/PluginAPI.h>
// #include <obse64_common/obse64_version.h>

// Global OBSE interfaces
OBSEMessagingInterface *g_ObseMessage = nullptr;
PluginHandle g_PluginHandle = kPluginHandle_Invalid;
HMODULE g_DllHandle = nullptr;

// Thread function that initializes the mod
DWORD WINAPI InitThread(LPVOID)
{
    return InitializeMod() ? TRUE : FALSE;
}

// Handler for OBSE messages
void MessageHandler(OBSEMessagingInterface::Message *msg)
{
    if (msg->type == OBSEMessagingInterface::kMessage_PostPostLoad)
    {
        CreateThread(NULL, 0, InitThread, NULL, 0, NULL);
    }
}

// DLL entry point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        g_DllHandle = hModule;
        DisableThreadLibraryCalls(hModule);
        break;

    case DLL_PROCESS_DETACH:
        CleanupMod();
        Logger::getInstance().log(LOG_INFO, "DLL detached");
        break;
    }
    return TRUE;
}

// OBSE plugin interface
extern "C"
{
    __declspec(dllexport) OBSEPluginVersionData OBSEPlugin_Version = {
        OBSEPluginVersionData::kVersion,                         // Structure version
        2,                                                       // Plugin version (0.2.0)
        "No Carry Weight",                                       // Plugin name
        "tkhquang",                                              // Author name
        OBSEPluginVersionData::kAddressIndependence_Signatures,  // Signature-based address independence
        OBSEPluginVersionData::kStructureIndependence_NoStructs, // No game structures used
        // {RUNTIME_VERSION_0_411_140, 0},                          // Compatible runtime version
        // 0,                                                       // No specific OBSE64 version required
        // 0,                                                       // reservedNonBreaking
        // 0,                                                       // reservedBreaking
        // {0}                                                      // Reserved array
    };

    __declspec(dllexport) bool OBSEPlugin_Load(const OBSEInterface *obse)
    {
        g_PluginHandle = obse->GetPluginHandle();
        g_ObseMessage = (OBSEMessagingInterface *)obse->QueryInterface(kInterface_Messaging);
        g_ObseMessage->RegisterListener(g_PluginHandle, "OBSE", MessageHandler);

        return true;
    }
};
