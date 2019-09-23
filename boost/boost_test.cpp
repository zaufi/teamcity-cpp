#define BOOST_TEST_MAIN

#include <boost/version.hpp>
/// \attention Some Boost UTF headers (of some versions) are
/// *NOT self-contained*, so be aware about implicit heder
/// dependencies...
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_log.hpp>                     // IWYU pragma: keep
#include <boost/test/framework.hpp>                         // IWYU pragma: keep
#if BOOST_VERSION >= 105900
# include <boost/test/data/test_case.hpp>
# include <boost/test/data/monomorphic.hpp>                 // IWYU pragma: keep
#endif                                                      // BOOST_VERSION >= 105900

#include <iostream>
#include <stdexcept>

/* Suite tree tests */
BOOST_AUTO_TEST_SUITE(my_suite1)

BOOST_AUTO_TEST_CASE(my_test1)
{
    BOOST_CHECK(2 == 1);
}

BOOST_AUTO_TEST_CASE(my_test2)
{
    int i = 0;

    BOOST_CHECK_EQUAL(i, 2);
    BOOST_CHECK_EQUAL(i, 0);
}

BOOST_AUTO_TEST_SUITE_END()                                 // my_suite1

BOOST_AUTO_TEST_CASE(my_test3)
{
    int i = 0;

    BOOST_CHECK_EQUAL(i, 0);
}

BOOST_AUTO_TEST_SUITE(my_suite2)

BOOST_AUTO_TEST_CASE(my_test4)
{
    int i = 0;

    BOOST_CHECK_EQUAL(i, 1);
}

BOOST_AUTO_TEST_SUITE(internal_suite)

BOOST_AUTO_TEST_CASE(my_test5)
{
    int i = 0;

    BOOST_CHECK_EQUAL(i, 1);
}

BOOST_AUTO_TEST_SUITE_END()                                 // internal_suite
BOOST_AUTO_TEST_SUITE_END()                                 // my_suite2

/* Other */
BOOST_AUTO_TEST_CASE(testCerr)
{
    std::cerr << "Hello from cerr" << std::endl;
}

BOOST_AUTO_TEST_CASE(testCout)
{
    std::cout << "Hello from cout" << std::endl;
}

namespace {

void ThrowRuntimeError()
{
    throw std::runtime_error("runtime exception text");
}

}                                                           // anonymous namespace

BOOST_AUTO_TEST_CASE(testException)
{
    ThrowRuntimeError();
}

BOOST_AUTO_TEST_CASE(testAssertExceptionGood)
{
    BOOST_CHECK_THROW(ThrowRuntimeError(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(testAssertExceptionFail)
{
    BOOST_CHECK_THROW(ThrowRuntimeError(), std::logic_error);
}

BOOST_AUTO_TEST_CASE(testFatal)
{
    BOOST_FAIL("bfail");
}

BOOST_AUTO_TEST_CASE(testError)
{
    BOOST_ERROR("berror");
}

BOOST_AUTO_TEST_CASE(testNothing)
{
    BOOST_TEST_MESSAGE("This test doesn't check anything");
}

BOOST_AUTO_TEST_CASE(skip_this_test)
{
    boost::unit_test::unit_test_log.test_unit_skipped(
        boost::unit_test_framework::framework::current_test_case()
#if BOOST_VERSION >= 105900
      , "This test is ignored"
#endif                                                      // BOOST_VERSION >= 105900
      );
    BOOST_CHECK(true);
}

#if BOOST_VERSION >= 105900
BOOST_DATA_TEST_CASE(test_case_arity1, boost::unit_test_framework::data::xrange(5), var)
{
    BOOST_TEST((var <= 4 && var >= 1));
}

BOOST_AUTO_TEST_CASE(dependencyFailTest, *boost::unit_test::depends_on("my_suite1/my_test1"))
{
    BOOST_FAIL("dependency failed");
}

BOOST_AUTO_TEST_CASE(dependencySkippedTest, *boost::unit_test::depends_on("skip_this_test"))
{
    BOOST_TEST(1 != 1);
}

BOOST_AUTO_TEST_CASE(dependencyOkTest, *boost::unit_test::depends_on("testNothing"))
{
    BOOST_TEST(true);
}

// NOTE The following tests were added on migration to Boost 1.70
# if BOOST_VERSION >= 107000

BOOST_AUTO_TEST_CASE(checkpointTest)
{
    BOOST_TEST_CHECKPOINT("This is checkpointTest");
    BOOST_TEST(1 != 1);
    throw std::runtime_error("Undefined Behaviour ahead!");
}

BOOST_AUTO_TEST_CASE(contextsTest)
{
    BOOST_TEST_INFO("This is contextsTest");
    BOOST_TEST(false, "Intentionally failed test");

    BOOST_TEST_CONTEXT("Alpha")
    {
        BOOST_TEST(1 != 1);

        BOOST_TEST_CONTEXT("Be" << "ta")
        BOOST_TEST(3 != 3);

        BOOST_TEST(4 == 4);
    }

    BOOST_TEST(5 != 5);
}
# endif                                                     // BOOST_VERSION >= 107000
#endif                                                      // BOOST_VERSION >= 105900
