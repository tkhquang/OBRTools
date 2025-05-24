cmake_minimum_required(VERSION 3.16)

project(OBR-NoCarryWeight VERSION 0.3.0 LANGUAGES CXX)

# --- Standard and Compiler Options ---
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# --- Compiler Flags ---
if(MSVC)
  add_compile_options(/W4)
else()
  # GCC/Clang/MinGW specific flags - remove -flto to avoid linking issues
  add_compile_options(-Wall -Wextra -Os -ffunction-sections -fdata-sections)
endif()

# --- Windows Definitions ---
if(WIN32)
  add_compile_definitions(
    WINVER=0x0A00
    _WIN32_WINNT=0x0A00
    WIN32_LEAN_AND_MEAN
    NOMINMAX
  )
endif()

# --- DetourModKit Dependency ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/DetourModKit/CMakeLists.txt")
  message(STATUS "Configuring DetourModKit from: external/DetourModKit")
  add_subdirectory(external/DetourModKit)

  # Use the exact target name from DetourModKit
  if(TARGET DetourModKit)
    set(DETOURMODKIT_TARGET DetourModKit)
    message(STATUS "DetourModKit target found: ${DETOURMODKIT_TARGET}")
  else()
    message(FATAL_ERROR "DetourModKit target not created by subdirectory")
  endif()
else()
  message(FATAL_ERROR "DetourModKit not found at 'external/DetourModKit'. "
    "Please ensure the submodule is initialized: "
    "'git submodule update --init --recursive'")
endif()

# --- Source Files ---
set(COMMON_SOURCES
  src/mod_core.cpp
  src/version.cpp
)

set(ASI_SOURCES
  ${COMMON_SOURCES}
  src/dllmain.cpp
)

set(OBSE_SOURCES
  ${COMMON_SOURCES}
  src/dllmain_obse64.cpp
)

# --- Include Directories ---
set(COMMON_INCLUDE_DIRS
  src
  external/obse64

  # DetourModKit include path no longer needed after CMake fix
)

# --- ASI Target (.asi file) ---
add_library(${PROJECT_NAME}-ASI SHARED ${ASI_SOURCES})

target_include_directories(${PROJECT_NAME}-ASI PRIVATE ${COMMON_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}-ASI PRIVATE
  DetourModKit
  psapi
  user32
  kernel32
)

# Set output name and properties for ASI
set_target_properties(${PROJECT_NAME}-ASI PROPERTIES
  OUTPUT_NAME "OBR-NoCarryWeight-temp-asi"
  SUFFIX ".asi"
  PREFIX ""
)

# Linker flags for size optimization (MinGW)
if(NOT MSVC)
  target_link_options(${PROJECT_NAME}-ASI PRIVATE
    -Wl,--gc-sections
    -static-libgcc
    -static-libstdc++
  )
endif()

# --- OBSE Target (.dll file) ---
add_library(${PROJECT_NAME}-OBSE SHARED ${OBSE_SOURCES})

target_include_directories(${PROJECT_NAME}-OBSE PRIVATE ${COMMON_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}-OBSE PRIVATE
  DetourModKit
  psapi
  user32
  kernel32
)

# Set output name and properties for OBSE
set_target_properties(${PROJECT_NAME}-OBSE PROPERTIES
  OUTPUT_NAME "OBR-NoCarryWeight-temp-dll"
  SUFFIX ".dll"
  PREFIX ""
)

# Post-build steps to rename to desired names
add_custom_command(TARGET ${PROJECT_NAME}-ASI POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  $<TARGET_FILE:${PROJECT_NAME}-ASI>
  ${CMAKE_CURRENT_BINARY_DIR}/OBR-NoCarryWeight.asi
  COMMENT "Renaming ASI output to final name"
)

add_custom_command(TARGET ${PROJECT_NAME}-OBSE POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  $<TARGET_FILE:${PROJECT_NAME}-OBSE>
  ${CMAKE_CURRENT_BINARY_DIR}/OBR-NoCarryWeight.dll
  COMMENT "Renaming OBSE output to final name"
)

# Linker flags for size optimization (MinGW)
if(NOT MSVC)
  target_link_options(${PROJECT_NAME}-OBSE PRIVATE
    -Wl,--gc-sections
    -static-libgcc
    -static-libstdc++
  )
endif()

# --- Installation (Optional) ---
install(TARGETS ${PROJECT_NAME}-ASI ${PROJECT_NAME}-OBSE
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

# --- Build Summary ---
message(STATUS "---------------------------------------------------------------------")
message(STATUS "OBR-NoCarryWeight (${PROJECT_VERSION}) Configuration Summary:")
message(STATUS "  Generator:                   ${CMAKE_GENERATOR}")
message(STATUS "  Build Type:                  ${CMAKE_BUILD_TYPE}")
message(STATUS "  Targets:")
message(STATUS "    - ${PROJECT_NAME}-ASI      -> OBR-NoCarryWeight.asi (via post-build copy)")
message(STATUS "    - ${PROJECT_NAME}-OBSE     -> OBR-NoCarryWeight.dll (via post-build copy)")
message(STATUS "---------------------------------------------------------------------")
