cmake_minimum_required(VERSION 3.14...4.4)

project(CRCpp VERSION 1.2.3.0)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# Build options
option(BUILD_DOC "Build documentation" OFF)

# Header-only Library Target
add_library(${PROJECT_NAME} INTERFACE)

# Compiler options
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

target_include_directories(${PROJECT_NAME} 
    INTERFACE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Propagates C++11 to users
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_11)

# pkg-config configuration file
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CRCpp.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/CRCpp.pc"
    @ONLY)

# CMake configuration files
configure_package_config_file(
    cmake/CRCppConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/CRCppConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/crcpp")

write_basic_package_version_file(CRCppConfigVersion.cmake
                                 VERSION ${PROJECT_VERSION}
                                 COMPATIBILITY SameMajorVersion
                                 ARCH_INDEPENDENT)

# Installation
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/inc/" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(FILES
          "${CMAKE_CURRENT_BINARY_DIR}/CRCpp.pc"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/pkgconfig")
install(FILES
          "${CMAKE_CURRENT_BINARY_DIR}/CRCppConfig.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/CRCppConfigVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/crcpp")

install(TARGETS ${PROJECT_NAME}
        EXPORT CRCppTargets)

install(EXPORT CRCppTargets
        FILE CRCppTargets.cmake
        NAMESPACE CRCpp::
        DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/crcpp")

if(CMAKE_VERSION VERSION_GREATER_EQUAL "4.3")
    install(PACKAGE_INFO CRCpp
            DESTINATION "${CMAKE_INSTALL_DATADIR}/cps"
            EXPORT CRCppTargets
            VERSION ${PROJECT_VERSION}
            COMPAT_VERSION "1.0.0.0" 
            LICENSE "BSD-3-Clause"
            DESCRIPTION "Easy to use and fast C++ CRC library."
            HOMEPAGE_URL "https://github.com/d-bahr/CRCpp")
endif()

# Documentation
if(BUILD_DOC)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        add_custom_target(doxygen
                          COMMAND "${DOXYGEN_EXECUTABLE}" Doxyfile.dox
                          WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/doxygen"
                          VERBATIM)
    else()
        message(WARNING "Doxygen must be installed to generate documentation")
    endif()
endif()

# Unit tests
include(CTest)
if(BUILD_TESTING)
    add_executable(unittest "${CMAKE_CURRENT_SOURCE_DIR}/test/src/main.cpp")
    target_link_libraries(unittest PRIVATE ${PROJECT_NAME})
    add_test(NAME unittest 
             COMMAND $<TARGET_FILE:unittest>)
    add_custom_target(tests
                      COMMAND "${CMAKE_CTEST_COMMAND}" --output-on-failure -C $<CONFIG>
                      DEPENDS unittest
                      VERBATIM)
endif()
