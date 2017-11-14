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

    if(Boost_VERSION VERSION_LESS 105900)
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
    else()
        render_test_runner(
            TARGET boost_test
            VERSION 1_65
          )
    endif()
endif()

install(
    FILES
        teamcity_boost.cpp
    DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}-${PROJECT_VERSION}"
    COMPONENT boost
  )
