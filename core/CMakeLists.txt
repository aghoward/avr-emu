add_library(core STATIC
    clock.cc
    coremodule.cc
    executor.cc
    noopclock.cc
    loader.cc
)

target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(core PRIVATE cdif)
