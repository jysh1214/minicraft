
set(SHADER_SOURCE_DIR ${CMAKE_INSTALL_PREFIX}/shader)
set(SHADER_SOURCE_DIR_DEFINES -D SHADER_SOURCE_DIR="${SHADER_SOURCE_DIR}")

mc_add_app(
  NAME
    minicraft
  SRCS
    "minicraft.cpp"
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
    minicraft
  DESTINATION
    ${CMAKE_INSTALL_PREFIX}/bin
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
