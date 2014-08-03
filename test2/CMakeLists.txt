CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

set(CMAKE_BUILD_TYPE None)
Project(testProject)

add_definitions("-isystem /usr/include/x86_64-linux-gnu/c++/4.8")
add_definitions("-isystem /usr/lib/gcc/x86_64-linux-gnu/4.8/include")

add_executable(${PROJECT_NAME}
    main.cpp
    ClassRename.cpp
    )
