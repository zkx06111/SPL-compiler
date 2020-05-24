cmake_minimum_required(VERSION 3.5)

project(SQL-compiler)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(parseTree)
add_subdirectory(sem)