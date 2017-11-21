#
# TeamCity C++ Reporting
#

#---------------------------------------------------------
# Init cmake
#---------------------------------------------------------
cmake_minimum_required(VERSION 3.9)

# ATTENTION Change this version on release
# TODO Get it via git?
project(
    teamcity-cpp
    VERSION 3.0.2
    DESCRIPTION "C++ Tests Reporting to TeamCity"
    LANGUAGES CXX
  )

#---------------------------------------------------------
# Setup package options and find required packages
#---------------------------------------------------------
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(ENABLE_BOOST "AUTO" CACHE STRING "Enable boost support (default auto)")
set(ENABLE_GTEST "AUTO" CACHE STRING "Enable GTest support (default auto)")
set(ENABLE_CPPUNIT "AUTO" CACHE STRING "Enable CppUnit support (default auto)")

# Find supported testing libs
# - boost UTF
if(ENABLE_BOOST OR ENABLE_BOOST STREQUAL "AUTO")
    find_package(Boost COMPONENTS unit_test_framework)
    if(Boost_FOUND)
        set(HAVE_BOOST ON)
    else()
        set(HAVE_BOOST OFF)
        if(ENABLE_BOOST AND NOT ENABLE_BOOST STREQUAL "AUTO")
            message(SEND_ERROR "Boost support has requested, but library was not found")
        endif()
    endif()
endif()
# Google test
if(ENABLE_GTEST OR ENABLE_GTEST STREQUAL "AUTO")
    find_package(GTest)
    if(GTEST_FOUND)
        set(HAVE_GTEST ON)
    else()
        set(HAVE_GTEST OFF)
        if(ENABLE_GTEST AND NOT ENABLE_GTEST STREQUAL "AUTO")
            message(SEND_ERROR "GTest support has requested, but library was not found")
        endif()
    endif()
endif()
# Cpp Unit
if(ENABLE_CPPUNIT OR ENABLE_CPPUNIT STREQUAL "AUTO")
    find_package(CppUnit)
    if(CPPUNIT_FOUND)
        set(HAVE_CPPUNIT ON)
    else()
        set(HAVE_CPPUNIT OFF)
        if(ENABLE_CPPUNIT AND NOT ENABLE_CPPUNIT STREQUAL "AUTO")
            message(SEND_ERROR "CppUnit support has requested, but library was not found")
        endif()
    endif()
endif()
find_package(Threads)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(RenderTestRunner)
include(CTest)
enable_testing()

#---------------------------------------------------------
# Dive into sub-directories
#---------------------------------------------------------

add_subdirectory(common)
if(HAVE_BOOST)
    add_subdirectory(boost)
endif()
if(HAVE_GTEST)
    add_subdirectory(gtest)
endif()
if(HAVE_CPPUNIT)
    add_subdirectory(cppunit)
endif()

#---------------------------------------------------------
# Prepare package configs
#---------------------------------------------------------
# NOTE In fact, this package do not install any
# binary code, so could be considered as `noarch`.
# To make it so, `CMAKE_SIZEOF_VOID_P` must be unset
# before generate package configuration files, so being
# found it won't check target architecture.
unset(CMAKE_SIZEOF_VOID_P)
configure_package_config_file(
    ${PROJECT_NAME}-config.cmake.in
    ${PROJECT_NAME}-config.cmake
    INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake"
  )

write_basic_package_version_file(
    ${PROJECT_NAME}-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
  )

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}-${PROJECT_VERSION}"
    COMPONENT boost
  )

#---------------------------------------------------------
# Prepare to make packages
#---------------------------------------------------------

if(WIN32)
    set(CPACK_GENERATOR ZIP)
else()
    set(CPACK_GENERATOR TBZ2)
endif()
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")
include(CPack)
