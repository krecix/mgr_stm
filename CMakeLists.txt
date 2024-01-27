cmake_minimum_required(VERSION 3.12...3.15)


set(CMAKE_TOOLCHAIN_FILE ./arm_stm32_toolchain.cmake)


project(VibrationControlSTM32 VERSION 1.0 
                            DESCRIPTION "Active Vibration Reduction on STM32"
                            LANGUAGES ASM C CXX)
                            
                            
set(SEMIHOSTING false CACHE BOOL "Enable semihosting")

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)


set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


set(GLOBAL_DEFINES
    -DSTM32H7A3xxQ
)

add_definitions(${GLOBAL_DEFINES})

include(compiler_flags_native.cmake)

add_subdirectory(Config)
add_subdirectory(App)
add_subdirectory(Libs)
add_subdirectory(Platforms)





