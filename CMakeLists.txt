cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

project(KingKong
	LANGUAGES CXX
)

set(KINGKONG_IS_MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(KINGKONG_IS_MAIN_PROJECT ON)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if(NOT CMAKE_BUILD_TYPE)
	if (NOT KINGKONG_BUILD_TESTS)
		message(STATUS "No build type selected, default to Release")
		set(CMAKE_BUILD_TYPE "Release")
	else()
		message(STATUS "No build type selected but tests are being built, default to Debug")
		set(CMAKE_BUILD_TYPE "Debug")
	endif()
endif()

option(KINGKONG_BUILD_EXAMPLES     "Build the examples in the project"      ${KINGKONG_IS_MAIN_PROJECT})
option(KINGKONG_BUILD_TESTS        "Build the tests in the project"         ${KINGKONG_IS_MAIN_PROJECT})
option(KINGKONG_AMALGAMATE         "Combine all headers into one"           OFF)
option(KINGKONG_INSTALL            "Add install step for KingKong"              ON )

option(KINGKONG_ENABLE_SSL         "Enable SSL capabilities (OpenSSL)"      OFF)
option(KINGKONG_ENABLE_COMPRESSION "Enable compression capabilities (ZLIB)" OFF)

add_library(KingKong INTERFACE)
add_library(KingKong::KingKong ALIAS KingKong)

target_include_directories(KingKong
	INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
)

find_package(Boost 1.64 COMPONENTS system date_time REQUIRED)
find_package(Threads REQUIRED)

target_link_libraries(KingKong
	INTERFACE
		Boost::boost Boost::system Boost::date_time
		Threads::Threads
)

if(KINGKONG_ENABLE_COMPRESSION)
	find_package(ZLIB REQUIRED)
	target_link_libraries(KingKong INTERFACE ZLIB::ZLIB)
	target_compile_definitions(KingKong INTERFACE KINGKONG_ENABLE_COMPRESSION)
endif()

if(KINGKONG_ENABLE_SSL)
	find_package(OpenSSL REQUIRED)
	target_link_libraries(KingKong INTERFACE OpenSSL::SSL)
	target_compile_definitions(KingKong INTERFACE KINGKONG_ENABLE_SSL)
endif()

if(KINGKONG_AMALGAMATE)
	add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/kingkong_all.h
		COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/scripts/merge.py
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${CMAKE_CURRENT_BINARY_DIR}/kingkong_all.h
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h ${CMAKE_CURRENT_SOURCE_DIR}/include/kingkong/*.h ${CMAKE_CURRENT_SOURCE_DIR}/include/kingkong/middlewares/*.h
	)

	add_custom_target(kingkong_amalgamated ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/kingkong_all.h)
endif()

if(KINGKONG_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

if(NOT MSVC AND KINGKONG_BUILD_TESTS)
	if(NOT KINGKONG_ENABLE_COMPRESSION)
		message(STATUS "Compression tests are omitted. (Configure with KINGKONG_ENABLE_COMPRESSION=ON to enable them)")
	endif()
	if(NOT KINGKONG_ENABLE_SSL)
		message(STATUS "SSL tests are omitted. (Configure with KINGKONG_ENABLE_SSL=ON to enable them)")
	else()
		add_test(NAME ssl_test COMMAND ${CMAKE_CURRENT_BINARY_DIR}/tests/ssl/ssltest)
	endif()

	add_subdirectory(tests)
	enable_testing()
	add_test(NAME kingkong_test COMMAND ${CMAKE_CURRENT_BINARY_DIR}/tests/unittest)
	add_test(NAME template_test COMMAND ${CMAKE_CURRENT_BINARY_DIR}/tests/template/test.py WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests/template)
endif()

if(KINGKONG_INSTALL)
	install(TARGETS KingKong EXPORT KingKongTargets)
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)

	install(EXPORT KingKongTargets
		FILE KingKongTargets.cmake
		NAMESPACE KingKong::
		DESTINATION lib/cmake/KingKong
	)

	include(CMakePackageConfigHelpers)
	configure_package_config_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/cmake/kingkong.cmake.in"
		"${CMAKE_CURRENT_BINARY_DIR}/KingKong.cmake"
		INSTALL_DESTINATION lib/cmake/KingKong
	)
	install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KingKong.cmake"
    DESTINATION lib/cmake/KingKong
  )
endif()

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "KingKong")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "KingKongCpp")
set(CPACK_PACKAGE_VENDOR "KingKongCpp")
set(CPACK_PACKAGE_DESCRIPTION "A Fast and Easy C++ Backend")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/krishpranav")
set(CPACK_DEBIAN_PACKAGE_DEBUG OFF)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)