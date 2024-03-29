cmake_minimum_required(VERSION 3.27)

project(NesEmu VERSION 1.0.0)

find_package(SFML 2.6.1 COMPONENTS graphics audio REQUIRED)
find_package(GTest CONFIG REQUIRED)

# define sources and headers
set(SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cpu.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cpu_addr.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cpu_ins.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/disk.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/disk_rw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/misc.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/neshdr.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ppu.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nes.cpp"
)
set(HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/const.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/cpu.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/disk.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/misc.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/neshdr.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/ppu.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/nes.hpp"
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

target_link_libraries(NesEmu PRIVATE
    sfml-graphics
    sfml-audio
)

target_include_directories(NesEmu PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_BINARY_DIR}/include" # Ensures config.h can be found
)

# Tests
enable_testing()
add_executable(NETest
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_cpu.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cpu.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cpu_addr.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cpu_ins.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/disk.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/disk_rw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/neshdr.cpp"
)
set_target_properties(NETest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
target_include_directories(NETest PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_BINARY_DIR}/include" # Ensures config.h can be found
)
target_link_libraries(NETest PRIVATE
    GTest::gtest
    GTest::gtest_main
)
add_test(NAME NETestSuite COMMAND NETest)
