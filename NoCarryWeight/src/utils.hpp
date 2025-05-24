/**
 * @file utils.h
 * @brief Header for utility functions specific to this mod.
 */
#ifndef UTILS_H
#define UTILS_H

#include <windows.h>
#include <string>

#include <obse64/PluginAPI.h>
#include <DetourModKit.hpp>

// Forward declarations of externals
extern OBSEMessagingInterface *g_ObseMessage;

/**
 * @brief Gets the appropriate plugin directory
 */
inline std::string getPluginDirectory()
{
    std::string runtimeDirectory = DMKFilesystem::getRuntimeDirectory();

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
