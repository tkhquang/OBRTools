/**
 * @file utils.h
 * @brief Header for utility functions specific to this mod.
 */
#ifndef UTILS_H
#define UTILS_H

#include <windows.h>
#include <string>

#include <obse64/PluginAPI.h>

// Forward declarations of externals
extern OBSEMessagingInterface *g_ObseMessage;

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
