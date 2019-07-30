#include "debuggergdb_test_common.h"
#include "debugger_defs.h"

SUITE(DebugDefs)
{
TEST(DetectPointerType0)
{
    CHECK(!IsPointerType(wxT("int")));
}

TEST(DetectPointerType1)
{
    CHECK(IsPointerType(wxT("int *")));
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
    CHECK(IsPointerType(wxT("int * const volatile")));
}

TEST(DetectPointerType5)
{
    CHECK(!IsPointerType(wxT("char *")));
}

TEST(DetectPointerType6)
{
    CHECK(!IsPointerType(wxT("char const *")));
}

TEST(DetectPointerTypeRestrict0)
{
    CHECK(IsPointerType(wxT("int * restrict")));
}

TEST(DetectPointerTypeRestrict1)
{
    CHECK(IsPointerType(wxT("int * const restrict")));
}

TEST(DetectPointerTypeRestrict2)
{
    CHECK(IsPointerType(wxT("int * volatile restrict")));
}

TEST(DetectPointerTypeRestrict3)
{
    CHECK(IsPointerType(wxT("int * const volatile restrict")));
}

TEST(DetectPointerTypeRestrict4)
{
    CHECK(!IsPointerType(wxT("char * restrict")));
}

TEST(DetectPointerTypeRestrict5)
{
    CHECK(!IsPointerType(wxT("char const * restrict")));
}

}
