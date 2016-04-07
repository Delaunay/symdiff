# required for C++11 features
CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

# project name + language used
PROJECT(symdiff CXX)

# SET(CMAKE_RELEASE_POSTFIX "")
SET(CMAKE_DEBUG_POSTFIX "")

SET(CMAKE_BUILD_TYPE "Debug")

IF(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    MESSAGE(STATUS "Building Debug Version")
ELSE()
    MESSAGE(STATUS "Building Release Version")
ENDIF()

# How to compile main lib
ADD_SUBDIRECTORY(src)
