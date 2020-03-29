#include "debuggergdb_test_common.h"

#include "parsewatchvalue.h"

SUITE(CDBWatchParser)
{

TEST(CDBSingleLineStructPointer)
{
    cb::shared_ptr<GDBWatch> w(new GDBWatch(wxT("t")));
    CHECK(ParseCDBWatchValue(w, wxT("struct t * 0x123456")));
    CHECK_EQUAL(wxT("t=0x123456"), *w);
}

TEST(CDBSingleLineClassPointer)
{
    cb::shared_ptr<GDBWatch> w(new GDBWatch(wxT("t")));
    CHECK(ParseCDBWatchValue(w, wxT("class t * 0x123456")));
    CHECK_EQUAL(wxT("t=0x123456"), *w);
}

} // SUITE(CDBWatchParser)

