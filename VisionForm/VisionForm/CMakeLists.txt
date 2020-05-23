cmake_minimum_required(VERSION 3.10.0)
project(VisionForm)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# set(UI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../PopWidgets)

add_executable(VisionForm main.cpp visionform.cpp visionform.ui visionform.qrc)
set_target_properties(VisionForm PROPERTIES VERSION 1.0 SOVERSION 1)

add_definitions(-DUNICODE -D_UNICODE)


find_package(Qt5 COMPONENTS Widgets REQUIRED)
include_directories(${Qt5Widgets_INCLUDE_DIRS})
add_definitions(${Qt5Widgets_DEFINITIONS})
target_link_libraries(VisionForm ${Qt5Widgets_LIBRARIES})
target_include_directories(VisionForm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

link_directories(${CMAKE_BINARY_DIR})
target_link_libraries(VisionForm Shape Canvas PopWidgets MySource)