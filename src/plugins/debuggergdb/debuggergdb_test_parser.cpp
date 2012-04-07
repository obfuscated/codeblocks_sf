#include "debuggergdb_test_common.h"
#include "parsewatchvalue.h"

wxString WatchToString(cbWatch const &watch)
{
    wxString s;
    watch.GetSymbol(s);
    s += wxT("=");

    wxString value;
    watch.GetValue(value);
    s += value;

    if (watch.GetChildCount() > 0)
    {
        s += wxT(" {");
        s += WatchToString(*watch.GetChild(0));

        for (int ii = 1; ii < watch.GetChildCount(); ++ii)
        {
            s += wxT(",");
            s += WatchToString(*watch.GetChild(ii));
        }

        s += wxT("}");
    }

    return s;
}

std::ostream& operator<<(std::ostream &stream, cbWatch const &w)
{
    return stream << WatchToString(w);
}

bool operator == (wxString const &s, cbWatch const &w)
{
    return s == WatchToString(w);
}

SUITE(WatchToString)
{

GDBWatch::Pointer MakeWatch(wxString const &symbol, wxString const &value)
{
    GDBWatch::Pointer w(new GDBWatch(symbol));
    w->SetValue(value);
    return w;
}

TEST(Simple)
{
    GDBWatch w(wxT("a"));
    w.SetValue(wxT("\"5\""));

    CHECK_EQUAL(wxT("a=\"5\""), w);
}

TEST(SimpleChildren)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("a")));
    cbWatch::AddChild(w, MakeWatch(wxT("b"), wxT("\"5\"")));
    cbWatch::AddChild(w, MakeWatch(wxT("c"), wxT("\"6\"")));

    CHECK_EQUAL(wxT("a= {b=\"5\",c=\"6\"}"), *w);
}

TEST(SimpleChildrenValue)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("a")));
    w->SetValue(wxT("\"value\""));
    cbWatch::AddChild(w, MakeWatch(wxT("b"), wxT("\"5\"")));
    cbWatch::AddChild(w, MakeWatch(wxT("c"), wxT("\"6\"")));

    CHECK_EQUAL(wxT("a=\"value\" {b=\"5\",c=\"6\"}"), *w);
}

TEST(ComplexChildren)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("a")));

    GDBWatch::Pointer c(new GDBWatch(wxT("b")));
    cbWatch::AddChild(c, MakeWatch(wxT("b1"), wxT("\"5\"")));
    cbWatch::AddChild(c, MakeWatch(wxT("b2"), wxT("\"6\"")));
    cbWatch::AddChild(w, c);

    c = GDBWatch::Pointer(new GDBWatch(wxT("c")));
    cbWatch::AddChild(c, MakeWatch(wxT("c1"), wxT("\"5\"")));
    cbWatch::AddChild(c, MakeWatch(wxT("c2"), wxT("\"6\"")));
    cbWatch::AddChild(w, c);

    CHECK_EQUAL(wxT("a= {b= {b1=\"5\",b2=\"6\"},c= {c1=\"5\",c2=\"6\"}}"), *w);
}

TEST(ComplexChildrenValue)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("a")));
    w->SetValue(wxT("\"valueA\""));
    GDBWatch::Pointer c(new GDBWatch(wxT("b")));
    c->SetValue(wxT("\"valueB\""));
    cbWatch::AddChild(c, MakeWatch(wxT("b1"), wxT("\"5\"")));
    cbWatch::AddChild(c, MakeWatch(wxT("b2"), wxT("\"6\"")));
    cbWatch::AddChild(w, c);

    c = GDBWatch::Pointer(new GDBWatch(wxT("c")));
    c->SetValue(wxT("\"valueC\""));
    cbWatch::AddChild(c, MakeWatch(wxT("c1"), wxT("\"5\"")));
    cbWatch::AddChild(c, MakeWatch(wxT("c2"), wxT("\"6\"")));
    cbWatch::AddChild(w, c);

    CHECK_EQUAL(wxT("a=\"valueA\" {b=\"valueB\" {b1=\"5\",b2=\"6\"},c=\"valueC\" {c1=\"5\",c2=\"6\"}}"), *w);
}

}


