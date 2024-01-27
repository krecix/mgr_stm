set(NAME Drivers_Interface)

set(INCLUDE_FILES
    AbstractADC.hpp
    AbstractElectricCoils.hpp
    AbstractEngine.hpp
    AbstractGPIO.hpp
    AbstractPositioner.hpp
    AbstractUART.hpp
    )

add_library(${NAME} INTERFACE ${INCLUDE_FILES})

target_include_directories(${NAME} INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/.
)

target_link_libraries(${NAME}
    INTERFACE
        Config
        etl
)
