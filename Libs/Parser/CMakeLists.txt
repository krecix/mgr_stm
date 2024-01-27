set(NAME Parser)

set(CXX_SRCS
    Src/Parser.cpp
)

set(INCLUDE_FILES
        Inc/Parser.hpp
        )

add_library(${NAME} STATIC
    ${CXX_SRCS} ${INCLUDE_FILES}
    ${COMMANDS}
)

target_include_directories(${NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

target_link_libraries(${NAME}
    PUBLIC
        Config
        etl
        Buffer
        Modules
)
