CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

set(CMAKE_BUILD_TYPE None)
Project(testProject)

add_executable(${PROJECT_NAME}
    main.cpp
    ClassRename.cpp
    FunctionRename.cpp
    )
