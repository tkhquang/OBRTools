/**
 * @file mod_core.h
 * @brief Core functionality shared between OBSE and ASI loaders
 */
#ifndef MOD_CORE_H
#define MOD_CORE_H

/**
 * @brief Initializes the mod by finding target function and hooking it
 * @return bool True if initialization succeeded, false otherwise
 */
bool InitializeMod();

/**
 * @brief Cleans up the mod resources when unloading
 */
void CleanupMod();

#endif // MOD_CORE_H
