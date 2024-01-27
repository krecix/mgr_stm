set(NAME Drivers)

set(CXX_SRCS
    Src/STM32H7A3_Engine.cpp
    Src/STM32H7A3_GPIO.cpp
    Src/STM32H7A3_Positioner.cpp
    Src/STM32H7A3_UART.cpp
	Src/STM32H7A3_ElectricCoils.cpp
	Src/STM32H7A3_ADC.cpp
)

set(INCLUDE_FILES
    Inc/STM32H7A3_ADC.hpp
    Inc/STM32H7A3_ElectricCoils.hpp
    Inc/STM32H7A3_Engine.hpp
    Inc/STM32H7A3_GPIO.hpp
    Inc/STM32H7A3_Positioner.hpp
    Inc/STM32H7A3_UART.hpp
    )

add_library(${NAME} STATIC
    ${CXX_SRCS} ${INCLUDE_FILES}
)

target_include_directories(${NAME}
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

target_link_libraries(${NAME}
    PUBLIC
		Platform
        Drivers_Interface
)
