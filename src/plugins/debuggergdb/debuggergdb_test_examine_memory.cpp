#include "debuggergdb_test_common.h"
#include "parsewatchvalue.h"

#include <iomanip>
#include <initializer_list>

static std::ostream& operator<<(std::ostream &stream, std::vector<uint8_t> const &s)
{
    stream << "{ ";
    bool first = true;
    for (uint8_t ch : s)
    {
        if (first)
            first = false;
        else
            stream << ", ";
        stream << std::hex << "0x" << std::setw(2) << std::setfill('0') << int(ch);
    }
    stream << " }";
    return stream;
}

static std::vector<uint8_t> MakeV(std::initializer_list<uint8_t> list)
{
    return std::vector<uint8_t>(list);
}

SUITE(ExamineMemory)
{

TEST(Valid0)
{
    wxString addr;
    std::vector<uint8_t> values;
    CHECK(ParseGDBExamineMemoryLine(addr, values, wxT("0x22ffc0:       0xf0    0xff    0x22    0x00    0x4f    0x6d    0x81    0x7c")));
    CHECK_EQUAL(MakeV({0xf0, 0xff, 0x22, 0x00, 0x4f, 0x6d, 0x81, 0x7c}), values);
    CHECK_EQUAL(wxT("0x22ffc0"), addr);
}

TEST(Valid1)
{
    wxString addr;
    std::vector<uint8_t> values;
    CHECK(ParseGDBExamineMemoryLine(addr, values, wxT("0x85267a0 <RS485TxTask::taskProc()::rcptBuf>:   0xa0   0xb0   0x0c   0x00   0x00   0x00   0x00   0x11")));
    CHECK_EQUAL(MakeV({0xa0, 0xb0, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x11}), values);
    CHECK_EQUAL(wxT("0x85267a0"), addr);
}

TEST(Valid2_clearValues)
{
    wxString addr;
    std::vector<uint8_t> values;
    values.push_back(134);
    values.push_back(13);
    values.push_back(14);
    values.push_back(1);
    CHECK(ParseGDBExamineMemoryLine(addr, values, wxT("0x85267a0 <RS485TxTask::taskProc()::rcptBuf>:   0xa0   0xb0   0x0c   0x00   0x00   0x00   0x00   0x11")));
    CHECK_EQUAL(MakeV({0xa0, 0xb0, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x11}), values);
    CHECK_EQUAL(wxT("0x85267a0"), addr);
}

TEST(InvalidAddress0)
{
    wxString addr;
    std::vector<uint8_t> values;
    CHECK(!ParseGDBExamineMemoryLine(addr, values, wxT("Cannot access memory at address 0xa0000000")));
    CHECK(values.empty());
    CHECK(addr.empty());
}

TEST(InvalidAddress0_lineAfter)
{
    wxString addr;
    std::vector<uint8_t> values;
    CHECK(ParseGDBExamineMemoryLine(addr, values, wxT("0xa0000000:")));
    CHECK(values.empty());
    CHECK_EQUAL(wxT("0xa0000000"), addr);
}

} // SUITE(GDBMemoryWatch)
