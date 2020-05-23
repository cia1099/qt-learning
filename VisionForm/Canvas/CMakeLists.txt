cmake_minimum_required(VERSION 3.10.0)
project(Canvas)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
add_library(Canvas SHARED canvas.cpp)
set_target_properties(Canvas PROPERTIES VERSION 1.0 SOVERSION 1)

add_definitions(-DUNICODE -D_UNICODE -DCANVAS_DLL_EXPORTS)


find_package(Qt5 COMPONENTS Widgets REQUIRED)
include_directories(${Qt5Widgets_INCLUDE_DIRS})
add_definitions(${Qt5Widgets_DEFINITIONS})
target_link_libraries(Canvas ${Qt5Widgets_LIBRARIES})
target_include_directories(Canvas PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

link_directories(${CMAKE_BINARY_DIR})
target_link_libraries(Canvas Shape)