cmake_minimum_required(VERSION 3.12)

project(viewer)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_executable(viewer main.c)

add_subdirectory(deps/SDL3-3.2.16)
add_subdirectory(deps/SDL3_image-3.2.4)

target_link_libraries(viewer PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)
