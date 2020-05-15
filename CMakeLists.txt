#
# TeamCity C++ Reporting
#

#---------------------------------------------------------
# Init cmake
#---------------------------------------------------------
cmake_minimum_required(VERSION 3.14)

# ATTENTION Change this version on release
# TODO Get it via git?
project(
    teamcity-cpp
    VERSION 3.3.0
    DESCRIPTION "C++ Tests Reporting to TeamCity"
    HOMEPAGE_URL "https://github.com/zaufi/teamcity-cpp"
    LANGUAGES CXX
  )

#---------------------------------------------------------
# Setup package options and find required packages
#---------------------------------------------------------
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(ENABLE_BOOST "AUTO" CACHE STRING "Enable boost support (default auto)")
set(ENABLE_GTEST "AUTO" CACHE STRING "Enable GTest support (default auto)")
set(ENABLE_CPPUNIT "AUTO" CACHE STRING "Enable CppUnit support (default auto)")
option(ENABLE_IWYU "Try to use `include-what-you-use` (see http://include-what-you-use.org/)" OFF)
set(
    IWYU_AUX_ARGS
    "-Xiwyu" "--max_line_length=120"
    "-Xiwyu" "--quoted_includes_first"
    # NOTE Uncomment the line below, if IWYU suggests you smth that you don't understand %)
    #"-Xiwyu" "-v4"
    CACHE STRING
    "Additional parameters to IWYU"
    FORCE
  )
mark_as_advanced(IWYU_AUX_ARGS)

# Give a hint to the Boost finder to use dynamic libraries
set(BUILD_SHARED_LIBS ON)

if(ENABLE_IWYU)
    message(STATUS "Looking for `iwyu`")
    find_program(IWYU_EXECUTABLE NAMES include-what-you-use iwyu)
    mark_as_advanced(IWYU_EXECUTABLE)
    if(IWYU_EXECUTABLE)
        message(STATUS "Looking for `iwyu` - found (${IWYU_EXECUTABLE})")
        file(GLOB _iwyu_mapping_files "${PROJECT_SOURCE_DIR}/.iwyu.d/*.imp")
        foreach(_iwyu_mapping_file IN LISTS _iwyu_mapping_files)
            list(
                APPEND _iwyu_mapping_args
                "-Xiwyu"
                "--mapping_file=${_iwyu_mapping_file}"
              )
        endforeach()
        unset(_iwyu_mapping_file)
        unset(_iwyu_mapping_files)
        if(EXISTS "${PROJECT_SOURCE_DIR}/.iwyu.imp")
            list(
                APPEND _iwyu_mapping_args
                "-Xiwyu"
                "--mapping_file=${PROJECT_SOURCE_DIR}/.iwyu.imp"
              )
        endif()
        set(CMAKE_C_INCLUDE_WHAT_YOU_USE "${IWYU_EXECUTABLE};${IWYU_AUX_ARGS};${_iwyu_mapping_args}")
        set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "${CMAKE_C_INCLUDE_WHAT_YOU_USE}")
        unset(_iwyu_mapping_args)
    else()
        message(STATUS "Looking for `iwyu` - not found")
        message(SEND_ERROR "`ENABLE_IWYU` has requested, but `iwyu` executable not found")
        set(ENABLE_IWYU OFF)
    endif()
endif()

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

if(BUILD_TESTING)
    include(CheckCompilerOption)
    check_compiler_option(-Wall)
    check_compiler_option(-Wextra)
    check_compiler_option(-Werror)
    check_compiler_option(-Wpedantic)
    check_compiler_option(-Wconversion)
    check_compiler_option(-Wold-style-cast LANGUAGES CXX)
    check_compiler_option(-Wc++17-compat LANGUAGES CXX)
    check_compiler_option(-Weffc++ LANGUAGES CXX)
endif()

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
