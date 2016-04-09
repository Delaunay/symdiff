# required for C++11 features
CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

# project name + language used
PROJECT(symdiff CXX)




SET(CMAKE_BUILD_TYPE "Debug")

IF(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    MESSAGE(STATUS "Building Debug Version")
ELSE()
    MESSAGE(STATUS "Building Release Version")
ENDIF()

# specify where compiled files must go
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


# add cmake FindXXX script to find pre-compiled libraries if any
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")


# Project's Options
# ====================================

OPTION(BUILD_TESTING        "Enable tests"       ON)
#OPTION(BUILD_BENCHMARK      "Build Benchmarks"   ON)
#OPTION(BUILD_DOCUMENTATION  "Build docs"         OFF)
#OPTION(BUILD_DOXYGEN        "Build Doxygen docs" ON)
#OPTION(BUILD_SPHINX         "Build Sphinx docs"  ON)

# Binary/pre-compiled Dependencies
# ====================================

# FIND_PACKAGE(...)


# Subdirectories
# ====================================

# Dependencies to be compiled
ADD_SUBDIRECTORY(dependencies)

# How to compile main lib
ADD_SUBDIRECTORY(src)

# Compile tests ?
IF(BUILD_TESTING)
    ENABLE_TESTING()

    MESSAGE(STATUS "Building tests")
    ADD_SUBDIRECTORY(tests)
ENDIF(BUILD_TESTING)