SUITE(GDBWatchParser)
{

TEST(Simple)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("a")));
    CHECK(ParseGDBWatchValue(w, wxT("0x60d088 \"test_test_test2\"")));
    CHECK_EQUAL(wxT("a=0x60d088 \"test_test_test2\""), *w);
}

TEST(SimpleMembers)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("cmp")));
    CHECK(ParseGDBWatchValue(w, wxT("{\n  a = 5, \n  b = 7, \n  c = 1, \n  d = 2 \n}")));
    CHECK_EQUAL(wxT("cmp= {a=5,b=7,c=1,d=2}"), *w);
}

TEST(SingleInheritance)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("t")));
    CHECK(ParseGDBWatchValue(w, wxT("{\n  <BaseA> = {\n    a = 15,\n    b = 20\n  },\n")
                                wxT("  members of DerivedA:\n  c = 5,\n  d = 10\n}")));
    CHECK_EQUAL(wxT("t= {<BaseA>= {a=15,b=20},c=5,d=10}"), *w);
}

TEST(MultipleInheritance)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("t")));
    CHECK(ParseGDBWatchValue(w, wxT("{\n  <BaseA> = {\n    a = 15,\n    b = 20\n  }, \n")
                                wxT("  <BaseB> = {\n    a = 25,\n    b = 30\n  }, \n")
                                wxT("  members of DerivedAB: \n  e = 5,\n  f = 10\n}")));
    CHECK_EQUAL(wxT("t= {<BaseA>= {a=15,b=20},<BaseB>= {a=25,b=30},e=5,f=10}"), *w);
}

TEST(TemplatedInheritance)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("s")));
    CHECK(ParseGDBWatchValue(w, wxT("{\nmembers of A<Value,Al<Value> >::Impl:\nVV = 0x72dc440 }")));
    CHECK_EQUAL(wxT("s= {VV=0x72dc440}"), *w);
}

TEST(IgnoreWarnings)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("t")));
    CHECK(ParseGDBWatchValue(w, wxT("warning: can't find linker symbol for virtual table for `wxString' value\n")
                                wxT("warning:   found `g_strEmpty' instead\n")
                                wxT("(type) {\n")
                                wxT("warning: can't find linker symbol for virtual table for `wxString' value\n")
                                wxT("warning:   found `g_strEmpty' instead\n")
                                wxT("   a = 5,\n    b = 10\n }")));
    CHECK_EQUAL(wxT("t=(type) {a=5,b=10}"), *w);
}

TEST(NoDataFields)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("t")));
    CHECK(ParseGDBWatchValue(w, wxT("{ \n   field = {<No data fields>} \n}")));
    CHECK_EQUAL(wxT("t= {field= {[0]=<No data fields>}}"), *w);
}

TEST(Empty)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("t")));
    CHECK(ParseGDBWatchValue(w, wxT("{ \n   empty = { \n } \n}")));
    CHECK_EQUAL(wxT("t= {empty=}"), *w);
}

TEST(SimpleArray)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("t")));
    CHECK(ParseGDBWatchValue(w, wxT("{1, 2, 3, 4, 5, 6, 7}")));
    CHECK_EQUAL(wxT("t= {[0]=1,[1]=2,[2]=3,[3]=4,[4]=5,[5]=6,[6]=7}"), *w);
}

TEST(TupleArray)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("t")));
    CHECK(ParseGDBWatchValue(w, wxT("{{\n      a = 1,\n      b = 2\n    }, {\n      a = 3,\n      b = 5\n    }}")));
    CHECK_EQUAL(wxT("t= {[0]= {a=1,b=2},[1]= {a=3,b=5}}"), *w);
}

TEST(StdStringWithCommas)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("t")));
    CHECK(ParseGDBWatchValue(w, wxT("{\nb = 0x3e24e4 \"AAAA,BBBB,CCCC,DDDDD\"}")));
    CHECK_EQUAL(wxT("t= {b=0x3e24e4 \"AAAA,BBBB,CCCC,DDDDD\"}"), *w);
}

TEST(StringWithQuotes)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("t")));
    CHECK(ParseGDBWatchValue(w, wxT("{\nb= 0x3e24e4 \"AAAA,\\\"BBBB\\\",CCCC,DDDDD\"}")));
    CHECK_EQUAL(wxT("t= {b=0x3e24e4 \"AAAA,\\\"BBBB\\\",CCCC,DDDDD\"}"), *w);
}

