#include "debuggergdb_test_common.h"

#include "gdb_commands.h"

SUITE(GDBStackFrameParser)
{

TEST(match0_flag)
{
    cbStackFrame sf;
    bool hasLineInfo;
    CHECK(GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#0 wxEntry () at main.cpp:5")));
    CHECK(hasLineInfo);
}
TEST(match0_number)
{
    cbStackFrame sf;
    bool hasLineInfo;
    GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#0 wxEntry () at main.cpp:5"));
    CHECK_EQUAL(0, sf.GetNumber());
}
TEST(match0_symbol)
{
    cbStackFrame sf;
    bool hasLineInfo;
    GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#0 wxEntry () at main.cpp:5"));
    CHECK_EQUAL(wxT("wxEntry ()"), sf.GetSymbol());
}
TEST(match0_file)
{
    cbStackFrame sf;
    bool hasLineInfo;
    GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#0 wxEntry () at main.cpp:5"));
    CHECK_EQUAL(wxT("main.cpp"), sf.GetFilename());
    CHECK_EQUAL(wxT("5"), sf.GetLine());
}

//#8  0x77d48734 in USER32!GetDC () from C:\\WINDOWS\\system32\\user32.dll
TEST(match1_flag)
{
    cbStackFrame sf;
    bool hasLineInfo;
    CHECK(GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#8  0x77d48734 in USER32!GetDC () from C:\\WINDOWS\\system32\\user32.dll")));
    CHECK(!hasLineInfo);
}
TEST(match1_number)
{
    cbStackFrame sf;
    bool hasLineInfo;
    GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#8  0x77d48734 in USER32!GetDC () from C:\\WINDOWS\\system32\\user32.dll"));
    CHECK_EQUAL(8, sf.GetNumber());
}
TEST(match1_address)
{
    cbStackFrame sf;
    bool hasLineInfo;
    GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#8  0x77d48734 in USER32!GetDC () from C:\\WINDOWS\\system32\\user32.dll"));
    CHECK_EQUAL(0x77d48734ul, sf.GetAddress());
}
TEST(match1_symbol)
{
    cbStackFrame sf;
    bool hasLineInfo;
    GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#8  0x77d48734 in USER32!GetDC () from C:\\WINDOWS\\system32\\user32.dll"));
    CHECK_EQUAL(wxT("USER32!GetDC()"), sf.GetSymbol());
}
TEST(match1_file)
{
    cbStackFrame sf;
    bool hasLineInfo;
    GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#8  0x77d48734 in USER32!GetDC () from C:\\WINDOWS\\system32\\user32.dll"));
    CHECK_EQUAL(wxT("C:\\WINDOWS\\system32\\user32.dll"), sf.GetFilename());
    CHECK_EQUAL(wxEmptyString, sf.GetLine());
}

//#9  0x001b04fe in ?? ()
//#11  0x00406810 in main ()
TEST(match2_flag)
{
    cbStackFrame sf;
    bool hasLineInfo;
    CHECK(GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#9  0x001b04fe in ?? ()")));
    CHECK(!hasLineInfo);
}
TEST(match2_number)
{
    cbStackFrame sf;
    bool hasLineInfo;
    GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#9  0x001b04fe in ?? ()"));
    CHECK_EQUAL(9, sf.GetNumber());
}
TEST(match2_address)
{
    cbStackFrame sf;
    bool hasLineInfo;
    GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#9  0x001b04fe in ?? ()"));
    CHECK_EQUAL(0x001b04feul, sf.GetAddress());
}
TEST(match2_symbol)
{
    cbStackFrame sf;
    bool hasLineInfo;
    GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#9  0x001b04fe in ?? ()"));
    CHECK_EQUAL(wxT("?? ()"), sf.GetSymbol());
}
TEST(match2_file)
{
    cbStackFrame sf;
    bool hasLineInfo;
    GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#9  0x001b04fe in ?? ()"));
    CHECK_EQUAL(wxEmptyString, sf.GetFilename());
    CHECK_EQUAL(wxEmptyString, sf.GetLine());
}

//#30 0x00403c0a in WinMain (hInstance=0x400000, hPrevInstance=0x0, lpCmdLine=0x241ef9 "", nCmdShow=10) at C:/Devel/wxSmithTest/app.cpp:297
TEST(match3_flag)
{
    cbStackFrame sf;
    bool hasLineInfo;
    CHECK(GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#30 0x00403c0a in WinMain (hInstance=0x400000, hPrevInstance=0x0, lpCmdLine=0x241ef9 "", nCmdShow=10) at C:/Devel/wxSmithTest/app.cpp:297")));
    CHECK(hasLineInfo);
}
TEST(match3_number)
{
    cbStackFrame sf;
    bool hasLineInfo;
    GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#30 0x00403c0a in WinMain (hInstance=0x400000, hPrevInstance=0x0, lpCmdLine=0x241ef9 "", nCmdShow=10) at C:/Devel/wxSmithTest/app.cpp:297"));
    CHECK_EQUAL(30, sf.GetNumber());
}
TEST(match3_address)
{
    cbStackFrame sf;
    bool hasLineInfo;
    GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#30 0x00403c0a in WinMain (hInstance=0x400000, hPrevInstance=0x0, lpCmdLine=0x241ef9 "", nCmdShow=10) at C:/Devel/wxSmithTest/app.cpp:297"));
    CHECK_EQUAL(0x00403c0aul, sf.GetAddress());
}
TEST(match3_symbol)
{
    cbStackFrame sf;
    bool hasLineInfo;
    GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#30 0x00403c0a in WinMain (hInstance=0x400000, hPrevInstance=0x0, lpCmdLine=0x241ef9 "", nCmdShow=10) at C:/Devel/wxSmithTest/app.cpp:297"));
    CHECK_EQUAL(wxT("WinMain(hInstance=0x400000, hPrevInstance=0x0, lpCmdLine=0x241ef9 "", nCmdShow=10)"), sf.GetSymbol());
}
TEST(match3_file)
{
    cbStackFrame sf;
    bool hasLineInfo;
    GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#30 0x00403c0a in WinMain (hInstance=0x400000, hPrevInstance=0x0, lpCmdLine=0x241ef9 "", nCmdShow=10) at C:/Devel/wxSmithTest/app.cpp:297"));
    CHECK_EQUAL(wxT("C:/Devel/wxSmithTest/app.cpp"), sf.GetFilename());
    CHECK_EQUAL(wxT("297"), sf.GetLine());
}

//#31 0x004076ca in main () at C:/Devel/wxWidgets-2.6.1/include/wx/intl.h:555
TEST(match4_flag)
{
    cbStackFrame sf;
    bool hasLineInfo;
    CHECK(GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#31 0x004076ca in main () at C:/Devel/wxWidgets-2.6.1/include/wx/intl.h:555")));
    CHECK(hasLineInfo);
}
TEST(match4_number)
{
    cbStackFrame sf;
    bool hasLineInfo;
    GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#31 0x004076ca in main () at C:/Devel/wxWidgets-2.6.1/include/wx/intl.h:555"));
    CHECK_EQUAL(31, sf.GetNumber());
}
TEST(match4_address)
{
    cbStackFrame sf;
    bool hasLineInfo;
    GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#31 0x004076ca in main () at C:/Devel/wxWidgets-2.6.1/include/wx/intl.h:555"));
    CHECK_EQUAL(0x004076caul, sf.GetAddress());
}
TEST(match4_symbol)
{
    cbStackFrame sf;
    bool hasLineInfo;
    GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#31 0x004076ca in main () at C:/Devel/wxWidgets-2.6.1/include/wx/intl.h:555"));
    CHECK_EQUAL(wxT("main()"), sf.GetSymbol());
}
TEST(match4_file)
{
    cbStackFrame sf;
    bool hasLineInfo;
    GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#31 0x004076ca in main () at C:/Devel/wxWidgets-2.6.1/include/wx/intl.h:555"));
    CHECK_EQUAL(wxT("C:/Devel/wxWidgets-2.6.1/include/wx/intl.h"), sf.GetFilename());
    CHECK_EQUAL(wxT("555"), sf.GetLine());
}

//#50  0x00410c8c in one::~one() (this=0x3d24c8) at main.cpp:14
TEST(match5_flag)
{
    cbStackFrame sf;
    bool hasLineInfo;
    CHECK(GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#50  0x00410c8c in one::~one() (this=0x3d24c8) at main.cpp:14")));
    CHECK(hasLineInfo);
}
TEST(match5_number)
{
    cbStackFrame sf;
    bool hasLineInfo;
    GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#50  0x00410c8c in one::~one() (this=0x3d24c8) at main.cpp:14"));
    CHECK_EQUAL(50, sf.GetNumber());
}
TEST(match5_address)
{
    cbStackFrame sf;
    bool hasLineInfo;
    GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#50  0x00410c8c in one::~one() (this=0x3d24c8) at main.cpp:14"));
    CHECK_EQUAL(0x00410c8cul, sf.GetAddress());
}
TEST(match5_symbol)
{
    cbStackFrame sf;
    bool hasLineInfo;
    GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#50  0x00410c8c in one::~one() (this=0x3d24c8) at main.cpp:14"));
    CHECK_EQUAL(wxT("one::~one() (this=0x3d24c8)"), sf.GetSymbol());
}
TEST(match5_file)
{
    cbStackFrame sf;
    bool hasLineInfo;
    GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#50  0x00410c8c in one::~one() (this=0x3d24c8) at main.cpp:14"));
    CHECK_EQUAL(wxT("main.cpp"), sf.GetFilename());
    CHECK_EQUAL(wxT("14"), sf.GetLine());
}

// #0  Foo::Bar::(anonymous namespace)::apply (this=0xbaf6cf0, stmt=0xb9d7160, stmtSubsts=...) at Foo/Bar/apply.cpp:219
TEST(match6_flag)
{
    cbStackFrame sf;
    bool hasLineInfo;
    CHECK(GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#0  Foo::Bar::(anonymous namespace)::apply (this=0xbaf6cf0")
                                                       wxT(", stmt=0xb9d7160, stmtSubsts=...) ")
                                                       wxT("at Foo/Bar/apply.cpp:219")));
    CHECK(hasLineInfo);
}
TEST(match6_number)
{
    cbStackFrame sf;
    bool hasLineInfo;
    CHECK(GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#0  Foo::Bar::(anonymous namespace)::apply (this=0xbaf6cf0")
                                                       wxT(", stmt=0xb9d7160, stmtSubsts=...) ")
                                                       wxT("at Foo/Bar/apply.cpp:219")));
    CHECK_EQUAL(0, sf.GetNumber());
}
TEST(match6_address)
{
    cbStackFrame sf;
    bool hasLineInfo;
    CHECK(GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#0  Foo::Bar::(anonymous namespace)::apply (this=0xbaf6cf0")
                                                       wxT(", stmt=0xb9d7160, stmtSubsts=...) ")
                                                       wxT("at Foo/Bar/apply.cpp:219")));
    CHECK_EQUAL(0x0, sf.GetAddress());
}
TEST(match6_symbol)
{
    cbStackFrame sf;
    bool hasLineInfo;
    CHECK(GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#0  Foo::Bar::(anonymous namespace)::apply (this=0xbaf6cf0")
                                                       wxT(", stmt=0xb9d7160, stmtSubsts=...) ")
                                                       wxT("at Foo/Bar/apply.cpp:219")));
    CHECK_EQUAL(wxT("Foo::Bar::(anonymous namespace)::apply (this=0xbaf6cf0, stmt=0xb9d7160, stmtSubsts=...)"),
                sf.GetSymbol());
}
TEST(match6_file)
{
    cbStackFrame sf;
    bool hasLineInfo;
    CHECK(GdbCmd_Backtrace::MatchLine(sf, hasLineInfo, wxT("#0  Foo::Bar::(anonymous namespace)::apply (this=0xbaf6cf0")
                                                       wxT(", stmt=0xb9d7160, stmtSubsts=...) ")
                                                       wxT("at Foo/Bar/apply.cpp:219")));
    CHECK_EQUAL(wxT("Foo/Bar/apply.cpp"), sf.GetFilename());
    CHECK_EQUAL(wxT("219"), sf.GetLine());
}

} // SUITE(GDBStackFrameParser)
