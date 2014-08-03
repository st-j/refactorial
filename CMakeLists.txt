CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

PROJECT (Refactorial)

# debian packet dependencies:
#
# sudo apt-get install libyaml-cpp-dev clang
#
# also need llvm+clang in version 3.2:
# {{{
#     export BRANCH=release_32
#     git clone http://llvm.org/git/llvm.git $HOME/llvm.git --branch $BRANCH
#     git clone http://llvm.org/git/clang.git $HOME/llvm.git/tools/clang --branch $BRANCH
#     mkdir -p $HOME/llvm.git/build && cd $HOME/llvm.git/build
#     cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=true -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$HOME/llvm.install -DLLVM_TARGETS_TO_BUILD="X86"
#     make -j`getconf _NPROCESSORS_ONLN` all install
# }}}
#
# compiling with gcc 4.8 seems to break, clang 3.3 works. do the following in your shell:
# {{{
#     export CC=clang
#     export CXX=clang++
#     mkdir build && cd build
#     cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
#     make
# }}}

# kinda hardcoded by trial-and-error...
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

# kinda hardcoded for my case...
find_program(LLVM_CONFIG_EXECUTABLE
    NAMES llvm-config
    PATHS "~/llvm.install/bin"
    )
if(${LLVM_CONFIG_EXECUTABLE} MATCHES NOTFOUND)
    message(FATAL_ERROR " -- could not find llvm-config")
else()
    llvm_get_config(LLVM_VERSION "--version")
    message(" -- got llvm-config: '${LLVM_CONFIG_EXECUTABLE}', version ${LLVM_VERSION}")
endif(${LLVM_CONFIG_EXECUTABLE} MATCHES NOTFOUND)

# crate this list: make it empty, wait for "undefined reference" errors, note a
# symbol and call:
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
    dl pthread yaml-cpp
    )
