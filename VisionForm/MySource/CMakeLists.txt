cmake_minimum_required(VERSION 3.10.0)
project(MySource)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
add_library(MySource SHARED ResourceManager.cpp)
set_target_properties(MySource PROPERTIES VERSION 1.0 SOVERSION 1)

add_definitions(-DUNICODE -D_UNICODE -DMYSOURCE_LIB)

# find_package(Qt5 COMPONENTS Core ANGLE REQUIRED)
# include_directories(${Qt5Core_INCLUDE_DIRS})
# target_link_libraries(MySource ${Qt5Core_LIBRARIES})
# target_include_directories(MySource PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Qt5 COMPONENTS Widgets REQUIRED)
include_directories(${Qt5Widgets_INCLUDE_DIRS})
add_definitions(${Qt5Widgets_DEFINITIONS})
target_link_libraries(MySource ${Qt5Widgets_LIBRARIES})