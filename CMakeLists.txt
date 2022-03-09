cmake_minimum_required (VERSION 3.5)

project(CRCpp)

# Build type
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RELEASE")
endif ()

# Build options
option(BUILD_DOC "Build documentation" OFF)
option(BUILD_TEST "Build test" ON)

# Compiler options
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Header file(s)
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/inc)

# Installation
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inc/ DESTINATION include)

# Documentation
if (BUILD_DOC)
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        set(DOXYGEN_GENERATE_HTML YES)
        add_custom_target(doxygen
                          COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile.dox
                          WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doxygen)
    else (DOXYGEN_FOUND)
        message("Doxygen must be installed to generate documentation")
    endif (DOXYGEN_FOUND)
endif ()

# Unit tests
if (BUILD_TEST)
    include(CTest)
    add_executable(unittest ${CMAKE_CURRENT_SOURCE_DIR}/test/src/main.cpp)
    set_target_properties(unittest PROPERTIES RUNTIME_OUTPUT_DIRECTORY test/bin)
    target_link_libraries(unittest ${PROJECT_NAME})
    add_test(NAME unittest
             COMMAND unittest
             WORKING_DIRECTORY ${RUNTIME_OUTPUT_DIRECTORY})
    add_custom_target(tests
                      COMMAND ${CMAKE_CTEST_COMMAND}
                      DEPENDS unittest)
endif ()
