cmake_minimum_required(VERSION 3.16)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(UrbanTrafficSimulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------------------
# Include
# --------------------------------

include_directories(include)

# --------------------------------
# SDL2
# --------------------------------

find_package(SDL2 REQUIRED)


find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

# --------------------------------
# JsonCpp
# --------------------------------

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

# --------------------------------
# Source files
# --------------------------------

file(GLOB_RECURSE PROJECT_SOURCES
    src/*.cpp
)

# --------------------------------
# Remove main.cpp for libraries
# --------------------------------

list(FILTER PROJECT_SOURCES EXCLUDE REGEX "src/main.cpp")

# --------------------------------
# Core Library
# --------------------------------

add_library(traffic_lib
    ${PROJECT_SOURCES}
)

target_include_directories(traffic_lib PUBLIC
    include
    ${JSONCPP_INCLUDE_DIRS}
)

target_link_directories(traffic_lib PUBLIC
    ${JSONCPP_LIBRARY_DIRS}
)

target_link_libraries(traffic_lib PUBLIC
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
    ${JSONCPP_LIBRARIES}
)

# --------------------------------
# Main Executable
# --------------------------------

add_executable(main
    src/main.cpp
)

target_link_libraries(main PRIVATE
    traffic_lib
)

# --------------------------------
# Testing
# --------------------------------

enable_testing()

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)

FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE TEST_SOURCES
    tests/*.cpp
)

add_executable(unit_tests
    ${TEST_SOURCES}
)

target_link_libraries(unit_tests PRIVATE
    traffic_lib
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(unit_tests)