mc_add_app(
  NAME
    cute
  SRCS
    "cute.cpp"
  DEPS
    ${OPENGL_LIBRARIES}
    ${GLEW_LIB}
    glfw
    glm::glm
    mc_lib
  INCLUDES
    ${OPENGL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${MC_LIB_INCLUDE_DIRS}
  COPTS
    ${MSVC_RUNTIME_LIBRARY_CONFIG}
  DEFINES
    ${GLFW_SYSTEM_DEFINES}
    ${GLEW_SYSTEM_DEFINES}
    ${SHADER_SOURCE_DIR_DEFINES}
)

install(
  TARGETS
    cute
  DESTINATION
    ${CMAKE_INSTALL_PREFIX}/demo/cute
)

# Install glsl file
install(
  DIRECTORY 
    ${PROJECT_SOURCE_DIR}/assets 
  DESTINATION
    ${CMAKE_INSTALL_PREFIX}
  FILES_MATCHING
    PATTERN "*.vs"
    PATTERN "*.fs"
)
