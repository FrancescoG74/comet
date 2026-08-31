cmake_minimum_required(VERSION 3.16)
project(comet VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)
find_package(Catch2 REQUIRED)

enable_testing()

option(COMET_ENABLE_SANITIZERS "Build with AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

# Applied only to our own targets; third_party/astronomy is built as-is.
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(COMET_WARNING_FLAGS -Wall -Wextra -Wpedantic)
else()
    set(COMET_WARNING_FLAGS "")
endif()

if(COMET_ENABLE_SANITIZERS)
    set(COMET_SANITIZER_FLAGS -fsanitize=address,undefined -fno-omit-frame-pointer)
else()
    set(COMET_SANITIZER_FLAGS "")
endif()

# Astronomy Engine (MIT license, https://github.com/cosinekitty/astronomy)
# provides real ephemeris-based positions for the Sun, planets and Moon.
add_library(astronomy_engine STATIC
    third_party/astronomy/astronomy.c
)
target_include_directories(astronomy_engine PUBLIC third_party/astronomy)

add_executable(comet
    src/main.cpp
    resources.qrc
    headers/ui/planetcontrolwidget.h
    headers/ui/planetcontrolcontroller.h
    headers/simulation/solarsystem.h
    headers/simulation/solarsystemmodel.h
    headers/simulation/solarsystemcontroller.h
    headers/simulation/orbitalelements.h
    headers/core/astronomicalbody.h
    headers/core/sprite.h
    src/core/astronomicalbody.cpp
    src/simulation/solarsystem.cpp
    src/simulation/solarsystemmodel.cpp
    src/simulation/solarsystemcontroller.cpp
    src/ui/planetcontrolwidget.cpp
    src/ui/planetcontrolcontroller.cpp
    src/core/sprite.cpp
)

target_link_libraries(comet PRIVATE Qt6::Core Qt6::Widgets Qt6::Gui astronomy_engine)

target_compile_options(comet PRIVATE ${COMET_WARNING_FLAGS} ${COMET_SANITIZER_FLAGS})
target_link_options(comet PRIVATE ${COMET_SANITIZER_FLAGS})


target_include_directories(comet PRIVATE 
    headers
    headers/core
    headers/simulation
    headers/ui
)

# Test executable
add_executable(comet_tests
    tests/test_main.cpp
    resources.qrc
    tests/core/test_astronomicalbody.cpp
    tests/core/test_sprite.cpp
    tests/simulation/test_physics.cpp
    tests/simulation/test_solarsystemmodel.cpp
    tests/simulation/test_orbitalelements.cpp
    tests/ui/test_planetcontrolwidget.cpp
    tests/ui/test_planetcontrolcontroller.cpp
    headers/core/astronomicalbody.h
    headers/core/sprite.h
    headers/simulation/orbitalelements.h
    headers/simulation/solarsystem.h
    headers/simulation/solarsystemmodel.h
    headers/simulation/solarsystemcontroller.h
    headers/ui/planetcontrolwidget.h
    headers/ui/planetcontrolcontroller.h
    src/core/astronomicalbody.cpp
    src/core/sprite.cpp
    src/simulation/solarsystem.cpp
    src/simulation/solarsystemmodel.cpp
    src/simulation/solarsystemcontroller.cpp
    src/ui/planetcontrolwidget.cpp
    src/ui/planetcontrolcontroller.cpp
)

target_link_libraries(comet_tests PRIVATE Catch2::Catch2WithMain Qt6::Core Qt6::Widgets Qt6::Gui astronomy_engine)
target_compile_options(comet_tests PRIVATE ${COMET_WARNING_FLAGS} ${COMET_SANITIZER_FLAGS})
target_link_options(comet_tests PRIVATE ${COMET_SANITIZER_FLAGS})
target_include_directories(comet_tests PRIVATE 
    headers
    headers/core
    headers/simulation
    headers/ui
)

add_test(NAME CometTests COMMAND comet_tests)
