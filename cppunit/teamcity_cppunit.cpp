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

#include "teamcity_cppunit.h"

#include "cppunit/Test.h"
#include "cppunit/Exception.h"

#include <sstream>

using namespace CPPUNIT_NS;

namespace jetbrains { namespace teamcity { namespace {
std::string sourceLine2string(const SourceLine &sline)
{
    std::stringstream ss;

    ss << sline.fileName() << ":" << sline.lineNumber();

    return ss.str();
}
}                                                           // anonymous namespace

TeamcityProgressListener::TeamcityProgressListener()
  : flowid(getFlowIdFromEnvironment())
{
}

TeamcityProgressListener::TeamcityProgressListener(const std::string& id)
  : flowid(id)
{
}

TeamcityProgressListener::~TeamcityProgressListener()
{}

void TeamcityProgressListener::startTest(Test *test)
{
    messages.testStarted(test->getName(), flowid);
}

void TeamcityProgressListener::addFailure(const TestFailure &failure)
{
    const Exception *e = failure.thrownException();

    std::string details = e->message().details();

    if (e->sourceLine().isValid())
    {
        details.append(" at ");
        details.append(sourceLine2string(e->sourceLine()));
        details.append("\n");
    }

    messages.testFailed(
        failure.failedTest()->getName()
      , e->message().shortDescription()
      , details
      , flowid
      );
}

void TeamcityProgressListener::endTest(Test *test)
{
    messages.testFinished(test->getName(), -1, flowid);
}

void TeamcityProgressListener::startSuite(Test *test)
{
    messages.suiteStarted(test->getName(), flowid);
}

void TeamcityProgressListener::endSuite(Test *test)
{
    messages.suiteFinished(test->getName(), flowid);
}

}}                                                          // namespace teamcity, jetbrains
