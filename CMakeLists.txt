CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

PROJECT (Refactorial)

add_definitions("-fno-rtti -Wno-c++11-extensions -Wno-nested-anon-types")

macro(llvm_get_config OUTPUT)
    execute_process(COMMAND ${LLVM_CONFIG_EXECUTABLE} ${ARGN}
        RESULT_VARIABLE __llvm_ok
        OUTPUT_VARIABLE ${OUTPUT})
    if (NOT __llvm_ok STREQUAL 0)
        message(FATAL_ERROR "llvm_get_config: failed to execute ${LLVM_CONFIG_EXECUTABLE} ${ARGN} (result: ${__llvm_ok})")
    endif()
    string(STRIP ${OUTPUT} "${${OUTPUT}}")
    string(REPLACE "\n" "" ${OUTPUT} "${${OUTPUT}}")
endmacro()

macro(resolve_library_flags VARIABLE)
    foreach(${VARIABLE}_lib ${${VARIABLE}_LIBRARIES})
      set(_${VARIABLE}_lib NOTFOUND)
      string(REPLACE "^-l" "" ${VARIABLE}_lib ${${VARIABLE}_lib})
      find_library(_${VARIABLE}_lib NAMES ${${VARIABLE}_lib} HINTS ${${VARIABLE}_LIBRARY_DIRS})
      if (NOT _${VARIABLE}_lib)
        set(_${VARIABLE}_lib ${${VARIABLE}_lib})
      endif()
      list(APPEND _${VARIABLE}_LIBRARIES ${_${VARIABLE}_lib})
    endforeach()
    list(APPEND _${VARIABLE}_LIBRARIES ${${VARIABLE}_LDFLAGS_OTHER})
    set(${VARIABLE}_LIBRARIES ${_${VARIABLE}_LIBRARIES} CACHE INTERNAL "")
endmacro()

find_program(LLVM_CONFIG_EXECUTABLE NAMES llvm-config PATHS "~/llvm.install/bin")
message(" -- got llvm-config: ${LLVM_CONFIG_EXECUTABLE}")

# crate this list: make it empty, wait for "undefined reference" errors, note a symbol and call:
#   nm -C -A ~/llvm.install/lib/* | grep -e "[rRtT]\s*llvm::MCContext::~MCContext"
list(APPEND CLANG_LIBRARIES
    clangTooling clangEdit clangDriver
    clangFrontend clangLex clangParse
    clangAnalysis clangSema clangRewriteCore
    clangSerialization clangEdit clangAST
    clangASTMatchers clangBasic
    )

llvm_get_config(LLVM_CXXFLAGS "--cxxflags")
llvm_get_config(LLVM_LIBRARY_DIRS "--libdir")
llvm_get_config(LLVM_LIBRARIES "--libs" support mcparser)
string(REPLACE " " ";" LLVM_LIBRARIES "${LLVM_LIBRARIES}")

FIND_LIBRARY(PCRE_LIBRARY pcre)
FIND_LIBRARY(PCRECPP_LIBRARY pcrecpp)

ADD_DEFINITIONS(${LLVM_CXXFLAGS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
LINK_DIRECTORIES(${LLVM_LIBRARY_DIRS})

ADD_EXECUTABLE(refactorial
    main.cpp
    Transforms/AccessorsTransform.cpp
    Transforms/ExtractParameterTransform.cpp
    Transforms/FunctionRenameTransform.cpp
    Transforms/IdentityTransform.cpp
    Transforms/MethodMoveTransform.cpp
    Transforms/RecordFieldRenameTransform.cpp
    Transforms/RenameTransforms.h
    Transforms/Transforms.cpp
    Transforms/Transforms.h
    Transforms/TypeRenameTransform.cpp
)

TARGET_LINK_LIBRARIES(refactorial
    ${CLANG_LIBRARIES}
    ${LLVM_LIBRARIES}
    ${PCRE_LIBRARY}
    ${PCRECPP_LIBRARY}
    dl pthread
    yaml-cpp
    )
