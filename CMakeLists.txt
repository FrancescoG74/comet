cmake_minimum_required(VERSION 3.16)
project(comet VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)
find_package(Catch2 REQUIRED)

enable_testing()

add_executable(comet
    main.cpp
    headers/planetcontrolwidget.h
    headers/solarsystem.h
    headers/solarsystemcontroller.h
    headers/astronomicalbody.h
    headers/sprite.h
    sources/astronomicalbody.cpp
    sources/solarsystem.cpp
    sources/solarsystemcontroller.cpp
    sources/planetcontrolwidget.cpp
    sources/sprite.cpp
)

target_link_libraries(comet PRIVATE Qt6::Core Qt6::Widgets Qt6::Gui)

target_include_directories(comet PRIVATE headers)

# Test executable
add_executable(comet_tests
    tests/test_main.cpp
    headers/astronomicalbody.h
    headers/sprite.h
    sources/astronomicalbody.cpp
    sources/sprite.cpp
)

target_link_libraries(comet_tests PRIVATE Catch2::Catch2WithMain Qt6::Core Qt6::Gui)
target_include_directories(comet_tests PRIVATE headers)

add_test(NAME CometTests COMMAND comet_tests)
