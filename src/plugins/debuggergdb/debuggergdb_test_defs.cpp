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
    CHECK(IsPointerType(wxT("int*")));
}

TEST(DetectPointerType2)
{
    CHECK(IsPointerType(wxT("int * const")));
    CHECK(IsPointerType(wxT("int *const")));
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
    CHECK(!IsPointerType(wxT("char const *")));
    CHECK(!IsPointerType(wxT("const char *")));


    CHECK(IsPointerType(wxT("char **")));
    CHECK(IsPointerType(wxT("char ****")));
    CHECK(IsPointerType(wxT("char const **")));
    CHECK(IsPointerType(wxT("const char **")));
    CHECK(IsPointerType(wxT("const char * const *")));
}

TEST(DetectPointerType6)
{
    CHECK(!IsPointerType(wxT("wchar_t *")));
    CHECK(!IsPointerType(wxT("const wchar_t *")));
    CHECK(IsPointerType(wxT("wchar_t ****")));
    CHECK(IsPointerType(wxT("wchar_t const ****")));
}

TEST(DetectPointerType7)
{
    CHECK(IsPointerType(wxT("random_char_t *")));
    CHECK(IsPointerType(wxT("const random_char_t *")));
}

TEST(DetectPointerType8)
{
    CHECK(!IsPointerType(wxT("std::vector<int*, std::allocator<int*> >")));
    CHECK(!IsPointerType(wxT("std::vector<int*, std::allocator<int*>>")));
    CHECK(!IsPointerType(wxT("std::vector< int *, std::allocator< int * > >")));
    CHECK(IsPointerType(wxT("std::vector< int *, std::allocator< int * > >*")));
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
    CHECK(IsPointerType(wxT("char ** restrict")));
}

TEST(DetectPointerTypeRestrict5)
{
    CHECK(!IsPointerType(wxT("char const * restrict")));
    CHECK(!IsPointerType(wxT("const char * restrict")));
    CHECK(IsPointerType(wxT("char const ** restrict")));
    CHECK(IsPointerType(wxT("const char ** restrict")));
}

TEST(DetectRefTypeRestrict)
{
    CHECK(!IsPointerType(wxT("int & restrict")));
    CHECK(!IsPointerType(wxT("const int & restrict")));
    CHECK(!IsPointerType(wxT("volatile int & restrict")));
    CHECK(!IsPointerType(wxT("const volatile int & restrict")));
}

}
