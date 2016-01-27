cmake_minimum_required(VERSION 3.4)

project(tctest VERSION ${teamcity_cpp_version} LANGUAGES CXX)

# Find Boost UTF...
find_package(Boost COMPONENTS unit_test_framework)
# ... and TeamCity integration library
# NOTE If found, this package would provide `teamcity-cpp-boost` object library.
find_package(teamcity-cpp-boost)

# TODO Add source files to your unit tests executable.
# NOTE Use generator expression to add objects provided by the integration pckage.
add_executable(
    tctest
    test.cpp
    $<TARGET_OBJECTS:teamcity-cpp-boost>
  )

# Link w/ boost UTF (as usual)
target_link_libraries(
    tctest
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  )
