/* Copyright 2011 JetBrains s.r.o.
 * Copyright 2015-2020 Alex Turbov <i.zaufi@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * $Revision: 88625 $
 */

#include "teamcity_messages.h"

#include <boost/version.hpp>
/// \attention Some Boost UTF headers (of some versions) are
/// *NOT self-contained*, so be aware about implicit header
/// dependencies...
#if BOOST_VERSION >= 105900
# include <boost/test/execution_monitor.hpp>
#else                                                       // BOOST_VERSION < 105900
# include <boost/test/unit_test_suite_impl.hpp>
#endif                                                      // BOOST_VERSION < 105900
#include <boost/test/results_collector.hpp>
#include <boost/test/utils/basic_cstring/io.hpp>
#include <boost/test/unit_test_log.hpp>                     // IWYU pragma: keep
#include <boost/test/unit_test_log_formatter.hpp>
#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <ostream>
// NOTE Due to IWYU BUG `string` header is not exported
// properly from "teamcity_messages.h" to here...
#include <string>                                           // IWYU pragma: keep

#if __cpluscplus >= 201103L
# define TEAMCITY_BOOST_OVERRIDE override
# define TEAMCITY_BOOST_VIRTUAL
#else
# define TEAMCITY_BOOST_OVERRIDE
# define TEAMCITY_BOOST_VIRTUAL virtual
#endif

namespace jetbrains { namespace teamcity { namespace {
const std::string ASSERT_CTX = "Assertion has occurred in a following context:";
const std::string FAILURE_CTX = "Failure has occurred in a following context:";
const boost::unit_test::test_unit_type UNIT_TEST_CASE =
#if BOOST_VERSION < 105900
    boost::unit_test::tut_case
#else                                                       // BOOST_VERSION >= 105900
    boost::unit_test::TUT_CASE
#endif                                                      // BOOST_VERSION >= 105900
  ;

// Formatter implementation
std::string toString(boost::unit_test::const_string bstr)
{
    std::stringstream ss(std::ios_base::out);
    ss << bstr;
    return ss.str();
}

std::string toString(const boost::execution_exception::error_code code)
{
    switch (code)
    {
        case boost::execution_exception::no_error:
            return std::string();
        case boost::execution_exception::user_error:
            return std::string("user error");
        case boost::execution_exception::cpp_exception_error:
            return std::string("C++ exception error");
        case boost::execution_exception::system_error:
            return std::string("system error");
        case boost::execution_exception::user_fatal_error:
            return std::string("user fatal error");
        case boost::execution_exception::system_fatal_error:
            return std::string("system fatal error");
        case boost::execution_exception::timeout_error:
            return std::string("timeout error");
        default:
            break;
    }
    return std::string("unknown error");
}

std::string toString(const boost::execution_exception& excpt)
{
    std::stringstream ss(std::ios_base::out);
    ss << excpt.where().m_file_name;

    size_t line = excpt.where().m_line_num;
    if (line)
        ss << ':' << line;

    const boost::unit_test::const_string& fn = excpt.where().m_function;
    if (!fn.empty())
        ss << '[' << fn << ']';

    ss  << ": " << excpt.what();

    const std::string& code = toString(excpt.code());
    if (!code.empty())
        ss << " (" << code << ')';

    return ss.str();
}
}                                                           // anonymous namespace

/// Custom formatter for TeamCity messages
class TeamcityBoostLogFormatter : public boost::unit_test::unit_test_log_formatter
{
    TeamcityMessages messages;
    std::string currentDetails;
    std::string flowId;
#if BOOST_VERSION >= 105900
    std::string currentContextDetails;
#endif                                                      // BOOST_VERSION >= 105900

public:
    //BEGIN These virtuals have more than one overload
    using boost::unit_test::unit_test_log_formatter::log_entry_value;
    using boost::unit_test::unit_test_log_formatter::test_unit_skipped;
    //END These virtuals have more than one overload

    TeamcityBoostLogFormatter(const std::string& flowId);
    TeamcityBoostLogFormatter();

    TEAMCITY_BOOST_VIRTUAL void log_start(std::ostream&, boost::unit_test::counter_t) TEAMCITY_BOOST_OVERRIDE;
    TEAMCITY_BOOST_VIRTUAL void log_finish(std::ostream&) TEAMCITY_BOOST_OVERRIDE;
    TEAMCITY_BOOST_VIRTUAL void log_build_info(
        std::ostream&
#if BOOST_VERSION >= 107000
      , bool
#endif                                                      // BOOST_VERSION >= 107000
      ) TEAMCITY_BOOST_OVERRIDE;
    TEAMCITY_BOOST_VIRTUAL void test_unit_start(
        std::ostream&
      , const boost::unit_test::test_unit&
      ) TEAMCITY_BOOST_OVERRIDE;
    TEAMCITY_BOOST_VIRTUAL void test_unit_finish(
        std::ostream&
      , const boost::unit_test::test_unit&
      , unsigned long
      ) TEAMCITY_BOOST_OVERRIDE;

