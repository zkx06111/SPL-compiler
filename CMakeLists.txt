cmake_minimum_required(VERSION 3.5)

project(SPL-compiler)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(parseTree)
add_subdirectory(sem)

add_executable(${PROJECT_NAME}
    main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC parser sem
)