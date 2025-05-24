# OBR-NoCarryWeight

A mod for Oblivion Remastered that removes carry weight limitations by directly hooking into the game's memory. Built with DetourModKit for improved stability and compatibility.

## Features

- **Complete carry weight removal**: Sets your character's weight to 0 when calculated
- **Memory-safe hooking**: Uses DetourModKit's SafetyHook-based implementation
- **Dual compatibility**: Available as both ASI loader plugin and OBSE plugin
- **Signature-based**: Uses Array-of-Bytes (AOB) patterns for game version independence
- **Minimal overhead**: Lightweight implementation with optimized code

## Installation

### Option 1: ASI Loader (Recommended)
1. Install [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader) or similar
2. Download the ASI version from [releases](https://github.com/tkhquang/OBRTools/releases)
3. Drop `OBR-NoCarryWeight.asi` into your game's root directory (where the .exe is located)

### Option 2: OBSE Plugin
1. Install [OBSE64](https://github.com/llde/obse64)
2. Download the OBSE version from [releases](https://github.com/tkhquang/OBRTools/releases)
3. Create `OBSE/Plugins/` folder in your game directory if it doesn't exist
4. Place `OBR-NoCarryWeight.dll` in the `OBSE/Plugins/` folder

## Supported Game Versions

- Oblivion Remastered (Steam) - `OblivionRemastered-Win64-Shipping.exe`
- Oblivion Remastered (Game Pass) - `OblivionRemastered-WinGDK-Shipping.exe`

## How It Works

The mod uses advanced pattern scanning to locate the game's carry weight calculation function and hooks it to set the weight value to 0.0f before calling the original function. This approach:

- Maintains game stability by preserving original function flow
- Works across different game versions using signature-based detection
- Provides instant effect without save file modifications

## Logging

The mod creates a log file `OBR-NoCarryWeight.log` in:
- **ASI version**: Same directory as the game executable
- **OBSE version**: `OBSE/Plugins/` directory

Check this file if you encounter any issues.

## Building from Source

### Prerequisites
- C++23 compatible compiler (MinGW-w64 recommended)
- CMake 3.16 or newer
- Git (for submodules)

### Build Steps
```bash
# Clone with submodules
git clone --recursive https://github.com/tkhquang/OBRTools.git
cd OBRTools/NoCarryWeight

# Configure with CMake
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# Build both versions
cmake --build build --config Release

# Output files will be in build/ directory:
# - OBR-NoCarryWeight.asi (ASI loader version)
# - OBR-NoCarryWeight.dll (OBSE plugin version)
```

## Dependencies

This mod is built using [DetourModKit](https://github.com/tkhquang/DetourModKit), which provides:
- SafetyHook for stable function hooking
- Advanced AOB (Array-of-Bytes) pattern scanning
- Robust logging system
- Memory utilities and safety checks

## Troubleshooting

### Mod not working?
1. Check the log file for error messages
2. Ensure you're using a supported game version
3. Verify the mod file is in the correct location
4. For ASI version: Confirm ASI loader is properly installed
5. For OBSE version: Verify OBSE64 is working correctly

### Performance issues?
The mod has minimal performance impact, but if you experience issues:
- Check for conflicts with other mods
- Verify your system meets the game's requirements
- Report issues with log file details

## Version History

### v0.3.0 (Current)
- Complete refactor using DetourModKit
- Improved stability and error handling
- Enhanced logging and debugging
- Better memory safety
- Streamlined build process with CMake

### v0.2.0
- Added OBSE plugin support
- Improved game version detection
- Better error handling

### v0.1.0
- Initial release
- Basic ASI loader support

## Contributing

Found a bug or want to contribute? Please:
1. Check existing issues on the [GitHub repository](https://github.com/tkhquang/OBRTools)
2. Create a new issue with detailed information
3. Include log files when reporting problems

## License

This project is licensed under the MIT License. See the [LICENSE](../LICENSE) file for details.

## Acknowledgments

- [DetourModKit](https://github.com/tkhquang/DetourModKit) - Core hooking and utilities framework
- [SafetyHook](https://github.com/cursey/safetyhook) - Safe and robust function hooking
- [OBSE64](https://github.com/llde/obse64) - Oblivion Script Extender for 64-bit
- [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader) - ASI plugin loading

## Disclaimer

This mod modifies game memory during runtime. While designed to be safe and non-intrusive, use at your own discretion. The authors are not responsible for any issues that may arise from using this mod.
