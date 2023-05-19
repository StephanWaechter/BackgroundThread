cmake_minimum_required (VERSION 3.8)

project (BackgroundThreadTest)

add_subdirectory("BackgroundThread")
add_subdirectory ("TestConsole")

FIND_PACKAGE(PkgConfig REQUIRED)
pkg_check_modules(GTKMM gtkmm-4.0)

if(${GTKMM_FOUND})
    add_subdirectory("TestGtk")
else()
    message(STATUS "gtkmm-4.0 not install dont build GtkDemo")
endif()

