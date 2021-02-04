cmake_minimum_required(VERSION 3.10)

project(PMFBT)

########################################################################################################################
# System properties
########################################################################################################################

# If not set, determines the running platform architecture.
if(NOT PLATFORM)
    if(CMAKE_SIZEOF_VOID_P MATCHES 8)
        set(PLATFORM 64)
    else()
        set(PLATFORM 32)
    endif()
endif()
message(STATUS "Compilation set for ${PLATFORM}bits architectures.")

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_definitions(-DLINUX -DPOSIX)
    set(ARCH_TARGET linux64)

    if(${PLATFORM} MATCHES 32)
        message(WARNING "OpenVR x86 binaries not provided on GNU/Linux.")
    endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_MACOSX_RPATH 0)
    add_definitions(-DOSX -DPOSIX)
    set(ARCH_TARGET osx32)

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(SDL_REQUIRED_LIBRARIES ${SDL_REQUIRED_LIBRARIES} SDL2main)
    add_definitions(-D_WIN32)
    set(ARCH_TARGET win${PLATFORM})

    # Binaries path for thirdparties are not generics so we try to guess their suffixes.
    set(WINDOWS_PATH_SUFFIXES win${PLATFORM} Win${PLATFORM} x${PLATFORM})

    if(${PLATFORM} MATCHES 64)
        message(WARNING "SDL x64 runtime binaries not provided on Windows.")
    endif()
endif()

########################################################################################################################
# Paths
########################################################################################################################

# Check that the steamVR SDK is installed
# (needed to prevent a segfault in OpenVR).
if(CMAKE_HOST_UNIX)
    find_file(OPENVRPATHS openvrpaths.vrpath PATHS $ENV{HOME}/.config/openvr "$ENV{HOME}/Library/Application Support/OpenVR/.openvr")
    if(${OPENVRPATHS} MATCHES OPENVRPATHS-NOTFOUND)
        message(FATAL_ERROR "${OPENVRPATHS} Please install SteamVR SDK to continue..")
    endif()
endif()

########################################################################################################################
# Compiler Detection
########################################################################################################################

if((${CMAKE_CXX_COMPILER_ID} MATCHES "GNU") OR (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang"))
    # Better to use the prebuilt GNU preprocessor define __GNUC__,
    # kept for legacy reason with the sample code.
    add_definitions(-DGNUC)

    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -std=c++11 -include ${SHARED_SRC_DIR}/compat.h")
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -pedantic -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

    # Handles x86 compilation support on x64 arch.
    if(${PLATFORM} MATCHES 32)
        set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS} -m32")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} /W2 /DEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MP /INCREMENTAL:NO")
else()
    message(FATAL_ERROR "Unsupported compiler '${CMAKE_CXX_COMPILER_ID}'")
endif()

########################################################################################################################
# Libraries
########################################################################################################################

#
# OpenCV
#
set(OpenCV_STATIC ON)
find_package(OpenCV REQUIRED PATHS "deps/openvr")

#
# HyperPose
#
set(BUILD_CLI NO)
set(BUILD_EXAMPLES NO)
set(BUILD_USER_CODES NO)
set(BUILD_TESTS NO)
add_subdirectory("deps/hyperpose")
set(HYPERPOSE_INCLUDE_DIRS deps/hyperpose/include)
set(HYPERPOSE_LIBS hyperpose)

#
# OpenVR
#
find_library(OPENVR_LIBRARIES
    NAMES
        openvr_api
    PATHS
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/openvr/bin
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/openvr/lib
    PATH_SUFFIXES
        osx32
        linux64
        ${WINDOWS_PATH_SUFFIXES}
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH
)
set(OPENVR_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/openvr/headers)

########################################################################################################################
# Sources
########################################################################################################################

set(CMAKE_CXX_STANDARD 17)

#
# Include source files
#
file(GLOB_RECURSE SOURCE_FILES
    ./src/*.cpp
    ./src/*.hpp
)

include_directories(
    src/
    ${HYPERPOSE_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${OPENVR_INCLUDE_DIR}
)

add_library(PMFBT SHARED
    ${SOURCE_FILES}
)

target_link_libraries(PMFBT
    ${HYPERPOSE_LIBS}
    ${OpenCV_LIBS}
    ${OPENVR_LIBRARIES}
)