TEST(RepeatingChars0)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("t")));
    CHECK(ParseGDBWatchValue(w, wxT("{\n\tc = 0x400d90 'A' <repeats 16 times>, \"aa\\\"a\"\n}")));
    CHECK_EQUAL(wxT("t= {c=0x400d90 'A' <repeats 16 times>, \"aa\\\"a\"}"), *w);
}

TEST(RepeatingChars1)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("t")));
    CHECK(ParseGDBWatchValue(w, wxT("{\n\tc = 0x400d90 'A' <repeats 16 times>, ' ' <repeats 29 times>, \"aabba\"\n}")));
    CHECK_EQUAL(wxT("t= {c=0x400d90 'A' <repeats 16 times>, ' ' <repeats 29 times>, \"aabba\"}"), *w);
}

TEST(RepeatingChars2)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("t")));
    CHECK(ParseGDBWatchValue(w, wxT("{\n\tc = 0x400d90 'A' <repeats 16 times>, ' ' <repeats 29 times>, \"aaa\",\n\ta = 5}")));
    CHECK_EQUAL(wxT("t= {c=0x400d90 'A' <repeats 16 times>, ' ' <repeats 29 times>, \"aaa\",a=5}"), *w);
}

TEST(RepeatingChars3)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("t")));
    // c = 0x400dd8 "{\n\tc = 0x400d90 'A' <repeats 16 times>, ' ' <repeats 29 times>, \"aaa\"\n}"
    CHECK(ParseGDBWatchValue(w, wxT("{\n\tc = 0x400dd8 \"{c = 0x400d90 'A' <repeats 16 times>,")
                                wxT(" ' ' <repeats 29 times>, \\\"aaa\\\"}\"}")));
    CHECK_EQUAL(wxT("t= {c=0x400dd8 \"{c = 0x400d90 'A' <repeats 16 times>, ' ' <repeats 29 times>, \\\"aaa\\\"}\"}"), *w);
}

TEST(RepeatingChars4)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("t")));
    CHECK(ParseGDBWatchValue(w, wxT("{\n  name = \"bb\", '\\000' <repeats 14 times>\n}")));
    CHECK_EQUAL(wxT("t= {name=\"bb\", '\\000' <repeats 14 times>}"), *w);
}

TEST(RepeatingChars5)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("t")));
    CHECK(ParseGDBWatchValue(w, wxT("{\n  name = \"bb\", '\\000' <repeats 14 times>, \"aabbccddee\"\n}")));
    CHECK_EQUAL(wxT("t= {name=\"bb\", '\\000' <repeats 14 times>, \"aabbccddee\"}"), *w);
}

TEST(RepeatingChars6)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("t")));
    CHECK(ParseGDBWatchValue(w, wxT("{\n  name1 = \"aa\", '\\000' <repeats 14 times>,\n")
                                wxT("  name2 = \"bb\", '\\000' <repeats 12 times>, \"aabbccddee\"\n}")));
    CHECK_EQUAL(wxT("t= {name1=\"aa\", '\\000' <repeats 14 times>,name2=\"bb\",")
                wxT(" '\\000' <repeats 12 times>, \"aabbccddee\"}"), *w);
}

TEST(RepeatingChars7)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("t")));
    CHECK(ParseGDBWatchValue(w, wxT("{\n  name = \"bb\", '\\000' <repeats 14 times>, '\\000' <repeats 12 times>\n}")));
    CHECK_EQUAL(wxT("t= {name=\"bb\", '\\000' <repeats 14 times>, '\\000' <repeats 12 times>}"), *w);
}

TEST(RepeatingChars8)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("t")));
    CHECK(ParseGDBWatchValue(w, wxT("{\n  name = \"bb\", '\\000' <repeats 14 times>, \"aabb\",")
                                wxT(" '\\000' <repeats 12 times>, \"aabbccddee\"\n}")));
    CHECK_EQUAL(wxT("t= {name=\"bb\", '\\000' <repeats 14 times>, \"aabb\",")
                wxT(" '\\000' <repeats 12 times>, \"aabbccddee\"}"), *w);
}

