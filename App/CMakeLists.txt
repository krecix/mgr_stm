set(NAME VibrationControlSTM32)


set(INCLUDE_FILES
    Include/CCUHardware.hpp
)
 
set(SOURCES
    	main.cpp
        pid.cpp
        CCUHardware.cpp
)



include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
)


# generate elf file
add_arm_executable(${NAME} ${SOURCES} ${INCLUDE_FILES})


arm_link_libraries(${NAME}
    Modules
    System
    Drivers
	Parser
)

