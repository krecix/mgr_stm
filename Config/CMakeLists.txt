set(NAME Config)

set(INCLUDE_FILES etl_profile.h Config.hpp)

add_library(${NAME} INTERFACE ${INCLUDE_FILES})

target_include_directories(${NAME} INTERFACE 
    $<INSTALL_INTERFACE:.>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
