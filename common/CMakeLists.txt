add_library(
    common INTERFACE
  )
add_library(TeamCity::cpp::common ALIAS common)

target_sources(
    common INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/teamcity_messages.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/teamcity_messages.h"
  )

target_include_directories(
    common INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}"
  )

install(
    FILES
        teamcity_messages.cpp
        teamcity_messages.h
    DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}-${PROJECT_VERSION}"
    COMPONENT boost
  )
install(
    FILES
        teamcity_messages.cpp
        teamcity_messages.h
    DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}-${PROJECT_VERSION}"
    COMPONENT gtest
  )
# TODO Install this files to other components

if(BUILD_TESTING)

    add_library(build_test)

    target_link_libraries(
        build_test
        PRIVATE
            TeamCity::cpp::common
      )

    target_compile_options(
        build_test
        PRIVATE
            $<$<BOOL:${COMPILER_HAS_WALL_OPTION}>:-Wall>
            $<$<BOOL:${COMPILER_HAS_WEXTRA_OPTION}>:-Wextra>
            $<$<BOOL:${COMPILER_HAS_WERROR_OPTION}>:-Werror>
            $<$<COMPILE_LANGUAGE:CXX>:
                $<$<BOOL:${COMPILER_HAS_WOLD_STYLE_CAST_OPTION}>:-Wold-style-cast>
                $<$<BOOL:${COMPILER_HAS_WC_17_COMPAT_OPTION}>:-Wc++17-compat>
                $<$<BOOL:${COMPILER_HAS_WEFFC_OPTION}>:-Weffc++>
            >
      )

endif()
