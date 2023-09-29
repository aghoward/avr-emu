cmake_minimum_required(VERSION 3.12)

project(AvrEmulator)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_executable(avr-emu main.cc)

add_subdirectory(cdif)
add_subdirectory(core)
add_subdirectory(instructions)
add_subdirectory(tests)

target_include_directories(avr-emu PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/instructions
)

target_link_libraries(avr-emu PUBLIC cdif core instructions)
target_compile_features(avr-emu PUBLIC cxx_std_20)
target_compile_options(avr-emu PUBLIC 
    -Wall
    -Wextra
    -Wshadow
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
    -pedantic
    -Wconversion
    -Wsign-conversion
    -Wmisleading-indentation
)

