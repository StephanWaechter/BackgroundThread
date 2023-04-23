# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project (BackgroundThreadTest)

add_subdirectory("BackgroundThread")
add_subdirectory ("TestConsole")
add_subdirectory("TestGtk")

#add_subdirectory("FutureTest")