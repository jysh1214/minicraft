# # Find OpenCV.
# if(MSVC)
#   # set(OpenCV_STATIC OFF)
#   # find_package(OpenCV REQUIRED world)
#   # set_target_properties(${OpenCV_LIBS} PROPERTIES MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE)
# else()
#   find_package(OpenCV REQUIRED)
# endif()

mc_add_app(
  NAME
    visualization
  SRCS
    "visualization.cpp"
  DEPS
    ${OPENGL_LIBRARIES}
    ${GLEW_LIB}
    glfw
    glm::glm
    ${OpenCV_LIBS}
    mc_lib
  INCLUDES
    ${OPENGL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${MC_LIB_INCLUDE_DIRS}
  COPTS
    ${MSVC_RUNTIME_LIBRARY_CONFIG}
  DEFINES
    ${GLFW_SYSTEM_DEFINES}
    ${GLEW_SYSTEM_DEFINES}
    ${OPENCV_STATIC_DEFINES}
)

install(
  TARGETS
    visualization
  DESTINATION
    ${CMAKE_INSTALL_PREFIX}/tools
)
