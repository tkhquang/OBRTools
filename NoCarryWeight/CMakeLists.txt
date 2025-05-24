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
  # GCC/Clang/MinGW specific flags
  add_compile_options(-Wall -Wextra -Os -flto -ffunction-sections -fdata-sections)
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
  set(DETOURMODKIT_TARGET DetourModKit)
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
)

# --- ASI Target (.asi file) ---
add_library(${PROJECT_NAME}-ASI SHARED ${ASI_SOURCES})

target_include_directories(${PROJECT_NAME}-ASI PRIVATE ${COMMON_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}-ASI PRIVATE
  ${DETOURMODKIT_TARGET}
  psapi
  user32
  kernel32
)

# Set output name and properties for ASI
set_target_properties(${PROJECT_NAME}-ASI PROPERTIES
  OUTPUT_NAME "${PROJECT_NAME}"
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
  ${DETOURMODKIT_TARGET}
  psapi
  user32
  kernel32
)

# Set output name and properties for OBSE
set_target_properties(${PROJECT_NAME}-OBSE PROPERTIES
  OUTPUT_NAME "${PROJECT_NAME}"
  SUFFIX ".dll"
  PREFIX ""
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
message(STATUS "    - ${PROJECT_NAME}-ASI      -> ${PROJECT_NAME}.asi")
message(STATUS "    - ${PROJECT_NAME}-OBSE     -> ${PROJECT_NAME}.dll")
message(STATUS "---------------------------------------------------------------------")
