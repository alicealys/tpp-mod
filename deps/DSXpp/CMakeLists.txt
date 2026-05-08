cmake_minimum_required(VERSION 3.10)

project(DualSenseX++)

option(DSX_V2 "Target DSX Version 2" OFF)

set(ENV{ENABLE_VIRTUAL_TERMINAL_PROCESSING} "1")

if(NOT DSX_V2)
    add_definitions(-DDSX_V3)
    message(STATUS "Build Targer: DSX version 3+")
else()
    message(STATUS "Build Targer: DSX version 2")
endif()

# by default target is DSX Version 3
# to build targeting DSX Version 2, you need to provide the above option
# e.g., cmake -B build . -DDSX_V2=ON

set(DEFAULT_BUILD_TYPE "Release")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  set(DEFAULT_BUILD_TYPE "Debug")
endif()

add_library(DSX++ STATIC
    src/UDPClient.cpp
    src/DSX++.cpp
)

# build DSXTest CLI app

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v5.0.0
)
FetchContent_MakeAvailable(ftxui)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
)

add_executable(DSXTest
    src/DSXTest/main.cpp
)

target_link_libraries(DSXTest
    PRIVATE DSX++
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
)

# build DSXSample app

add_executable(DSXSample
    src/DSXSample/sample.cpp
)

target_link_libraries(DSXSample
    PRIVATE DSX++
)


