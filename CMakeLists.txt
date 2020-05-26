cmake_minimum_required(VERSION 3.5)

project(SPL-compiler)

set(CMAKE_CXX_STANDARD 17)

find_package(LLVM REQUIRED CONFIG)

link_directories(${LLVM_LIB_DIRS})
link_libraries(LLVM)

message(${LLVM_INCLUDE_DIRS})

add_subdirectory(parseTree)
add_subdirectory(sem)
add_subdirectory(gen)

# execute_process(COMMAND llvm-config --libs OUTPUT_VARIABLE llvm_libs)
# string(STRIP "${llvm_libs}" llvm_libs)
# llvm_map_components_to_libnames(llvm_libs support core irreader)

# execute_process(
#     COMMAND llvm-config --cxxflags --ldflags --system-libs --libs core support
#     OUTPUT_VARIABLE LLVM_FLAGS
# )

add_executable(${PROJECT_NAME}
    main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC parser sem gen
)