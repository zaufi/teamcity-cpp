if(BUILD_TESTING)
    add_executable(
        gtest_test
        gtest_test.cpp
        teamcity_gtest.cpp
      )

    target_link_libraries(
        gtest_test PRIVATE
        GTest::GTest
        ${CMAKE_THREAD_LIBS_INIT}
        TeamCity::cpp::common
      )

    render_test_runner(
        TARGET gtest_test
      )
endif()

install(
    FILES
        teamcity_gtest.cpp
        teamcity_gtest.h
        ../common/teamcity_messages.cpp
        ../common/teamcity_messages.h
    DESTINATION "${PROJECT_NAME}-gtest-${PROJECT_VERSION}"
    COMPONENT gtest
  )
