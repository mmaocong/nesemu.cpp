cmake_minimum_required(VERSION 3.27)

project(NesEmu VERSION 1.0.0)

# define sources and headers
set(SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cassette.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cpu.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cpu_ins.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/disk.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/misc.cpp"
)
set(HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/cassette.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/const.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/cpu.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/disk.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/misc.hpp"
)

# Create the executable
add_executable(NesEmu
    ${SOURCES}
    ${HEADERS}
)

# Set the runtime output directory to be inside the build directory
set_target_properties(NesEmu PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Configure the file into the build directory
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in"
    "${CMAKE_BINARY_DIR}/include/config.h"
)

target_include_directories(NesEmu PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_BINARY_DIR}/include" # Ensures config.h can be found
)
