#include "debuggergdb_test_common.h"
#include "debugger_defs.h"

SUITE(DebugDefs)
{
TEST(DetectPointerType0)
{
    CHECK(IsPointerType(wxT("int *")));
}

TEST(DetectPointerType1)
{
    CHECK(!IsPointerType(wxT("int")));
}

TEST(DetectPointerType2)
{
    CHECK(IsPointerType(wxT("int * const")));
}

TEST(DetectPointerType3)
{
    CHECK(IsPointerType(wxT("int * volatile")));
}

TEST(DetectPointerType4)
{
    CHECK(!IsPointerType(wxT("char *")));
}

TEST(DetectPointerType5)
{
    CHECK(!IsPointerType(wxT("char const *")));
}


}
