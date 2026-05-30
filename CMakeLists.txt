cmake_minimum_required(VERSION 3.16)

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

message(STATUS "SDL2 include dirs: ${SDL2_INCLUDE_DIRS}")

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
    /opt/homebrew/include
)

target_link_libraries(traffic_lib
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
)

# --------------------------------
# Main Executable
# --------------------------------

add_executable(main
    src/main.cpp
)

target_link_libraries(main
    traffic_lib
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
)

# --------------------------------
# Testing
# --------------------------------

enable_testing()

find_package(GTest REQUIRED)

file(GLOB_RECURSE TEST_SOURCES
    tests/*.cpp
)

add_executable(unit_tests
    ${TEST_SOURCES}
)

target_link_libraries(unit_tests
    traffic_lib
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(unit_tests)