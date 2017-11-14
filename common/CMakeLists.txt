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
# TODO Install this files to other components