    TEAMCITY_BOOST_VIRTUAL void log_entry_start(
        std::ostream&
      , const boost::unit_test::log_entry_data&
      , log_entry_types
      ) TEAMCITY_BOOST_OVERRIDE;
    TEAMCITY_BOOST_VIRTUAL void log_entry_value(std::ostream&, boost::unit_test::const_string) TEAMCITY_BOOST_OVERRIDE;
    TEAMCITY_BOOST_VIRTUAL void log_entry_finish(std::ostream&) TEAMCITY_BOOST_OVERRIDE;

#if BOOST_VERSION < 105900
    TEAMCITY_BOOST_VIRTUAL void log_exception(
        std::ostream&
      , const boost::unit_test::log_checkpoint_data&
      , boost::unit_test::const_string
      ) TEAMCITY_BOOST_OVERRIDE;
    TEAMCITY_BOOST_VIRTUAL void test_unit_skipped(std::ostream&, const boost::unit_test::test_unit&) TEAMCITY_BOOST_OVERRIDE;
#else                                                       // BOOST_VERSION >= 105900
    TEAMCITY_BOOST_VIRTUAL void log_exception_start(
        std::ostream&
      , const boost::unit_test::log_checkpoint_data&
      , const boost::execution_exception&
      ) TEAMCITY_BOOST_OVERRIDE;
    TEAMCITY_BOOST_VIRTUAL void test_unit_skipped(
        std::ostream&
      , const boost::unit_test::test_unit&
      , boost::unit_test::const_string
      ) TEAMCITY_BOOST_OVERRIDE;

