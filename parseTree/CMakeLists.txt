add_custom_command(
    OUTPUT lex.yy.c
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scanner.l
    COMMAND flex ${CMAKE_CURRENT_SOURCE_DIR}/scanner.l
)

add_custom_command(
    OUTPUT y.tab.cpp y.tab.hpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/parser.y
    COMMAND bison -d -o y.tab.cpp ${CMAKE_CURRENT_SOURCE_DIR}/parser.y
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c
)

add_library(parser
    ${CMAKE_CURRENT_BINARY_DIR}/y.tab.cpp
)
target_include_directories(parser
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
)

set_property(TARGET parser PROPERTY CXX_STANDARD 11)