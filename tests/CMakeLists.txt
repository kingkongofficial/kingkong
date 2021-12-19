cmake_minimum_required(VERSION 3.15)
project(kingkong_test)

include(${CMAKE_SOURCE_DIR}/cmake/compiler_options.cmake)

set(TEST_SRCS
  unitest.cpp
)

add_executable(unitest ${TEST_SRCS})
target_link_libraries(unitest Kingkong::Kingkong)
add_warnings_optimizations(unitest)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set_target_properties(unitest PROPERTIES COMPILE_FLAGS "--coverage -fprofile-arcs -ftest-coverage")
  target_link_libraries(unitest gcov)
endif()

add_subdirectory(template)
if (KINGKONG_ENABLE_SSL)
	add_subdirectory(ssl)
endif()
add_subdirectory(img)