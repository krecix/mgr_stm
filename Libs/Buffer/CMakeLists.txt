set(NAME Buffer)

set(INCLUDE_FILES Buffer.hpp)

add_library(${NAME} INTERFACE ${INCLUDE_FILES})

target_include_directories(${NAME} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/.
)

target_link_libraries(${NAME}
    INTERFACE
        Config
)
