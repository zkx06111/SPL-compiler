add_library(sem
    SymbolTable.cpp
    Type.cpp
    TypeUtil.cpp
    Func.cpp
    IdTable.cpp
    TypeTable.cpp
    VarTable.cpp
    ConstTable.cpp
    FuncTable.cpp
    LabelTable.cpp
    SemError.cpp
    sem.cpp
)

target_include_directories(sem
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(sem
    PUBLIC parser
)

set_property(TARGET sem PROPERTY CXX_STANDARD 17)