cmake_minimum_required (VERSION 3.16)

if(APPLE)
  option(BUILD_UNIVERSAL "Build a Universal binary on macOS" ON)
  if(BUILD_UNIVERSAL)
    # Build a Universal binary on macOS
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
  else()
    # Build for the host architecture on macOS
    set(CMAKE_OSX_ARCHITECTURES "${CMAKE_HOST_SYSTEM_PROCESSOR}" CACHE STRING "" FORCE)
  endif()
endif()

project(GPT4All-cli)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 9)
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

set(CMAKE_EXPORT_COMPILE_COMMANDS "on")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")


# options
option(GPT4ALL_AVX_ONLY OFF "Build for avx only")



# sanitizers
set(LLAMA_BUILD_EXAMPLES OFF CACHE BOOL "llama: build examples" FORCE)
#set(BUILD_SHARED_LIBS ON FORCE)


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

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffast-math")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=native")

# dependencies

set(CMAKE_C_STANDARD   11)
set(CMAKE_CXX_STANDARD 11)

find_package(Threads REQUIRED)

# main

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo")
endif ()

# Generate a header file with the version number
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

# Include the binary directory for the generated header file
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_subdirectory(../gpt4all-backend llmodel) 

add_executable(chat ./src/chat.cpp ./src/header.h ./src/utils.h ./src/parse_json.h ../gpt4all-backend/llmodel_c.h)
target_link_libraries(chat PRIVATE llmodel)
