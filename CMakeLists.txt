cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(libshp VERSION 1.0 LANGUAGES C)

set(C_STANDARD 11)
option(MAKE_TESTS FALSE "Build Unit Tests")

add_library(libshp ./src/csocket.c)
add_library(LIBSHP::libshp ALIAS libshp)

target_include_directories(libshp
		PUBLIC
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
		)

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/LIBSHP)

install(TARGETS libshp
		EXPORT libshp-targets
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT libshp-targets
		FILE
		LIBSHPTargets.cmake
		NAMESPACE
		LIBSHP::
		DESTINATION
		${INSTALL_CONFIGDIR}
		)

export(EXPORT libshp-targets
		FILE ${CMAKE_CURRENT_BINARY_DIR}/LIBSHPTargets.cmake
		NAMESPACE LIBSHP::)

export(PACKAGE libshp)

#if(MAKE_TESTS)
	add_definitions(-DUNIT_TESTING)
	enable_testing()
	add_subdirectory(tests)
#endif()