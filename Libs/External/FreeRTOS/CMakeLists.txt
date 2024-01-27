set(NAME freeRTOS)


set(INCLUDE_DIRS
    include
)

set(SRCS_DIR
    src
)

set(C_SRCS
    ${SRCS_DIR}/croutine.c
    ${SRCS_DIR}/event_groups.c
    ${SRCS_DIR}/FreeRTOS_openocd.c
    ${SRCS_DIR}/list.c
    ${SRCS_DIR}/port.c
    ${SRCS_DIR}/queue.c
    ${SRCS_DIR}/stream_buffer.c
    ${SRCS_DIR}/tasks.c
    ${SRCS_DIR}/timers.c

    #Heap manage
    ${SRCS_DIR}/MemMang/heap_4.c
)


add_library(${NAME} STATIC
    ${C_SRCS}
)

target_include_directories(${NAME}
	PUBLIC
		$<INSTALL_INTERFACE:${INCLUDE_DIRS}>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_DIRS}>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${NAME}
    PUBLIC
        Platform
)



