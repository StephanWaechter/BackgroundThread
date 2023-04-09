# CMakeList.txt : CMake project for CMakeTest, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
project(FutureTest VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 17)

add_executable(FutureTest  "main.cpp") 

target_link_libraries(FutureTest)

install(TARGETS FutureTest DESTINATION ${CMAKE_BINARY_DIR})