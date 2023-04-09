# CMakeList.txt : CMake project for CMakeTest, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
project(TestGtk VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 17)

if(NOT TARGET SWTHREADING::BackgroundThread)
    find_package(BackgroundThread CONFIG)
endif()

FIND_PACKAGE(PkgConfig REQUIRED)
FIND_PACKAGE(Threads REQUIRED)
 
add_executable(TestGtk  "gtk_main.cpp") 
pkg_check_modules(GTKMM gtkmm-4.0 REQUIRED)

target_include_directories(TestGtk PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
    PRIVATE
    ${GTKMM_INCLUDE_DIRS})
target_link_libraries(
    TestGtk PRIVATE 
    ${GTKMM_LINK_LIBRARIES}
    SWTHREADING::BackgroundThread)

# TODO: Add tests and install targets if needed.
install(TARGETS TestGtk DESTINATION ${CMAKE_BINARY_DIR})