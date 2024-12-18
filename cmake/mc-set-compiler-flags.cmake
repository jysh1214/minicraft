# Compiler flags
# Author: Alex Chiang
#
# MSVC
# Ref https://docs.microsoft.com/en-us/cpp/build/reference/compiler-option-warning-level?view=msvc-170

set(NO_SHARED_LIBS_FLAG)
set(STATIC_LINK_FLAG)
if(NOT BUILD_SHARED_LIBS)
  if(MSVC)
  else()
    set(NO_SHARED_LIBS_FLAG -disable-shared) # TODO: only for clang
    set(STATIC_LINK_FLAG -static -static-libstdc++)
  endif()
endif()

set(NO_EXCEPTION_FLAG)
if(NO_EXCEPTION)
  if(MSVC)
    set(NO_EXCEPTION_FLAG /EHsc)
  else() # Clang or GNU
    set(NO_EXCEPTION_FLAG -fno-exceptions)
  endif()
endif()

set(WARNING_AS_ERROR_FLAG)
if(WARNING_AS_ERROR)
  if(MSVC)
    set(WARNING_AS_ERROR_FLAG /WX)
  else() # Clang or GNU
    set(WARNING_AS_ERROR_FLAG -Werror)
  endif()
endif()
