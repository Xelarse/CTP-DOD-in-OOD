cmake_minimum_required(VERSION 3.11.4)
project(Allman)
set(CMAKE_CXX_STANDARD 14)

## project independent scripts ##
include(cmake/build/flags.cmake)
include(cmake/build/platform.cmake)

## set to true to use dlls instead of static libs ##
option(STATIC_APP "Build a static application" ON)
if( STATIC_APP )
    message("##### BUILDING STATIC APPLICATION #####")
    set(BUILD_SHARED_LIBS false)
else()
    message("##### BUILDING SHARED APPLICATION #####")
    set(BUILD_SHARED_LIBS true)
endif()

## enable the game project ##
add_subdirectory(src)

