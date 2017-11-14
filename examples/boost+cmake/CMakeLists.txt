cmake_minimum_required(VERSION 3.9)

project(tctest VERSION 1.0.0 LANGUAGES CXX)

# Find Boost UTF...
find_package(Boost MODULE COMPONENTS unit_test_framework)
# ... and TeamCity integration library
# NOTE If found, this package would provide `teamcity-cpp-boost` object library.
find_package(teamcity-cpp 3 CONFIG COMPONENTS boost)

# TODO Add source files to your unit tests executable.
# NOTE Use generator expression to add objects provided by the integration pckage.
add_executable(
    tctest
    test.cpp
  )

# Link w/ boost UTF (as usual)
target_link_libraries(
    tctest
    TeamCity::cpp::boost
    Boost::unit_test_framework
  )
