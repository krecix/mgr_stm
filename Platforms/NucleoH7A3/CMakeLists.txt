set(NAME Platform)

set(SOURCES
    startup_stm32h7a3xx.s
    system_stm32h7xx.c
    tcm.c
    syscalls.c
    sysmem.c
)

set(INCLUDE_FILES
    Include/cachel1_armv7.h
    Include/cmsis_compiler.h
    Include/cmsis_gcc.h
    Include/cmsis_version.h
    Include/core_cm7.h
    Include/mpu_armv7.h
    Include/stm32h7a3xxq.h
    Include/stm32h7xx.h
    Include/system_stm32h7xx.h)

add_library(${NAME} STATIC ${SOURCES} ${INCLUDE_FILES})

target_include_directories(${NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Include)
target_include_directories(${NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Include)
