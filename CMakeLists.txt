cmake_minimum_required (VERSION 3.2)

if(APPLE)
  option(OLD_MACOS       "Using old macos"                   OFF) 
  option(BUILD_UNIVERSAL "Build a Universal binary on macOS" ON)
  if(BUILD_UNIVERSAL AND NOT OLD_MACOS)
    # Build a Universal binary on macOS
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
  else()
    # Build for the host architecture on macOS
    set(CMAKE_OSX_ARCHITECTURES "${CMAKE_HOST_SYSTEM_PROCESSOR}" CACHE STRING "" FORCE)
  endif()
  if (OLD_MACOS)
    add_definitions(-DOLD_MACOS)
  endif()
endif()

project(LlamaGPTJ-chat)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 3)
set(VERSION_PATCH 0)
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

set(CMAKE_EXPORT_COMPILE_COMMANDS "on")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")


if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(LLAMA_STANDALONE ON)
else()
    set(LLAMA_STANDALONE OFF)
endif()


# options
option(AVX2                         "enable AVX2"                                           ON)
option(AVX512                       "enable AVX512"                                         OFF)

option(LLAMA_AVX                    "llama: enable AVX"                                     ON)
option(LLAMA_AVX2                   "llama: enable AVX2"                                    ${AVX2})
option(LLAMA_AVX512                 "llama: enable AVX512"                                  ${AVX512})
option(LLAMA_AVX512_VBMI            "llama: enable AVX512-VBMI"                             ${AVX512})
option(LLAMA_AVX512_VNNI            "llama: enable AVX512-VNNI"                             ${AVX512})
option(LLAMA_FMA                    "llama: enable FMA"                                     ${AVX2})


# sanitizers
#set(LLAMA_BUILD_EXAMPLES ON CACHE BOOL "llama: build examples" FORCE)
if(APPLE)
elseif(UNIX)
    if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
    option(AVX2                         "enable AVX2"                                           OFF)
    option(LLAMA_AVX                    "llama: enable AVX"                                     OFF)
    option(LLAMA_AVX2                   "llama: enable AVX2"                                    OFF)
    option(LLAMA_AVX512                 "llama: enable AVX512"                                  OFF)
    option(LLAMA_AVX512_VBMI            "llama: enable AVX512-VBMI"                             OFF)
    option(LLAMA_AVX512_VNNI            "llama: enable AVX512-VNNI"                             OFF)
    set(BUILD_SHARED_LIBS ON FORCE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mno-outline-atomics")
    endif()
endif()

if (GGML_SANITIZE_THREAD)
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -fsanitize=thread")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
endif()

if (GGML_SANITIZE_ADDRESS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}     -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
endif()

if (GGML_SANITIZE_UNDEFINED)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}     -fsanitize=undefined")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
endif()
if (AVX512)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}     -mavx512vl")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx512vl")
endif()

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffast-math")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=native")

# dependencies

set(CMAKE_C_STANDARD   17)
set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)

# main

# Include static libs for compatibility:
if(APPLE)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-search_paths_first -lSystem")
elseif(UNIX)
    if(NOT ${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
    endif()
elseif(WIN32)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
endif()

# Generate a header file with the version number
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

# Include the binary directory for the generated header file
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_subdirectory(gpt4all-backend/llama.cpp)
add_subdirectory(gpt4all-backend)
add_subdirectory(src)
