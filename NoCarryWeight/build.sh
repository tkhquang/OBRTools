#!/bin/bash
# Build script for OBR-NoCarryWeight mod
# Creates both OBSE plugin and ASI loader versions

# Common compiler flags for size optimization
COMMON_FLAGS="-std=c++20 -m64 -Os -flto -ffunction-sections -fdata-sections -Wall -Wextra -DWIN32_LEAN_AND_MEAN -DNOMINMAX"

INCLUDE_DIRS="-Isrc -Iexternal/minhook/include -Iexternal/obse64"

LINKER_FLAGS="-shared -Wl,--add-stdcall-alias -static -static-libgcc -static-libstdc++ -Wl,--gc-sections"

MINHOOK_SRC="external/minhook/src/buffer.c external/minhook/src/hook.c external/minhook/src/trampoline.c external/minhook/src/hde/hde32.c external/minhook/src/hde/hde64.c"

COMMON_SRC="src/mod_core.cpp src/aob_scanner.cpp src/logger.cpp src/version.cpp"

LIBRARIES="-luser32 -lkernel32 -lpsapi"

# Ensure build directory exists
mkdir -p build

# For OBSE64 Plugin
echo "Building OBSE64 Plugin..."
g++ $COMMON_FLAGS $INCLUDE_DIRS $LINKER_FLAGS \
    $COMMON_SRC src/dllmain_obse64.cpp $MINHOOK_SRC \
    -o build/OBR-NoCarryWeight.dll \
    $LIBRARIES

if [ $? -ne 0 ]; then
    echo "OBSE64 Plugin build failed!"
    exit 1
fi

# Strip debug symbols
strip --strip-all build/OBR-NoCarryWeight.dll

# Apply UPX compression if available
# if command -v upx &> /dev/null; then
#     echo "Applying UPX compression to DLL..."
#     upx --best --lzma build/OBR-NoCarryWeight.dll
# fi

echo "OBSE64 DLL Size: $(du -h build/OBR-NoCarryWeight.dll | cut -f1)"

# For ASI Loader
echo "Building ASI Plugin..."
g++ $COMMON_FLAGS $INCLUDE_DIRS $LINKER_FLAGS \
    $COMMON_SRC src/dllmain.cpp $MINHOOK_SRC \
    -o build/OBR-NoCarryWeight.asi \
    $LIBRARIES

if [ $? -ne 0 ]; then
    echo "ASI Plugin build failed!"
    exit 1
fi

# Strip debug symbols
strip --strip-all build/OBR-NoCarryWeight.asi

# # Apply UPX compression if available
# if command -v upx &> /dev/null; then
#     echo "Applying UPX compression to ASI..."
#     upx --best --lzma build/OBR-NoCarryWeight.asi
# fi

echo "ASI Plugin Size: $(du -h build/OBR-NoCarryWeight.asi | cut -f1)"
echo "Build complete!"
