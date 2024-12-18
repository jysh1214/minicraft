# minicraft
#
# Author: Alex Chiang
#
set(CMAKE_MIN_VERSION 3.21)
cmake_minimum_required(VERSION ${CMAKE_MIN_VERSION})
message(STATUS "Using CMake version ${CMAKE_VERSION}")

project(minicraft C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(DEFAULT_CMAKE_BUILD_TYPE Release)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)

set(LIBRARY_DIR ${PROJECT_SOURCE_DIR}/library)

# Options
option(BUILD_SHARED_LIBS "Build & link with dynamic library." OFF)
option(BUILD_TEST "Enable unit tests." ON)
option(BUILD_BENCHMARK "Enable benchmark." ON)
option(BUILD_TOOLS "Build extra tools." ON)
option(ENABLE_EXCEPTION "" OFF)
option(WARNING_AS_ERROR "" ON)

set(MC_PACKAGE_ROOT_PREFIX "minicraft")
set(MC_PACKAGE_ROOT_DIR "")

set(PYTHON_EXEC "python" CACHE STRING "Python excutable")

if(CMAKE_TOOLCHAIN_FILE)
  message(STATUS "Using CMake toolchain: ${CMAKE_TOOLCHAIN_FILE}")
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

include(mc-add-app)
include(mc-add-lib)
# include(mc-set-compiler-flags)

find_package(OpenGL REQUIRED)
find_package(glew REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)
# Find OpenCV.
if(MSVC)
  # set(OpenCV_STATIC ON)
  find_package(OpenCV REQUIRED)
  # set_target_properties(${OpenCV_LIBS} PROPERTIES MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE)


  # We build OpeCV with Release mode -> runtime library -> /MT
  # if we build minicraft as Debug mode -> runtime library -> /MTd
  # 

  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
else()
  find_package(OpenCV REQUIRED)
endif()

if(BUILD_SHARED_LIBS)
  set(GLEW_LIB GLEW::glew)
else()
  set(GLEW_LIB GLEW::glew_s)
endif()

# To build the DLL version of GLFW on Windows
set(GLFW_SYSTEM_DEFINES)
if(BUILD_SHARED_LIBS AND MSVC)
  list(APPEND GLFW_SYSTEM_DEFINES GLFW_DLL)
endif()

set(MC_CONFIG_DIRS ${PROJECT_SOURCE_DIR}/config)
set(MC_LIB_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/lib)
set(MC_LIB_DEBUG_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/lib/debug)
set(MC_TOOLS_DIRS ${PROJECT_SOURCE_DIR}/tools)

# Set shaders path
set(SHADER_SOURCE_DIR ${CMAKE_INSTALL_PREFIX}/assets)
set(SHADER_SOURCE_DIR_DEFINES -D SHADER_SOURCE_DIR="${SHADER_SOURCE_DIR}")

set(MC_TEXTURE_DIR ${CMAKE_INSTALL_PREFIX}/assets)
set(MC_TEXTURE_DIR_DEFINES -D MC_TEXTURE_DIR="${MC_TEXTURE_DIR}")

if(MSVC)
  set(MSVC_RUNTIME_LIBRARY_CONFIG
    "$<$<CONFIG:Debug>:/MTd>"
    "$<$<CONFIG:RelWithDebInfo>:/MT>"
    "$<$<CONFIG:Release>:/MT>"
    "$<$<CONFIG:MinSizeRel>:/MT>"
  )
endif()

set(OPENCV_STATIC_DEFINES -D OPENCV_STATIC)

if(BUILD_TEST)
  message(STATUS "Enable exception for testing.")
  set(ENABLE_EXCEPTION ON)
endif()

if(ENABLE_EXCEPTION)
  set(ENABLE_EXCEPTION_DEFINES -D ENABLE_EXCEPTION=true)
endif()

add_subdirectory(lib)
# add_subdirectory(app)

add_subdirectory(demo)

# if(BUILD_TEST)
#   add_subdirectory(test)
# endif()

if(BUILD_TOOLS)
  add_subdirectory(tools/visualization)
endif()

# Code gen

# Gen block.h
set(BLOCK_TYPE_FILE ${MC_CONFIG_DIRS}/block_type.json)
set(BLOCK_PROPERTY_FILE ${MC_CONFIG_DIRS}/block_property.json)
set(BLOCK_HEADER ${MC_LIB_INCLUDE_DIRS}/block.h)
set(BLOCK_SOURCE ${MC_LIB_INCLUDE_DIRS}/block.cpp)
add_custom_target(GenBlockHeaderFile ALL
  COMMAND 
    ${PYTHON_EXEC}
    ${MC_TOOLS_DIRS}/codegen/gen_block.py 
    ${BLOCK_TYPE_FILE}
    ${BLOCK_PROPERTY_FILE}
    ${BLOCK_HEADER}
    ${BLOCK_SOURCE}
  DEPENDS 
    ${BLOCK_TYPE_FILE}
    ${BLOCK_PROPERTY_FILE}
  COMMENT "Generating `block.h`"
)
