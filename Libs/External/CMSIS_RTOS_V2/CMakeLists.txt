set(NAME RTOS_API)


set(C_SRCS
    cmsis_os2.c
)


add_library(${NAME} STATIC
    ${C_SRCS}
)

target_include_directories(${NAME}
	PUBLIC
		$<INSTALL_INTERFACE:Include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_link_libraries(${NAME}
    PUBLIC
        freeRTOS
)



