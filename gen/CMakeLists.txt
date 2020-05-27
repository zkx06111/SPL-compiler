add_library(gen
    GeneratorContext.cpp
    FunctionContext.cpp
    Operations.cpp
    SysFunc.cpp
    TypeContext.cpp
    ValueContext.cpp
    gen.cpp
)

target_include_directories(gen
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${LLVM_INCLUDE_DIRS}
)

target_compile_definitions(gen
    PUBLIC ${LLVM_DEFINITIONS}
)

llvm_map_components_to_libnames(LLVM_LIBS support core irreader)

target_link_libraries(gen
    PUBLIC sem parser ${LLVM_LIBS}
)

set_property(TARGET gen PROPERTY CXX_STANDARD 17)