cmake_minimum_required(VERSION 3.10)

project(dasputer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(hdl)

enable_testing()
include(Dart)
include(GoogleTest)
add_subdirectory(googletest)

find_package(PkgConfig)
pkg_check_modules(SDL2 REQUIRED sdl2)

hdl_sim(bounce
    SOURCES
        top_bounce.sv
        simple_480p.sv
    SIM_SOURCES
        main.cpp
)

target_include_directories(bounce PUBLIC ${SDL2_INCLUDE_DIRS})
target_link_directories(bounce PUBLIC ${SDL2_LIBRARY_DIRS})
target_link_libraries(bounce ${SDL2_LIBRARIES})

hdl_test(bounce_test
    SOURCES
        top_bounce.sv
        simple_480p.sv
    TEST_SOURCES
        test/bounce_testcase.cpp
        test/test.cpp
)