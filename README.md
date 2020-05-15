teamcity-cpp
============

[![official project](http://jb.gg/badges/official.svg)](https://confluence.jetbrains.com/display/ALL/JetBrains+on+GitHub)
[![GitHub license](https://img.shields.io/badge/license-Apache%20License%202.0-blue.svg?style=flat)](http://www.apache.org/licenses/LICENSE-2.0)
[![Travis Build Status](https://travis-ci.org/JetBrains/teamcity-cpp.svg?branch=master)](https://travis-ci.org/JetBrains/teamcity-cpp)

C++ Tests Reporting to TeamCity Continuous Integration Server.

Repository consist of C++ files to include in your project that automatically reports unit tests to the TeamCity server if run within TeamCity.

The reporting is performed via TeamCity service messages.

Alternatively you may use XML Report Processing to import results from running tests to TeamCity.
Also use it for not yet supported frameworks such as CxxTest.

See http://confluence.jetbrains.com/display/TW/Cpp+Unit+Test+Reporting for downloads and more information.


How to build with CMake
=======================

    $ cd teamcity-cpp-X.Y
    $ mkdir build
    $ cd build
    $ cmake -DCMAKE_INSTALL_PREFIX=/usr/local/share ..
    $ make
    $ make test
    $ make install

At configure step (i.e. running `cmake`) one may add options to explicitly require or ignore particulat testing libraries:
* `ENABLE_BOOST`
* `ENABLE_GTEST`
* `ENABLE_CPPUNIT`


How to use Boost listener with CMake-based projects
===================================================

Boost UTF integration provides an exported target for your pleasure. Finding the `teamcity-cpp-boost` would "import" it
to your project. In fact, it is [object library](https://cmake.org/cmake/help/latest/command/add_library.html#object-libraries),
so to link it with your unit tests executable, one ought to add the following snippet to your `CMakeLists.txt`:

    find_package(teamcity-cpp-boost 1.8)

    add_executable(your-unit-tests-binary-name ... $<TARGET_OBJECTS:teamcity-cpp-boost> ...)
