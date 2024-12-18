include(CMakeParseArguments)


function(mc_add_lib)
  cmake_parse_arguments(
    RULE
    ""
    "NAME"
    "SRCS;DEPS;INCLUDES;COPTS;DEFINES;LINKOPTS"
    ${ARGN}
  )

  add_library(${RULE_NAME} "")
  if(RULE_SRCS)
    target_sources(${RULE_NAME}
      PRIVATE
        ${RULE_SRCS}
    )
  else()
    message(STATUS "[WARNING] mc_add_app(${RULE_NAME}): No source file.")
  endif()

  target_include_directories(${RULE_NAME}
    SYSTEM PUBLIC
      "$<BUILD_INTERFACE:${MC_SOURCE_DIR}>"
      "$<BUILD_INTERFACE:${MC_BINARY_DIR}>"
  )

  target_include_directories(${RULE_NAME}
    SYSTEM PUBLIC
      ${RULE_INCLUDES}
  )

  target_compile_definitions(${RULE_NAME}
    PUBLIC
      ${RULE_DEFINES}
  )

  target_compile_options(${RULE_NAME}
    PRIVATE
      ${RULE_COPTS}
  )

  target_link_options(${RULE_NAME}
    PRIVATE
      ${RULE_LINKOPTS}
  )

  target_link_libraries(${RULE_NAME}
    PRIVATE
      ${RULE_DEPS}
  )

endfunction()
