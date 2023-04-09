# CMakeList.txt : CMake project for CMakeTest, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
project(BackgroundThreadLibrary VERSION 0.0.2)

add_library(BackgroundThread "Task.cpp" "Thread.cpp" )
target_compile_definitions(BackgroundThread PUBLIC cxx_std_17)
target_include_directories(BackgroundThread PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>)
target_link_libraries(BackgroundThread)

add_library(SWTHREADING::BackgroundThread ALIAS BackgroundThread)

install(TARGETS BackgroundThread EXPORT BackgroundThreadTargets
		LIBRARY DESTINATION lib,
		ARCHIVE DESTINATION lib,
		RUNTIME DESTINATION lib,
		INCLUDES DESTINATION include
)
install(EXPORT BackgroundThreadTargets
		DESTINATION lib/cmake/BackgroundThread
		FILE BackgroundThreadTargets.cmake
		NAMESPACE SWTHREADING
)