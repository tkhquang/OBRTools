/**
 * @file mod_core.cpp
 * @brief Core functionality implementation shared between OBSE and ASI loaders
 */

#include "mod_core.h"
#include "aob_scanner.h"
#include "constants.h"
#include "logger.h"
#include "version.h"
#include "utils.h"

#include <windows.h>
#include <psapi.h>

#include <MinHook.h>

// Global handles
HMODULE g_GameHandle = nullptr;

// Function pointer for the original weight calculation function
typedef void (*OriginalFunction_t)(void *playerData);
OriginalFunction_t g_OriginalFunction = nullptr;

// Hook function to set the weight to zero
void HookFunction(void *playerData)
{
    if (playerData != nullptr)
    {
        // Set the weight value at [playerData + 0x10] to 0.0f
        *(float *)(reinterpret_cast<uintptr_t>(playerData) + 0x10) = 0.0f;
    }
    // Call the original function
    g_OriginalFunction(playerData);
}

bool InitializeMod()
{
    auto &logger = Logger::getInstance();
    Version::logVersionInfo();

    std::string plugin_directory = getPluginDirectory();
    logger.log(LOG_INFO, "Plugin Directory: " + plugin_directory);

    // Initialize MinHook library
    if (MH_Initialize() != MH_OK)
    {
        logger.log(LOG_ERROR, "Failed to initialize MinHook");
        return false;
    }

    // Get the module handle and information
    g_GameHandle = GetModuleHandleA(Constants::MODULE_NAME);

    if (!g_GameHandle)
    {
        logger.log(LOG_ERROR, "Failed to get module handle");
        return false;
    }

    MODULEINFO moduleInfo;
    if (!GetModuleInformation(GetCurrentProcess(), g_GameHandle, &moduleInfo, sizeof(moduleInfo)))
    {
        logger.log(LOG_ERROR, "Failed to get module information");
        return false;
    }

    BYTE *moduleBase = static_cast<BYTE *>(moduleInfo.lpBaseOfDll);
    size_t moduleSize = moduleInfo.SizeOfImage;

    const std::string &functionAOB = Constants::CARRY_WEIGHT_AOB;
    std::vector<BYTE> pattern = parseAOB(functionAOB);

    if (pattern.empty())
    {
        logger.log(LOG_ERROR, "Invalid AOB pattern for function");
        return false;
    }

    // Search for the pattern in the module
    BYTE *functionAddress = FindPattern(moduleBase, moduleSize, pattern);
    if (!functionAddress)
    {
        logger.log(LOG_ERROR, "Function pattern not found");
        return false;
    }

    logger.log(LOG_INFO, "Function found at " + format_address(reinterpret_cast<uintptr_t>(functionAddress)));

    // Create and enable the hook
    if (MH_CreateHook(functionAddress, reinterpret_cast<LPVOID>(&HookFunction),
                      reinterpret_cast<LPVOID *>(&g_OriginalFunction)) != MH_OK)
    {
        logger.log(LOG_ERROR, "Failed to create hook");
        return false;
    }

    if (MH_EnableHook(functionAddress) != MH_OK)
    {
        logger.log(LOG_ERROR, "Failed to enable hook");
        return false;
    }

    logger.log(LOG_INFO, "Mod enabled successfully");
    return true;
}

void CleanupMod()
{
    // Disable hooks and uninitialize MinHook
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
    Logger::getInstance().log(LOG_INFO, "Mod disabled");
}
