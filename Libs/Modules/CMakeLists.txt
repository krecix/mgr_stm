set(NAME Modules)

set(CXX_SRCS
    Src/BinaryCommunicator.cpp
    Src/VibrationController.cpp
    Src/ModuleController.cpp
)

set(INCLUDE_FILES
    Inc/BinaryCommunicator.hpp
    Inc/ModuleController.hpp
    Inc/Pattern.hpp
    Inc/PIDController.hpp
    Inc/VibrationController.hpp
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
		Drivers_Interface
		Config
		System
		Buffer
	PRIVATE
		etl
		Parser
		
)
