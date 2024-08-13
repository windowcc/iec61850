# CMakeLists.txt
#
# Top-level CMake file for the iec61850 client.
#
#*******************************************************************************
# This is part of the Paho MQTT C++ client library.
#
# Copyright (c) 2021-2022, chao.xia
#
# 
# Contributors:
#   chao.xia
#*******************************************************************************/

## Note: on OS X you should install XCode and the associated command-line tools

## cmake flags
cmake_minimum_required(VERSION 3.10)

## --- TOOLCHAIN options ---
# if(DEFINED ENV{TOOLCHAIN})
#     set(CMAKE_C_COMPILER      $ENV{TOOLCHAIN}gcc)
#     set(CMAKE_CXX_COMPILER    $ENV{TOOLCHAIN}g++)
#     set(CMAKE_AR      "$ENV{TOOLCHAIN}ar" CACHE FILEPATH "CW archiver" FORCE)
# endif()

## project name
project("iec61850_client" 
    VERSION "1.0.0"
    LANGUAGES CXX
)


## --- Build options ---

option(IEC61850_BUILD_TESTS "Build tests" FALSE)
option(IEC61850_BUILD_DOCUMENTATION "Create and install the API documentation (requires Doxygen)" FALSE)

## --- C++11 build flags ---

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set(CMAKE_C_FLAGS "-O3 -g -ggdb -pthread")
# set(CMAKE_C_FLAGS_DEBUG "-O3 -g -Wall -ggdb -Werror -Wno-unused -pthread")
# set(CMAKE_CXX_FLAGS "-O3 -g -ggdb -pthread")
# set(CMAKE_CXX_FLAGS_DEBUG "-O3 -g -Wall -ggdb -Werror -Wno-unused -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -pthread")


# For the paho_mqtt_c module
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)


# --- Documentation ---

if(IEC61850_BUILD_DOCUMENTATION)
    add_subdirectory(doc)
endif()

# --- Unit Tests ---

if(IEC61850_BUILD_TESTS)
    add_subdirectory(test)
endif()

## --- build execute ---

add_subdirectory(src)
