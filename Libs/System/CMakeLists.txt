set(NAME System)


set(CXX_SRCS
    Task.cpp
    SoftwareTimer.cpp
    Semaphore.cpp
)

set(INCLUDE_FILES
    Include/RTOSDefines.hpp
    Include/Semaphore.hpp
    Include/SoftwareTimer.hpp
    Include/System.hpp
    Include/Task.hpp
)

add_library(${NAME} STATIC
    ${CXX_SRCS} ${INCLUDE_FILES}
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
        RTOS_API
)
