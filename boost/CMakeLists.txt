if(BUILD_TESTING)
    add_executable(
        boost_test
        boost_test.cpp
        teamcity_boost.cpp
      )

    target_compile_definitions(
        boost_test PRIVATE
        -DBOOST_TEST_DYN_LINK
      )
    target_compile_options(
        boost_test PRIVATE
        # TODO Check for that options
        -Wall -Wextra -Werror -Wold-style-cast -Weffc++
      )

    # NOTE Boost prior 1.60 incorrectly recognize preprocessor variadics
    # for GCC/clang
    if(Boost_VERSION VERSION_LESS 106000)
        target_compile_definitions(
            boost_test PRIVATE
            -DBOOST_PP_VARIADICS=1
          )
    endif()

    # NOTE Boost >= 1.60 require C++11 for data based tests
    # TODO Check other (upcoming) versions (after 1.61)
    if(Boost_VERSION VERSION_GREATER 106000)
        target_compile_features(
            boost_test
            PRIVATE cxx_uniform_initialization
          )
    endif()

    target_link_libraries(
        boost_test
        PRIVATE
            TeamCity::cpp::common
            Boost::unit_test_framework
      )

    # NOTE Boost 1.70.0 has native CMake support
    # and the meaning of `Boost_VERSION` has changed!
    if(Boost_VERSION VERSION_GREATER_EQUAL 1.70.0)
        render_test_runner(
            TARGET boost_test
            VERSION 1_70
            TEST_ARGS --color_output=no
          )
    elseif(Boost_VERSION VERSION_LESS 105900)
        render_test_runner(
            TARGET boost_test
          )
    elseif(Boost_VERSION VERSION_LESS 106200)
        render_test_runner(
            TARGET boost_test
            VERSION 1_59
          )
    elseif(Boost_VERSION VERSION_LESS 106500)
        render_test_runner(
            TARGET boost_test
            VERSION 1_62
          )
    elseif(Boost_VERSION VERSION_LESS 107000)
        render_test_runner(
            TARGET boost_test
            VERSION 1_65
            TEST_ARGS --color_output=no
          )
    else()
        render_test_runner(
            TARGET boost_test
            VERSION 1_70
            TEST_ARGS --color_output=no
          )
    endif()
endif()

install(
    FILES
        teamcity_boost.cpp
    DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}-${PROJECT_VERSION}"
    COMPONENT boost
  )
