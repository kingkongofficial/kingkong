cmake_minimum_required(VERSION 3.15)
project(kingkong_examples)

include(${CMAKE_SOURCE_DIR}/cmake/compiler_options.cmake)

add_executable(helloworld helloworld.cpp)
add_warnings_optimizations(helloworld)
target_link_libraries(helloworld PUBLIC kingkong::kingkong)

# if(KINGKONG_ENABLE_COMPRESSION)
#   add_executable(example_compression example_compression.cpp)
#   add_warnings_optimizations(example_compression)
#   target_link_libraries(example_compression kingkong::kingkong)
# else()
#   message(STATUS "example_compression example deactivated")
# endif()

# if(KINGKONG_ENABLE_SSL)
#   add_executable(example_ssl ssl/example_ssl.cpp)
#   add_warnings_optimizations(example_ssl)
#   target_link_libraries(example_ssl PUBLIC kingkong::kingkong)
# else()
#   message(STATUS "example_ssl example deactivated")
# endif()

# add_executable(example_websocket websocket/example_ws.cpp)
# add_warnings_optimizations(example_websocket)
# target_link_libraries(example_websocket PUBLIC kingkong::kingkong)
# add_custom_command(OUTPUT ws.html
#   COMMAND ${CMAKE_COMMAND} -E
#   copy ${PROJECT_SOURCE_DIR}/websocket/templates/ws.html ${CMAKE_CURRENT_BINARY_DIR}/templates/ws.html
#   DEPENDS ${PROJECT_SOURCE_DIR}/websocket/templates/ws.html
# )
# add_custom_target(example_ws_copy ALL DEPENDS ws.html)

# add_executable(basic_example example.cpp)
# add_warnings_optimizations(basic_example)
# target_link_libraries(basic_example PUBLIC kingkong::kingkong)
# add_custom_command(OUTPUT example_test.py
#   COMMAND ${CMAKE_COMMAND} -E
#   copy ${PROJECT_SOURCE_DIR}/example_test.py ${CMAKE_CURRENT_BINARY_DIR}/example_test.py
#   DEPENDS ${PROJECT_SOURCE_DIR}/example_test.py
# )
# add_custom_target(example_copy ALL DEPENDS example_test.py)

# if(KINGKONG_AMALGAMATE)
#   add_executable(example_with_all example_with_all.cpp)
#   add_dependencies(example_with_all kingkong_amalgamated)
#   add_warnings_optimizations(example_with_all)
#   target_link_libraries(example_with_all PUBLIC kingkong::kingkong)
#   target_include_directories(example_with_all PUBLIC ${CMAKE_BINARY_DIR})
# endif()

# add_executable(example_chat example_chat.cpp)
# add_warnings_optimizations(example_chat)
# target_link_libraries(example_chat PUBLIC kingkong::kingkong)
# add_custom_command(OUTPUT example_chat.html
#   COMMAND ${CMAKE_COMMAND} -E
#   copy ${PROJECT_SOURCE_DIR}/example_chat.html ${CMAKE_CURRENT_BINARY_DIR}/templates/example_chat.html
#   DEPENDS ${PROJECT_SOURCE_DIR}/example_chat.html
# )
# add_custom_target(example_chat_copy ALL DEPENDS example_chat.html)

# add_executable(example_static_file example_static_file.cpp)
# add_warnings_optimizations(example_static_file)
# target_link_libraries(example_static_file PUBLIC kingkong::kingkong)

# add_executable(example_catchall example_catchall.cpp)
# add_warnings_optimizations(example_catchall)
# target_link_libraries(example_catchall PUBLIC kingkong::kingkong)

# add_executable(example_json_map example_json_map.cpp)
# add_warnings_optimizations(example_json_map)
# target_link_libraries(example_json_map PUBLIC kingkong::kingkong)

# add_executable(example_blueprint example_blueprint.cpp)
# add_warnings_optimizations(example_blueprint)
# target_link_libraries(example_blueprint PUBLIC kingkong::kingkong)

# if(MSVC)
#   add_executable(example_vs example_vs.cpp)
#   add_warnings_optimizations(example_vs)
#   target_link_libraries(example_vs kingkong::kingkong)
# endif()