# required for C++11 features
CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

# project name + language used
PROJECT(symdiff CXX)

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

SET(${CXX_STANDARD_REQUIRED} ON)

# Project's Options
# ====================================

OPTION(BUILD_TESTING        "Enable tests"       ON)
OPTION(USE_LLVM_IR          "Enable LLVM-IR"     OFF)
OPTION(BUILD_EXAMPLES       "Build examples"     OFF)
OPTION(BUILD_BENCHMARK      "Build Benchmarks"   OFF)
OPTION(BUILD_DOCUMENTATION  "Build docs"         OFF)
OPTION(BUILD_DOXYGEN        "Build Doxygen docs" OFF)
OPTION(BUILD_SPHINX         "Build Sphinx docs"  OFF)

# Binary/pre-compiled Dependencies
# ====================================

FIND_PACKAGE(Git REQUIRED)

IF(${USE_LLVM_IR})
    FIND_PACKAGE(LLVM)

    IF(${LLVM_FOUND})
        MESSAGE(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
        MESSAGE(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

        # I think cmake llvm utils 'bug' (seek usr/lib/llvm instead of usr/lib/llvm-3.8)
        INCLUDE_DIRECTORIES("/usr/lib/llvm-3.8/include/")
        ADD_DEFINITIONS(${LLVM_DEFINITIONS})

            #ExecutionEngine
            #Interpreter
            #MC

        llvm_map_components_to_libnames(LLVM_LIBS
            Support
            Core
            nativecodegen)

    ENDIF()
ENDIF()

# Configuration

# Find revision ID and hash of the sourcetree
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})
INCLUDE(cmake/genrev.cmake)

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

IF(BUILD_EXAMPLES)
    MESSAGE(STATUS "Building examples")
    ADD_SUBDIRECTORY(examples)
ENDIF(BUILD_EXAMPLES)
