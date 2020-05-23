cmake_minimum_required(VERSION 3.10.0)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "there is no any build configration")
    set(CMAKE_BUILD_TYPE Release ... FORCE)
    message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
endif()
project(MakeForm)
add_subdirectory(Shape)
add_subdirectory(PopWidgets)
add_subdirectory(Canvas)
add_subdirectory(VisionForm)
add_subdirectory(MySource)

if(MSVC)
    message(STATUS "current building platform: ${CMAKE_VS_PLATFORM_NAME}")
    message(STATUS "default building platform: ${CMAKE_VS_PLATFORM_NAME_DEFAULT}")
endif()


# ###==========
# # Set a default build type if none was specified
# set(default_build_type "Release")
# if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
#   set(default_build_type "Debug")
# endif()
 
# if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
#   message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
#   set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
#       STRING "Choose the type of build." FORCE)
#   # Set the possible values of build type for cmake-gui
#   set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    # "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
# endif()
# ####=====ref:https://blog.kitware.com/cmake-and-the-default-build-type/


