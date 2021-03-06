find_program(DIFF_EXECUTABLE diff)

if(NOT DIFF_EXECUTABLE)
    message(FATAL_ERROR "`diff` executable not found")
endif()

set(_rtr_base_dir "${CMAKE_CURRENT_LIST_DIR}")

function(render_test_runner)
    set(_options)
    set(_one_value_args TARGET VERSION)
    set(_multi_value_args TEST_ARGS)
    cmake_parse_arguments(_rtr "${_options}" "${_one_value_args}" "${_multi_value_args}" ${ARGN})

    if(_rtr_VERSION)
        set(_rtr_VERSION "_${_rtr_VERSION}")
    endif()

    if(NOT (_rtr_TARGET AND TARGET ${_rtr_TARGET}))
        message(FATAL_ERROR "TARGET option is missed or invalid")
    endif()

    foreach(output "output" "error")
        foreach(kind "" "_flowid")
            if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_rtr_TARGET}${kind}.expected.${output}${_rtr_VERSION}")
                message(FATAL_ERROR "Missed ${_rtr_TARGET}${kind}.expected.${output}${_rtr_VERSION} in ${CMAKE_CURRENT_SOURCE_DIR}")
            endif()
            set(_rtr${kind}_expected_${output} "${CMAKE_CURRENT_SOURCE_DIR}/${_rtr_TARGET}${kind}.expected.${output}${_rtr_VERSION}")
        endforeach()
    endforeach()

    configure_file("${_rtr_base_dir}/run_test.cmake.in" "${_rtr_TARGET}.cmake" @ONLY)

    add_test(
        NAME ${_rtr_TARGET}
        COMMAND ${CMAKE_COMMAND} -DTEST_ARGS=${_rtr_TEST_ARGS} -P "${_rtr_TARGET}.cmake"
      )

    add_test(
        NAME ${_rtr_TARGET}_flowid
        COMMAND ${CMAKE_COMMAND} -DPRETEND_TEAMCITY=ON -DTEST_ARGS=${_rtr_TEST_ARGS} -P "${_rtr_TARGET}.cmake"
      )

    add_custom_target(
        update_${_rtr_TARGET}_expectations
        COMMAND ${CMAKE_COMMAND} -DUPDATE_EXPECTATIONS=ON -DTEST_ARGS=${_rtr_TEST_ARGS} -P "${_rtr_TARGET}.cmake"
        COMMAND ${CMAKE_COMMAND} -DUPDATE_EXPECTATIONS=ON -DPRETEND_TEAMCITY=ON -DTEST_ARGS=${_rtr_TEST_ARGS} -P "${_rtr_TARGET}.cmake"
        COMMENT "Update test output expectations at source directory: ${CMAKE_CURRENT_SOURCE_DIR}"
      )
endfunction()
