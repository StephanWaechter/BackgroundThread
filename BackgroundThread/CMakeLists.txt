# CMakeList.txt : CMake project for CMakeTest, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
project(BackgroundThreadLibrary VERSION 0.0.2)

add_library(BackgroundThread  "ThreadPool.cpp" "UiWorker.cpp")
target_compile_definitions(BackgroundThread PUBLIC cxx_std_17)
target_include_directories(BackgroundThread PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:include>)
target_link_libraries(BackgroundThread)


set_target_properties(BackgroundThread PROPERTIES PUBLIC_HEADER
"Task.hpp;ThreadPool.hpp;Token.hpp;Types.hpp;UiWork.hpp;UiWorker.hpp")

add_library(SWTHREADING::BackgroundThread ALIAS BackgroundThread)

install(TARGETS BackgroundThread EXPORT BackgroundThreadTargets
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/BackgroundThread
)

install(EXPORT BackgroundThreadTargets
	DESTINATION lib/cmake/BackgroundThread
	FILE BackgroundThreadTargets.cmake
	NAMESPACE SWTHREADING
)
