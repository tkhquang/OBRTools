/**
 * @file utils.h
 * @brief Header for general utility functions used across the mod.
 */
#ifndef UTILS_H
#define UTILS_H

#include <windows.h>
#include <psapi.h>

#include "constants.h"
#include "logger.h"

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstdint>

#include <obse64/PluginAPI.h>

// Forward declarations of externals
extern OBSEMessagingInterface *g_ObseMessage;

// --- String Formatting Utilities ---

/**
 * @brief Formats a memory address into a standard hex string
 */
inline std::string format_address(uintptr_t address)
{
    std::ostringstream oss;
    oss << "0x" << std::hex << std::uppercase
        << std::setw(sizeof(uintptr_t) * 2)
        << std::setfill('0') << address;
    return oss.str();
}

/**
 * @brief Formats an integer as a 2-digit uppercase hex string
 */
inline std::string format_hex(int value)
{
    std::ostringstream oss;
    oss << "0x" << std::uppercase << std::hex
        << std::setw(2) << std::setfill('0')
        << (value & 0xFF);
    return oss.str();
}

/**
 * @brief Trims leading/trailing whitespace from a string
 */
inline std::string trim(const std::string &s)
{
    auto first = s.find_first_not_of(" \t\r\n");
    if (std::string::npos == first)
    {
        return s;
    }
    auto last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, (last - first + 1));
}

/**
 * @brief Gets the path to the running executable
 */
inline std::string getRuntimePath()
{
    static char appPath[MAX_PATH] = {0};

    if (appPath[0])
        return appPath;

    GetModuleFileName(GetModuleHandle(NULL), appPath, sizeof(appPath));
    return appPath;
}

/**
 * @brief Gets the directory containing the running executable
 */
inline const std::string &getRuntimeDirectory()
{
    static std::string s_runtimeDirectory;

    if (s_runtimeDirectory.empty())
    {
        std::string runtimePath = getRuntimePath();
        std::string::size_type lastSlash = runtimePath.rfind('\\');

        if (lastSlash != std::string::npos)
        {
            s_runtimeDirectory = runtimePath.substr(0, lastSlash + 1);
        }
        else
        {
            Logger::getInstance().log(LOG_WARNING, "No slash found in runtime path");
        }
    }

    return s_runtimeDirectory;
}

/**
 * @brief Gets the appropriate plugin directory
 */
inline std::string getPluginDirectory()
{
    std::string runtimeDirectory = getRuntimeDirectory();

    // Check if we're running under OBSE
    if (g_ObseMessage)
    {
        if (!runtimeDirectory.empty())
        {
            return runtimeDirectory + "OBSE\\Plugins\\";
        }
    }

    return runtimeDirectory;
}

#endif // UTILS_H