    TEAMCITY_BOOST_VIRTUAL void log_exception_finish(std::ostream&) TEAMCITY_BOOST_OVERRIDE;
    TEAMCITY_BOOST_VIRTUAL void entry_context_start(std::ostream&, boost::unit_test::log_level) TEAMCITY_BOOST_OVERRIDE;
# if BOOST_VERSION < 106500
    TEAMCITY_BOOST_VIRTUAL void log_entry_context(std::ostream&, boost::unit_test::const_string) TEAMCITY_BOOST_OVERRIDE;
    TEAMCITY_BOOST_VIRTUAL void entry_context_finish(std::ostream&) TEAMCITY_BOOST_OVERRIDE;
# else                                                      // BOOST_VERSION >= 106500
    TEAMCITY_BOOST_VIRTUAL void log_entry_context(
        std::ostream&
      , boost::unit_test::log_level
      , boost::unit_test::const_string
      ) TEAMCITY_BOOST_OVERRIDE;
    TEAMCITY_BOOST_VIRTUAL void entry_context_finish(std::ostream&, boost::unit_test::log_level) TEAMCITY_BOOST_OVERRIDE;
# endif                                                     // BOOST_VERSION >= 106500
#endif                                                      // BOOST_VERSION >= 105900
#if BOOST_VERSION >= 106200
    TEAMCITY_BOOST_VIRTUAL std::string get_default_stream_description() const TEAMCITY_BOOST_OVERRIDE
    {
        return "TeamCity (via service messages)";
    }
#endif                                                      // BOOST_VERSION >= 106200
};

// Fake fixture to register formatter
struct TeamcityFormatterRegistrar
{
    TeamcityFormatterRegistrar()
    {
        if (underTeamcity())
        {
            boost::unit_test::unit_test_log.set_formatter(new TeamcityBoostLogFormatter());
            boost::unit_test::unit_test_log.set_threshold_level(boost::unit_test::log_test_units);
        }
    }
};

#if BOOST_VERSION < 106500
    BOOST_GLOBAL_FIXTURE(TeamcityFormatterRegistrar);
#else                                                       // BOOST_VERSION >= 106500
    BOOST_TEST_GLOBAL_CONFIGURATION(TeamcityFormatterRegistrar);
#endif                                                      // BOOST_VERSION >= 106500

TeamcityBoostLogFormatter::TeamcityBoostLogFormatter()
  : messages()
  , currentDetails()
  , flowId(getFlowIdFromEnvironment())
#if BOOST_VERSION >= 105900
  , currentContextDetails()
#endif                                                      // BOOST_VERSION >= 105900
{}

TeamcityBoostLogFormatter::TeamcityBoostLogFormatter(const std::string& id)
  : messages()
  , currentDetails()
  , flowId(id)
#if BOOST_VERSION >= 105900
  , currentContextDetails()
#endif                                                      // BOOST_VERSION >= 105900
{}

void TeamcityBoostLogFormatter::log_start(
    std::ostream& out
  , boost::unit_test::counter_t /*test_cases_amount*/
  )
{
    messages.setOutput(out);
}

void TeamcityBoostLogFormatter::log_finish(std::ostream& /*out*/)
{}

void TeamcityBoostLogFormatter::log_build_info(
    std::ostream& /*out*/
#if BOOST_VERSION >= 107000
  , const bool /*log_build_info*/
#endif
  )
{}

void TeamcityBoostLogFormatter::test_unit_start(
    std::ostream& /*out*/
  , const boost::unit_test::test_unit& tu
  )
{
    if (tu.p_type == UNIT_TEST_CASE)
        messages.testStarted(tu.p_name, flowId);
    else
        messages.suiteStarted(tu.p_name, flowId);

    currentDetails.clear();
}

void TeamcityBoostLogFormatter::test_unit_finish(
    std::ostream& /*out*/
  , const boost::unit_test::test_unit& tu
  , const unsigned long elapsed
  )
{
    const boost::unit_test::test_results& tr = boost::unit_test::results_collector.results(tu.p_id);
    if (tu.p_type == UNIT_TEST_CASE)
    {
        if (!tr.passed())
        {
            if (tr.p_skipped)
                messages.testIgnored(tu.p_name, "ignored", flowId);
            else if (tr.p_aborted)
                messages.testFailed(tu.p_name, "aborted", currentDetails, flowId);
            else
                messages.testFailed(tu.p_name, "failed", currentDetails, flowId);

            if (!currentContextDetails.empty())
            {
                messages.testOutput(
                    tu.p_name
                  , currentContextDetails
                  , flowId
                  , TeamcityMessages::StdErr
                  );
                currentContextDetails.clear();
            }
        }

        messages.testFinished(tu.p_name, elapsed / 1000, flowId);
    }
    else
    {
        messages.suiteFinished(tu.p_name, flowId);
    }
}

void TeamcityBoostLogFormatter::log_entry_start(
    std::ostream& /*out*/
  , const boost::unit_test::log_entry_data& entry_data
  , log_entry_types /*let*/
  )
{
    std::stringstream ss(std::ios_base::out);
    ss  << entry_data.m_file_name << ':' << entry_data.m_line_num << ": ";
    currentDetails += ss.str();
}

void TeamcityBoostLogFormatter::log_entry_value(
    std::ostream& /*out*/
  , boost::unit_test::const_string value
  )
{
    currentDetails += toString(value);
}

void TeamcityBoostLogFormatter::log_entry_finish(std::ostream& /*out*/)
{
    currentDetails += '\n';
}

#if BOOST_VERSION < 105900

void TeamcityBoostLogFormatter::log_exception(
    std::ostream& out
  , const boost::unit_test::log_checkpoint_data&
  , boost::unit_test::const_string explanation
  )
{
    currentDetails += toString(explanation) + '\n';
}

void TeamcityBoostLogFormatter::test_unit_skipped(
    std::ostream& /*out*/
  , const boost::unit_test::test_unit& tu
  )
{
    messages.testIgnored(tu.p_name, "test ignored", flowId);
}

#else                                                       // BOOST_VERSION >= 105900

void TeamcityBoostLogFormatter::log_exception_start(
    std::ostream& /*out*/
  , const boost::unit_test::log_checkpoint_data& cp
  , const boost::execution_exception& excpt
  )
{
    const std::string what = toString(excpt);
    currentDetails += what + '\n';
    if (!cp.m_file_name.empty())
    {
        std::stringstream ss(std::ios_base::out);
        ss << "Last checkpoint at " << cp.m_file_name << ':' << cp.m_line_num;
        if (!cp.m_message.empty())
            ss << ": " << cp.m_message;
        currentDetails += ss.str();
    }
}

void TeamcityBoostLogFormatter::log_exception_finish(std::ostream& /*out*/)
{
}

void TeamcityBoostLogFormatter::test_unit_skipped(
    std::ostream& /*out*/
  , const boost::unit_test::test_unit& tu
  , boost::unit_test::const_string reason
  )
{
    messages.testIgnored(tu.p_name, toString(reason), flowId);
}

void TeamcityBoostLogFormatter::entry_context_start(
    std::ostream& /*out*/
  , boost::unit_test::log_level l
  )
{
    const std::string& initial_msg = (l == boost::unit_test::log_successful_tests ? ASSERT_CTX : FAILURE_CTX);
    currentContextDetails = initial_msg;
}

# if BOOST_VERSION < 106500
void TeamcityBoostLogFormatter::log_entry_context(
    std::ostream& /*out*/
  , boost::unit_test::const_string ctx
  )
# else                                                      // BOOST_VERSION >= 106500
void TeamcityBoostLogFormatter::log_entry_context(
    std::ostream& /*out*/
  , boost::unit_test::log_level
  , boost::unit_test::const_string ctx
  )
# endif                                                     // BOOST_VERSION >= 106500
{
    currentContextDetails += '\n' + toString(ctx);
}

# if BOOST_VERSION < 106500
void TeamcityBoostLogFormatter::entry_context_finish(std::ostream& /*out*/)
# else                                                      // BOOST_VERSION >= 106500
void TeamcityBoostLogFormatter::entry_context_finish(std::ostream& /*out*/, boost::unit_test::log_level)
# endif                                                     // BOOST_VERSION >= 106500
{
}

#endif                                                      // BOOST_VERSION >= 105900
}}                                                          // namespace teamcity, jetbrains
