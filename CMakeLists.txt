cmake_minimum_required(VERSION 3.16)
project(comet VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)



add_executable(comet
    main.cpp
    headers/planetcontrolwidget.h
    headers/solarsystem.h
    headers/solarsystemcontroller.h
    headers/astronomicalbody.h
    sources/astronomicalbody.cpp
    sources/solarsystem.cpp
    sources/solarsystemcontroller.cpp
    sources/planetcontrolwidget.cpp
)

target_link_libraries(comet PRIVATE Qt6::Core Qt6::Widgets Qt6::Gui)

target_include_directories(comet PRIVATE headers)
