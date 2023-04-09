# CMakeList.txt : CMake project for CMakeTest, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
project(TestConsol VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 17)

if(TARGET SWTHREADING::BackgroundThread)
    message(STATUS "using local")
else()
    message(STATUS "using package")
    find_package(BackgroundThread CONFIG) 
endif()

add_executable(TestConsol  "console_main.cpp") 

target_link_libraries(TestConsol PRIVATE SWTHREADING::BackgroundThread)

install(TARGETS TestConsol DESTINATION ${CMAKE_BINARY_DIR})