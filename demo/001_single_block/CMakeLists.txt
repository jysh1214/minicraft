mc_add_app(
  NAME
    single_block
  SRCS
    "single_block.cpp"
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
  DEFINES
    ${GLFW_SYSTEM_DEFINES}
    ${GLEW_SYSTEM_DEFINES}
    ${SHADER_SOURCE_DIR_DEFINES}
)

install(
  TARGETS
    single_block
  DESTINATION
    ${CMAKE_INSTALL_PREFIX}/demo/001_single_block
)

# Install glsl file
install(
  DIRECTORY 
    ${PROJECT_SOURCE_DIR}/shader 
  DESTINATION
    ${CMAKE_INSTALL_PREFIX}
  FILES_MATCHING
    PATTERN "*.glsl"
)