// parsing the output of "const char *[]"
TEST(RepeatingChars9)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("t")));
    CHECK(ParseGDBWatchValue(w, wxT("{0x400e90 \"1st\", 0x400e94 '.' <repeats 16 times>, 0x400ea5 \"3th\"}")));
    CHECK_EQUAL(wxT("t= {[0]=0x400e90 \"1st\",[1]=0x400e94 '.' <repeats 16 times>,[2]=0x400ea5 \"3th\"}"), *w);
}

// parsing the output of "const char *[]"
TEST(RepeatingChars10)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("t")));
    CHECK(ParseGDBWatchValue(w, wxT("{0x4080d8 \"1st\", 0x4080dc \"2nd\", '.' <repeats 48 times>, 0x408110 \"3th\"}")));
    CHECK_EQUAL(wxT("t= {[0]=0x4080d8 \"1st\",[1]=0x4080dc \"2nd\", '.' <repeats 48 times>,[2]=0x408110 \"3th\"}"), *w);
}

// parsing the output of "const char *[]"
TEST(RepeatingChars11)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("t")));
    CHECK(ParseGDBWatchValue(w, wxT("{0x4080d8 \"1st\", 0x4080dc '.' <repeats 14 times>,")
                                wxT(" \"#\", '&' <repeats 16 times>, 0x4080fc \"3th\"}")));
    CHECK_EQUAL(wxT("t= {[0]=0x4080d8 \"1st\",[1]=0x4080dc '.' <repeats 14 times>, \"#\", '&' <repeats 16 times>,")
                wxT("[2]=0x4080fc \"3th\"}"), *w);
}

TEST(ChangeType0)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("*s")));
    CHECK(ParseGDBWatchValue(w, wxT("Cannot access memory at address 0x0")));
    CHECK(ParseGDBWatchValue(w, wxT("{\n  number = 29,\n  real = 36\n}")));
    CHECK_EQUAL(wxT("*s= {number=29,real=36}"), *w);
}

TEST(ChangeType1)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("s")));
    CHECK(ParseGDBWatchValue(w, wxT("10")));
    CHECK(ParseGDBWatchValue(w, wxT("{\n  number = 29,\n  real = 36\n}")));
    CHECK_EQUAL(wxT("s= {number=29,real=36}"), *w);
}

TEST(StructSummarySimple)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("s")));
    CHECK(ParseGDBWatchValue(w, wxT("test = {a=5}")));
    CHECK_EQUAL(wxT("s=test {a=5}"), *w);
}

/*
This one is highly unparsable :(
TEST(StructSummaryComplex)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("s")));
    CHECK(ParseGDBWatchValue(w, wxT("{a= test2, test3 ={b = 5}}")));
    CHECK_EQUAL(wxT("s= {a=test2, test3 {b=5}"), *w);
}
*/

TEST(PythonSTLVector)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("s")));
    CHECK(ParseGDBWatchValue(w, wxT("std::vector of length 4, capacity 4 = {0, 1, 2, 3}")));
    CHECK_EQUAL(wxT("s=length 4, capacity 4 {[0]=0,[1]=1,[2]=2,[3]=3}"), *w);
}

TEST(PythonSTLMap)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("s")));
    CHECK(ParseGDBWatchValue(w, wxT("std::map with 20 elements = {[\"BEGIN_EVENT_TABLE\"] = \"-END_EVENT_TABLE\"}")));
    CHECK_EQUAL(wxT("s=20 elements {[\"BEGIN_EVENT_TABLE\"]=\"-END_EVENT_TABLE\"}"), *w);
}
/*
TEST(PythonSTLMapVector)
{
    GDBWatch::Pointer w(new GDBWatch(wxT("s")));
    CHECK(ParseGDBWatchValue(w, wxT("std::map with 3 elements = {")
                                wxT("[\"test1\"] = std::vector of length 4, capacity 4 = {0, 1, 2, 3}, ")
                                wxT("[\"test2\"] = std::vector of length 4, capacity 4 = {0, 1, 2, 3}, ")
                                wxT("[\"test3\"] = std::vector of length 4, capacity 4 = {0, 1, 2, 3}}")));
    CHECK_EQUAL(wxT("s=3 elements {}"), *w);
}
*/

} // SUITE(GDBWatchParser)

