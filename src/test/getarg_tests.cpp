#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-YRC");
    BOOST_CHECK(GetBoolArg("-YRC"));
    BOOST_CHECK(GetBoolArg("-YRC", false));
    BOOST_CHECK(GetBoolArg("-YRC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-YRCo"));
    BOOST_CHECK(!GetBoolArg("-YRCo", false));
    BOOST_CHECK(GetBoolArg("-YRCo", true));

    ResetArgs("-YRC=0");
    BOOST_CHECK(!GetBoolArg("-YRC"));
    BOOST_CHECK(!GetBoolArg("-YRC", false));
    BOOST_CHECK(!GetBoolArg("-YRC", true));

    ResetArgs("-YRC=1");
    BOOST_CHECK(GetBoolArg("-YRC"));
    BOOST_CHECK(GetBoolArg("-YRC", false));
    BOOST_CHECK(GetBoolArg("-YRC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noYRC");
    BOOST_CHECK(!GetBoolArg("-YRC"));
    BOOST_CHECK(!GetBoolArg("-YRC", false));
    BOOST_CHECK(!GetBoolArg("-YRC", true));

    ResetArgs("-noYRC=1");
    BOOST_CHECK(!GetBoolArg("-YRC"));
    BOOST_CHECK(!GetBoolArg("-YRC", false));
    BOOST_CHECK(!GetBoolArg("-YRC", true));

    ResetArgs("-YRC -noYRC");  // -YRC should win
    BOOST_CHECK(GetBoolArg("-YRC"));
    BOOST_CHECK(GetBoolArg("-YRC", false));
    BOOST_CHECK(GetBoolArg("-YRC", true));

    ResetArgs("-YRC=1 -noYRC=1");  // -YRC should win
    BOOST_CHECK(GetBoolArg("-YRC"));
    BOOST_CHECK(GetBoolArg("-YRC", false));
    BOOST_CHECK(GetBoolArg("-YRC", true));

    ResetArgs("-YRC=0 -noYRC=0");  // -YRC should win
    BOOST_CHECK(!GetBoolArg("-YRC"));
    BOOST_CHECK(!GetBoolArg("-YRC", false));
    BOOST_CHECK(!GetBoolArg("-YRC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--YRC=1");
    BOOST_CHECK(GetBoolArg("-YRC"));
    BOOST_CHECK(GetBoolArg("-YRC", false));
    BOOST_CHECK(GetBoolArg("-YRC", true));

    ResetArgs("--noYRC=1");
    BOOST_CHECK(!GetBoolArg("-YRC"));
    BOOST_CHECK(!GetBoolArg("-YRC", false));
    BOOST_CHECK(!GetBoolArg("-YRC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-YRC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-YRC", "eleven"), "eleven");

    ResetArgs("-YRC -bar");
    BOOST_CHECK_EQUAL(GetArg("-YRC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-YRC", "eleven"), "");

    ResetArgs("-YRC=");
    BOOST_CHECK_EQUAL(GetArg("-YRC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-YRC", "eleven"), "");

    ResetArgs("-YRC=11");
    BOOST_CHECK_EQUAL(GetArg("-YRC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-YRC", "eleven"), "11");

    ResetArgs("-YRC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-YRC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-YRC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-YRC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-YRC", 0), 0);

    ResetArgs("-YRC -bar");
    BOOST_CHECK_EQUAL(GetArg("-YRC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-YRC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-YRC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-YRC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-YRC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--YRC");
    BOOST_CHECK_EQUAL(GetBoolArg("-YRC"), true);

    ResetArgs("--YRC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-YRC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noYRC");
    BOOST_CHECK(!GetBoolArg("-YRC"));
    BOOST_CHECK(!GetBoolArg("-YRC", true));
    BOOST_CHECK(!GetBoolArg("-YRC", false));

    ResetArgs("-noYRC=1");
    BOOST_CHECK(!GetBoolArg("-YRC"));
    BOOST_CHECK(!GetBoolArg("-YRC", true));
    BOOST_CHECK(!GetBoolArg("-YRC", false));

    ResetArgs("-noYRC=0");
    BOOST_CHECK(GetBoolArg("-YRC"));
    BOOST_CHECK(GetBoolArg("-YRC", true));
    BOOST_CHECK(GetBoolArg("-YRC", false));

    ResetArgs("-YRC --noYRC");
    BOOST_CHECK(GetBoolArg("-YRC"));

    ResetArgs("-noYRC -YRC"); // YRC always wins:
    BOOST_CHECK(GetBoolArg("-YRC"));
}

BOOST_AUTO_TEST_SUITE_END()
