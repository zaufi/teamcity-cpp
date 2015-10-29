/* Copyright 2011 JetBrains s.r.o.
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

#include <cstdlib>
#include <sstream>

namespace jetbrains { namespace teamcity { namespace {
/// Use RAII to write message open/close markers
class RaiiMessage
{
public:
    RaiiMessage(const char* const name, std::ostream* const out)
      : m_out(out)
    {
        // endl for http://jetbrains.net/tracker/issue/TW-4412
        *m_out << '\n' << "##teamcity[" << name;
    }
    ~RaiiMessage()
    {
        // endl for http://jetbrains.net/tracker/issue/TW-4412
        *m_out << "]\n";
    }

private:
    std::ostream* m_out;
};
}                                                           // anonymous namespace

std::string getFlowIdFromEnvironment()
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    char* flowId = 0;
    size_t sz = 0;
    std::string result;
    if (!_dupenv_s(&flowId, &sz, "TEAMCITY_PROCESS_FLOW_ID"))
    {
        result = flowId != 0 ? flowId : std::string();
        free(flowId);
    }

    return result;
#else
    const char *flowId = getenv("TEAMCITY_PROCESS_FLOW_ID");
    return flowId == 0 ? std::string() : flowId;
#endif
}

bool underTeamcity()
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    char* teamCityProjectName = 0;
    size_t sz = 0;
    bool result = false;
    if (!_dupenv_s(&teamCityProjectName, &sz, "TEAMCITY_PROJECT_NAME"))
    {
        result = teamCityProjectName != 0;
        free(teamCityProjectName);
    }

    return result;
#else
    return getenv("TEAMCITY_PROJECT_NAME") != 0;
#endif
}

TeamcityMessages::TeamcityMessages()
  : m_out(&std::cout)
{}

void TeamcityMessages::setOutput(std::ostream& out)
{
    m_out = &out;
}

/// \todo Copying char-by-char is ineffective!
std::string TeamcityMessages::escape(std::string s)
{
    std::string result;

    for (size_t i = 0; i < s.length(); i++)
    {
        char c = s[i];

        switch (c)
        {
        case '\n': result.append("|n"); break;
        case '\r': result.append("|r"); break;
        case '\'': result.append("|'"); break;
        case '|':  result.append("||"); break;
        case ']':  result.append("|]"); break;
        default:   result.append(&c, 1);
        }
    }

    return result;
}

void TeamcityMessages::writeProperty(const char* const name, std::string value, const bool ifNonEmpty)
{
    if (ifNonEmpty || !value.empty())
        *m_out << ' ' << name << "='" << escape(value) << '\'';
}

void TeamcityMessages::suiteStarted(std::string name, std::string flowid)
{
    RaiiMessage msg("testSuiteStarted", m_out);
    writeProperty("name", name);
    writeProperty("flowId", flowid);
}

void TeamcityMessages::suiteFinished(std::string name, std::string flowid)
{
    RaiiMessage msg("testSuiteFinished", m_out);
    writeProperty("name", name);
    writeProperty("flowId", flowid);
}

void TeamcityMessages::testStarted(std::string name, std::string flowid, const bool captureStandardOutput)
{
    RaiiMessage msg("testStarted", m_out);
    writeProperty("name", name);
    writeProperty("flowId", flowid);

    if (captureStandardOutput)
        writeProperty("captureStandardOutput", "true");     // false by default
}

void TeamcityMessages::testFinished(std::string name, const int durationMs, std::string flowid)
{
    RaiiMessage msg("testFinished", m_out);

    writeProperty("name", name);
    writeProperty("flowId", flowid);

    if (durationMs >= 0)
    {
        /// \bug W/ some locales it is possible to get a number w/ a number separator(s)!
        /// \todo Make a test for that!
        std::stringstream out(std::ios_base::out);
        out << durationMs;
        writeProperty("duration", out.str());
    }
}

void TeamcityMessages::testFailed(std::string name, std::string message, std::string details, std::string flowid)
{
    RaiiMessage msg("testFailed", m_out);
    writeProperty("name", name);
    writeProperty("message", message);
    writeProperty("details", details);
    writeProperty("flowId", flowid);
}

void TeamcityMessages::testIgnored(std::string name, std::string message, std::string flowid)
{
    RaiiMessage msg("testIgnored", m_out);
    writeProperty("name", name);
    writeProperty("message", message);
    writeProperty("flowId", flowid);
}

void TeamcityMessages::testOutput(std::string name, std::string output, std::string flowid, bool isStdError)
{
    RaiiMessage msg(isStdError ? "testStdErr" : "testStdOut", m_out);
    writeProperty("name", name);
    writeProperty("out", output);
    writeProperty("flowId", flowid);
}

}}                                                          // namespace teamcity, jetbrains
