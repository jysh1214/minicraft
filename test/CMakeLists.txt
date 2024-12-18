cmake_minimum_required(VERSION ${CMAKE_MIN_VERSION})

include(FetchContent)

find_package(Gtest QUIET)
# Fetch google test source code from official repository
if(NOT Gtest_FOUND)
  set(GTEST_REPOSITORY https://github.com/google/googletest.git)
  set(GTEST_TAG "release-1.10.0")

  FetchContent_Declare(googletest
    GIT_REPOSITORY ${GTEST_REPOSITORY}
    GIT_TAG ${GTEST_TAG}
  )

  FetchContent_GetProperties(googletest)
  if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
  endif()
endif()

mc_add_app(
  NAME
    mc_test
  SRCS
    "block_test.cpp"
    "chunk_test.cpp"
    "mc_test.cpp"
  DEPS
    ${OPENGL_LIBRARIES}
    ${GLEW_LIB}
    glfw
    glm::glm
    mc_lib
    gtest
  INCLUDES
    ${OPENGL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${MC_LIB_INCLUDE_DIRS}
    ${MC_LIB_DEBUG_INCLUDE_DIRS}
  DEFINES
    ${GLFW_SYSTEM_DEFINES}
    ${GLEW_SYSTEM_DEFINES}
    ${SHADER_SOURCE_DIR_DEFINES}
    ${ENABLE_EXCEPTION_DEFINES}
)
