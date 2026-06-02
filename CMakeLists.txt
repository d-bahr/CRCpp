cmake_minimum_required (VERSION 3.5)

project(CRCpp VERSION 1.2.1.0)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

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

# pkg-config configuration file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CRCpp.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/CRCpp.pc
    @ONLY)

# CMake configuration files
configure_package_config_file(
    cmake/CRCppConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/CRCppConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/crcpp")
write_basic_package_version_file(CRCppConfigVersion.cmake
                                 VERSION ${PACKAGE_VERSION}
                                 COMPATIBILITY SameMajorVersion
                                 ARCH_INDEPENDENT)

# Installation
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inc/ DESTINATION ${CMAKE_INSTALL_INCLUDE_DIR})
install(FILES
          ${CMAKE_CURRENT_BINARY_DIR}/CRCpp.pc
        DESTINATION ${CMAKE_INSTALL_DATADIR}/pkgconfig)
install(FILES
	  ${CMAKE_CURRENT_BINARY_DIR}/CRCppConfig.cmake
	  ${CMAKE_CURRENT_BINARY_DIR}/CRCppConfigVersion.cmake
	DESTINATION ${CMAKE_INSTALL_DATADIR}/cmake/crcpp)

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
