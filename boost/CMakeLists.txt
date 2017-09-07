if(BUILD_TESTING)
    add_executable(
        boost_test
        boost_test.cpp
        teamcity_boost.cpp
        $<TARGET_OBJECTS:common>
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
        boost_test PRIVATE
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

configure_package_config_file(
    teamcity-cpp-boost-config.cmake.in
    teamcity-cpp-boost-config.cmake
    INSTALL_DESTINATION "${PROJECT_NAME}-boost-${PROJECT_VERSION}/cmake"
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
  )

write_basic_package_version_file(
    teamcity-cpp-boost-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
  )

install(
    FILES
        teamcity_boost.cpp
        ../common/teamcity_messages.cpp
        ../common/teamcity_messages.h
    DESTINATION "${PROJECT_NAME}-boost-${PROJECT_VERSION}"
    COMPONENT boost
  )

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/teamcity-cpp-boost-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/teamcity-cpp-boost-config-version.cmake"
    DESTINATION "${PROJECT_NAME}-boost-${PROJECT_VERSION}/cmake"
    COMPONENT boost
  )

cpack_add_component(
    boost
    DISPLAY_NAME "Boost Unit Test Framework support"
    ARCHIVE_FILE "${PROJECT_NAME}-boost-${PROJECT_VERSION}"
  )
