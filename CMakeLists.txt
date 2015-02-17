CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

PROJECT (Refactorial)

SET (CMAKE_MODULE_PATH "${Refactorial_SOURCE_DIR}/CMake/Modules; ${CMAKE_MODULE_PATH}")


macro(llvm_get_config OUTPUT)
    execute_process(COMMAND ${LLVM_CONFIG_EXECUTABLE} ${ARGN}
        RESULT_VARIABLE __llvm_ok
        OUTPUT_VARIABLE ${OUTPUT})
    if (NOT __llvm_ok STREQUAL 0)
        message(FATAL_ERROR " -- llvm_get_config: failed to execute ${LLVM_CONFIG_EXECUTABLE} ${ARGN} (result: ${__llvm_ok})")
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

# kinda hardcoded for my setup...
find_program(LLVM_CONFIG_EXECUTABLE
    NAMES llvm-config
    PATHS "~/llvm.install/bin"
    )
# check that
if(${LLVM_CONFIG_EXECUTABLE} MATCHES NOTFOUND)
    message(FATAL_ERROR " -- could not find llvm-config")
else()
    llvm_get_config(LLVM_VERSION "--version")
    message("-- got llvm-config: '${LLVM_CONFIG_EXECUTABLE}', version ${LLVM_VERSION}")
endif(${LLVM_CONFIG_EXECUTABLE} MATCHES NOTFOUND)

# crate this list: make it empty, wait for "undefined reference" errors, note a
# symbol and call:
#   nm -C -A ~/llvm.install/lib/* | grep -e "[rRtT]\s*llvm::MCContext::~MCContext"
# note: don't link "clang" which is a shared lib. you'll get doubled symbols
list(APPEND CLANG_LIBRARIES
    clangTooling clangEdit clangDriver
    clangFrontend clangLex clangParse
    clangAnalysis clangSema clangRewrite
    clangSerialization clangEdit clangAST
    clangASTMatchers clangBasic
    )

llvm_get_config(LLVM_CXXFLAGS "--cxxflags")
llvm_get_config(LLVM_LIBRARY_DIRS "--libdir")
#llvm_get_config(LLVM_LIBRARIES "--libs" support mcparser)
llvm_get_config(LLVM_LIBRARIES "--libs")
string(REPLACE " " ";" LLVM_LIBRARIES "${LLVM_LIBRARIES}")

find_library(PCRE_LIBRARY pcre)
find_library(PCRECPP_LIBRARY pcrecpp)

find_package(YAMLCPP REQUIRED)



add_definitions(${LLVM_CXXFLAGS})
add_definitions("-fexceptions")

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${YAMLCPP_INCLUDE_DIR})
link_directories(${LLVM_LIBRARY_DIRS})

add_executable(refactorial
    main.cpp
    AugmentedJSONCompilationDatabase.cpp
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

target_link_libraries(refactorial
    ${CLANG_LIBRARIES}
    ${LLVM_LIBRARIES}
    ${PCRE_LIBRARY}
    ${PCRECPP_LIBRARY}
    ${YAMLCPP_LIBRARIES}
    dl pthread z curses
    )

# debug crazy system-header-indlude-problems in linked clang:
#    echo '#include <stdarg.h> \nint main(){return 0;}' | ~/llvm.install/bin/clang -xc -v -
# but this targets "clang", where the libclang might be different
#

add_custom_target(all-tests-refactorings)

# great power: can also be outside of this cmake's tree!
#set(TEST_DIRECTORY $ENV{HOME}/test_project)
set(TEST_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test2)

file(GLOB ALL_TEST_YAMLS "${TEST_DIRECTORY}/*.yml")

foreach(YAML_FILE ${ALL_TEST_YAMLS})

    get_filename_component(YAML_NAME ${YAML_FILE} NAME_WE)

    # create a local build-dir with compile-commands, create a local branch
    # (!), call the tool to make the changes, and create a commit
    add_custom_target(test-refactor-${YAML_NAME}
        COMMAND mkdir -p build && cd build && cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
        COMMAND git checkout -- .
        COMMAND git branch -f test-refactor-${YAML_NAME}
        COMMAND git checkout test-refactor-${YAML_NAME}
        COMMAND ${CMAKE_BINARY_DIR}/refactorial 
            -refactor-specification-file=${YAML_FILE}
            -p=${TEST_DIRECTORY}/build/compile_commands.json --
            -isystem /usr/include/x86_64-linux-gnu/c++/4.8
            -isystem /usr/lib/gcc/x86_64-linux-gnu/4.8/include
        COMMAND git add -u .
        COMMAND git commit -m "automatic commit: refactorial vs. ${YAML_NAME}"
        COMMAND make -C build
        COMMAND git checkout -
        WORKING_DIRECTORY ${TEST_DIRECTORY}
        DEPENDS refactorial
        )


    add_dependencies(all-tests-refactorings test-refactor-${YAML_NAME})

endforeach(YAML_FILE)
