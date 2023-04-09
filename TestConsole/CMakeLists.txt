# CMakeList.txt : CMake project for CMakeTest, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
project(TestConsol)

set(CMAKE_CXX_STANDARD 17)

if(NOT TARGET SWTHREADING::BackgroundThread)
    find_package(BackgroundThread CONFIG)
endif()

add_executable(TestConsol  "console_main.cpp") 

target_link_libraries(TestConsol PRIVATE SWTHREADING::BackgroundThread)

install(TARGETS TestConsol DESTINATION ${CMAKE_BINARY_DIR})