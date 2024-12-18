mc_add_app(
  NAME
    world
  SRCS
    "world.cpp"
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
    world
  DESTINATION
    ${CMAKE_INSTALL_PREFIX}/demo/world
)

# Install glsl file
install(
  DIRECTORY 
    ${PROJECT_SOURCE_DIR}/resources 
  DESTINATION
    ${CMAKE_INSTALL_PREFIX}
  FILES_MATCHING
    PATTERN "*.vs"
    PATTERN "*.fs"
)
