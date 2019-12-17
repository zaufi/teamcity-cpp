include_guard(GLOBAL)

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

get_property(_check_compiler_option_enabled_languages GLOBAL PROPERTY ENABLED_LANGUAGES)

function(check_compiler_option OPTION)
    set(_options ALSO_PASS_TO_LINKER)
    set(_one_value_args LANGUAGE RESULT_VARIABLE)
    set(_multi_value_args)
    cmake_parse_arguments(PARSE_ARGV 0 _arg "${_options}" "${_one_value_args}" "${_multi_value_args}")

    if(NOT _arg_LANGUAGE)
        if(CXX IN_LIST _check_compiler_option_enabled_languages)
            set(_arg_LANGUAGE "CXX")
        elseif(C IN_LIST _check_compiler_option_enabled_languages)
            set(_arg_LANGUAGE "C")
        else()
            message(FATAL_ERROR "Code review required")
        endif()
    endif()

    if(NOT _arg_RESULT_VARIABLE)
        string(MAKE_C_IDENTIFIER "COMPILER_HAS${OPTION}_OPTION" _arg_RESULT_VARIABLE)
        string(REPLACE "___" "_" _arg_RESULT_VARIABLE "${_arg_RESULT_VARIABLE}")
        string(REPLACE "__" "_" _arg_RESULT_VARIABLE "${_arg_RESULT_VARIABLE}")
        string(TOUPPER "${_arg_RESULT_VARIABLE}" _arg_RESULT_VARIABLE)
    endif()

    if(_arg_ALSO_PASS_TO_LINKER)
        set(_saved_CMAKE_REQUIRED_LINK_OPTIONS "${CMAKE_REQUIRED_LINK_OPTIONS}")
        list(APPEND CMAKE_REQUIRED_LINK_OPTIONS ${OPTION})
    endif()

    if(_arg_LANGUAGE STREQUAL "CXX")
        check_cxx_compiler_flag("${OPTION}" ${_arg_RESULT_VARIABLE})
    elseif(_arg_LANGUAGE STREQUAL "C")
        check_c_compiler_flag("${OPTION}" ${_arg_RESULT_VARIABLE})
    else()
        message(FATAL_ERROR "Unsupported language ${_arg_LANGUAGE}")
    endif()

    if(_arg_ALSO_PASS_TO_LINKER)
        set(CMAKE_REQUIRED_LINK_OPTIONS "${_saved_CMAKE_REQUIRED_LINK_OPTIONS}")
    endif()

    if(${_arg_RESULT_VARIABLE})
        set(${_arg_RESULT_VARIABLE} "${${_arg_RESULT_VARIABLE}}" PARENT_SCOPE)
    else()
        # NOTE It is important to set `false` value as well and do not leave it
        # undefined, so generator expressions could use it for boolean conversion.
        set(${_arg_RESULT_VARIABLE} FALSE PARENT_SCOPE)
    endif()

endfunction()
