/**
 * @file mod_core.cpp
 * @brief Core functionality implementation shared between OBSE and ASI loaders
 */

#include "mod_core.h"
#include "constants.h"
#include "version.h"
#include "utils.h"

#include <windows.h>
#include <psapi.h>

// DetourModKit includes
#include <DetourModKit/logger.hpp>
#include <DetourModKit/hook_manager.hpp>

// Global handles
HMODULE g_GameHandle = nullptr;

// Function pointer for the original weight calculation function
typedef void (*OriginalFunction_t)(void *playerData);
OriginalFunction_t g_OriginalFunction = nullptr;

// Hook ID for management
std::string g_HookId;

// Hook function to set the weight to zero
void HookFunction(void *playerData)
{
    if (playerData != nullptr)
    {
        // Set the weight value at [playerData + 0x10] to 0.0f
        *(float *)(reinterpret_cast<uintptr_t>(playerData) + 0x10) = 0.0f;
    }
    // Call the original function
    if (g_OriginalFunction)
    {
        g_OriginalFunction(playerData);
    }
}

bool InitializeMod()
{
    // Configure DetourModKit Logger
    DetourModKit::Logger::configure(Constants::MOD_NAME, Constants::getLogFilename(), Constants::LOG_TIMESTAMP_FORMAT);
    auto &logger = DetourModKit::Logger::getInstance();
    logger.setLogLevel(DetourModKit::LOG_INFO);

    // Log version information
    Version::logVersionInfo();

    std::string plugin_directory = getPluginDirectory();
    logger.log(DetourModKit::LOG_INFO, "Plugin Directory: " + plugin_directory);

    // Try to find the game module - try each possible module name in sequence
    logger.log(DetourModKit::LOG_INFO, "Looking for game executable...");
    for (const char *moduleName : Constants::MODULE_NAMES)
    {
        g_GameHandle = GetModuleHandleA(moduleName);
        if (g_GameHandle)
        {
            logger.log(DetourModKit::LOG_INFO, "Game module found: " + std::string(moduleName));
            break;
        }
    }

    if (!g_GameHandle)
    {
        logger.log(DetourModKit::LOG_ERROR, "Failed to get module handle for any supported game version");
        return false;
    }

    MODULEINFO moduleInfo;
    if (!GetModuleInformation(GetCurrentProcess(), g_GameHandle, &moduleInfo, sizeof(moduleInfo)))
    {
        logger.log(DetourModKit::LOG_ERROR, "Failed to get module information");
        return false;
    }

    uintptr_t moduleBase = reinterpret_cast<uintptr_t>(moduleInfo.lpBaseOfDll);
    size_t moduleSize = moduleInfo.SizeOfImage;

    // Create hook using DetourModKit's HookManager with AOB scanning
    auto &hookManager = DetourModKit::HookManager::getInstance();

    DetourModKit::HookConfig config;
    config.autoEnable = true;

    // Use create_inline_hook_aob for automatic AOB scanning and hooking
    g_HookId = hookManager.create_inline_hook_aob(
        "CarryWeightHook",
        moduleBase,
        moduleSize,
        Constants::CARRY_WEIGHT_AOB,
        0, // No offset from pattern start
        reinterpret_cast<void *>(&HookFunction),
        reinterpret_cast<void **>(&g_OriginalFunction),
        config);

    if (g_HookId.empty())
    {
        logger.log(DetourModKit::LOG_ERROR, "Failed to create hook - check AOB pattern or target function");
        return false;
    }

    logger.log(DetourModKit::LOG_INFO, "Hook created successfully: " + g_HookId);

    logger.log(DetourModKit::LOG_INFO, "Mod enabled successfully");
    return true;
}

void CleanupMod()
{
    auto &logger = DetourModKit::Logger::getInstance();

    // Remove hooks using DetourModKit's HookManager
    if (!g_HookId.empty())
    {
        auto &hookManager = DetourModKit::HookManager::getInstance();
        if (hookManager.remove_hook(g_HookId))
        {
            logger.log(DetourModKit::LOG_INFO, "Hook removed successfully");
        }
        else
        {
            logger.log(DetourModKit::LOG_WARNING, "Failed to remove hook or hook already removed");
        }
        g_HookId.clear();
    }

    logger.log(DetourModKit::LOG_INFO, "Mod disabled");
}
