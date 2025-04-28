/**
 * @file constants.h
 * @brief Constants used throughout the mod
 */
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <array>
#include "version.h"
#include "logger.h"

namespace Constants
{
    // Version information from version.h
    constexpr const char *MOD_VERSION = Version::VERSION_STRING;
    constexpr const char *MOD_NAME = Version::MOD_NAME;
    constexpr const char *MOD_WEBSITE = Version::REPOSITORY;

    // File extensions
    constexpr const char *INI_FILE_EXTENSION = ".ini";
    constexpr const char *LOG_FILE_EXTENSION = ".log";

    // Get full configuration filename
    inline std::string getConfigFilename()
    {
        return std::string(MOD_NAME) + INI_FILE_EXTENSION;
    }

    // Get full log filename
    inline std::string getLogFilename()
    {
        return std::string(MOD_NAME) + LOG_FILE_EXTENSION;
    }

    // Default configuration values
    constexpr enum LogLevel DEFAULT_LOG_LEVEL = LOG_INFO;

    // Target game modules - array of possible module names to try
    constexpr std::array<const char *, 2> MODULE_NAMES = {
        "OblivionRemastered-Win64-Shipping.exe", // Steam version
        "OblivionRemastered-WinGDK-Shipping.exe" // Gamepass version
    };

    // AOB pattern for the weight calculation function prologue
    // Identifies the function at OblivionRemastered-Win64-Shipping.exe+6666BF0
    // Instruction sequence:
    //   41 54        - push r12
    //   41 56        - push r14
    //   48 83 EC 58  - sub rsp, 58h
    //   4C 8B F1     - mov r14, rcx
    //   0F 29 74 24 40 - movaps [rsp+40h], xmm6
    //   48 8B 49 08  - mov rcx, [rcx+08h]
    //   45 33 E4     - xor r12d, r12d
    //   48 8B 01     - mov rax, [rcx]
    constexpr const char *CARRY_WEIGHT_AOB =
        "41 54 41 56 48 83 EC ?? 4C 8B F1 0F 29 74 24 ?? "
        "48 8B 49 ?? 45 33 E4 48 8B 01";
}

#endif // CONSTANTS_H
