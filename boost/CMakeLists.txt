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
        boost_test
        PRIVATE
            $<$<BOOL:${COMPILER_HAS_WALL_OPTION}>:-Wall>
            $<$<BOOL:${COMPILER_HAS_WEXTRA_OPTION}>:-Wextra>
            $<$<BOOL:${COMPILER_HAS_WERROR_OPTION}>:-Werror>
            $<$<BOOL:${COMPILER_HAS_WPEDANTIC_OPTION}>:-Wpedantic>
            $<$<BOOL:${COMPILER_HAS_WCONVERSION_OPTION}>:-Wconversion>
            $<$<COMPILE_LANGUAGE:CXX>:
                $<$<BOOL:${COMPILER_HAS_WOLD_STYLE_CAST_OPTION}>:-Wold-style-cast>
                $<$<BOOL:${COMPILER_HAS_WC_17_COMPAT_OPTION}>:-Wc++17-compat>
                $<$<BOOL:${COMPILER_HAS_WOVERLOADED_VIRTUAL_OPTION}>:-Woverloaded-virtual>
                # ALERT For the GCC < 5.x this option gives warnings (Ok, errors)
                # for the internals of the Boost Test about a non-virtual destructor.
                $<$<AND:$<BOOL:${COMPILER_HAS_WEFFC_OPTION}>,$<CXX_COMPILER_ID:GNU>,$<VERSION_GREATER:$<CXX_COMPILER_VERSION>,5>>:-Weffc++>
            >
      )

    # NOTE Boost >= 1.60 require C++11 for data based tests
    target_compile_features(
        boost_test
        PRIVATE
            cxx_uniform_initialization
      )

    target_link_libraries(
        boost_test
        PRIVATE
            TeamCity::cpp::common
            Boost::unit_test_framework
      )

    render_test_runner(
        TARGET boost_test
        VERSION 1_70
        TEST_ARGS --color_output=no
      )
endif()

install(
    FILES
        teamcity_boost.cpp
    DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}-${PROJECT_VERSION}"
    COMPONENT boost
  )
