#include "debuggergdb_test_common.h"
#include "parsewatchvalue.h"

inline std::ostream& operator<<(std::ostream &stream, GDBLocalVariable const &s)
{
    return stream << s.name.utf8_str().data() << "|" << s.value.utf8_str().data() << ">";
}

inline bool operator == (wxString const &s, GDBLocalVariable const &v)
{
    return s == v.name + wxT("|") + v.value;
}


SUITE(GDBLocalsParser)
{

TEST(TokenizeSimple)
{
    std::vector<GDBLocalVariable> results;
    TokenizeGDBLocals(results, wxT("a = 5\nb = 10"));
    CHECK_EQUAL(2u, results.size());
    CHECK_EQUAL(wxT("a|5"), results[0]);
    CHECK_EQUAL(wxT("b|10"), results[1]);
}

TEST(TokenizeCompound)
{
    std::vector<GDBLocalVariable> results;
    TokenizeGDBLocals(results, wxT("a = 10\ncompound = {\n  a = 1, \n  b = 2\n}"));
    CHECK_EQUAL(2u, results.size());
    CHECK_EQUAL(wxT("a|10"), results[0]);
    CHECK_EQUAL(wxT("compound|{\n  a = 1, \n  b = 2\n}"), results[1]);
}

TEST(TokenizeString0)
{
    std::vector<GDBLocalVariable> results;
    TokenizeGDBLocals(results, wxT("a = \"test\n\"\nb = 10"));
    CHECK_EQUAL(2u, results.size());
    CHECK_EQUAL(wxT("a|\"test\n\""), results[0]);
    CHECK_EQUAL(wxT("b|10"), results[1]);
}

TEST(TokenizeString1)
{
    std::vector<GDBLocalVariable> results;
    TokenizeGDBLocals(results, wxT("a = \"test{\"\nb = 10"));
    CHECK_EQUAL(2u, results.size());
    CHECK_EQUAL(wxT("a|\"test{\""), results[0]);
    CHECK_EQUAL(wxT("b|10"), results[1]);
}

TEST(TokenizeString2)
{
    std::vector<GDBLocalVariable> results;
    TokenizeGDBLocals(results, wxT("a = 123 '{'\nb = 10"));
    CHECK_EQUAL(2u, results.size());
    CHECK_EQUAL(wxT("a|123 '{'"), results[0]);
    CHECK_EQUAL(wxT("b|10"), results[1]);
}

TEST(TokenizeString3)
{
    std::vector<GDBLocalVariable> results;
    TokenizeGDBLocals(results, wxT("a = 123 '\"'\nb = 10"));
    CHECK_EQUAL(2u, results.size());
    CHECK_EQUAL(wxT("a|123 '\"'"), results[0]);
    CHECK_EQUAL(wxT("b|10"), results[1]);
}

TEST(TokenizeString4)
{
    std::vector<GDBLocalVariable> results;
    TokenizeGDBLocals(results, wxT("a = \"test's\"\nb = 10"));
    CHECK_EQUAL(2u, results.size());
    CHECK_EQUAL(wxT("a|\"test's\""), results[0]);
    CHECK_EQUAL(wxT("b|10"), results[1]);
}
} // SUITE(GDBLocalsParser)
