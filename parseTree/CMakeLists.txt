add_custom_command(
    OUTPUT lex.yy.c
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scanner.l
    COMMAND flex ${CMAKE_CURRENT_SOURCE_DIR}/scanner.l
)

add_custom_command(
    OUTPUT y.tab.c y.tab.h
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/parser.y
    COMMAND bison -d -o y.tab.c ${CMAKE_CURRENT_SOURCE_DIR}/parser.y
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c
)

add_library(parse_tree INTERFACE)

target_include_directories(parse_tree
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(frontend
    ${CMAKE_CURRENT_BINARY_DIR}/y.tab.c
)
target_link_libraries(frontend
    PUBLIC parse_tree
)
