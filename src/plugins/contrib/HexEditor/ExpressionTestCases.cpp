#include "ExpressionTestCases.h"
#include "ExpressionParser.h"
#include "ExpressionExecutor.h"

#include <wx/intl.h>
#include <sstream>

namespace Expression
{

//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//

    template<>
    void TestCases::Test<1>()
    {
        TestCompile( _T("1") );
        TestCompile( _T("E") );
        TestCompile( _T("PI") );
        TestCompile( _T("@") );
        TestCompile( _T("cur") );
    }

    template<>
    void TestCases::Test<2>()
    {
        TestNoCompile( _T("a") );
        TestNoCompile( _T("e") );
        TestNoCompile( _T("pi") );
        TestNoCompile( _T("sin") );
        TestNoCompile( _T("+") );
    }

    template<>
    void TestCases::Test<3>()
    {
        TestValue( _T("1"),   1 );
        TestValue( _T("-1"), -1 );
        TestValue( _T("10"), 10 );
        TestValueEps( _T("E - E"), 0 );
    }

    template<>
    void TestCases::Test<4>()
    {
        TestValueEps( _T("10.0"), 10 );
        TestValueEps( _T("20."),  20 );
        TestValueEps( _T("0.1"), 0.1 );
        TestValueEps( _T("0.12345432123454321"), 0.12345432123454321 );
        TestValueEps( _T(".123"), 0.123 );
    }

    template<>
    void TestCases::Test<5>()
    {
        TestValue( _T("1 + 2"),  3 );
        TestValue( _T("2 - 3"), -1 );
        TestValue( _T("3 * 4"), 12 );
        TestValue( _T("5 % 3"),  2 );
        TestValue( _T("5 / 2"),  2 );
        TestValueEps( _T("5 / 2."), 2.5 );
    }

    template<>
    void TestCases::Test<6>()
    {
        TestValueEps( _T("sin(0)"),      0 );
        TestValueEps( _T("sin(PI)"),     0 );
        TestValueEps( _T("sin(2*PI)"),   0 );
        TestValueEps( _T("sin(100*PI)"), 0 );

        TestValueEps( _T("cos(0)"),      1 );
        TestValueEps( _T("cos(PI)"),    -1 );
        TestValueEps( _T("cos(2*PI)"),   1 );
        TestValueEps( _T("cos(99*PI)"), -1 );

        TestValueEps( _T("tg(0)"),                    0 );
        TestValueEps( _T("tg(PI/6) - pow(3,0.5)/3"),  0 );
        TestValueEps( _T("tg(PI/4)"),                 1 );
        TestValueEps( _T("tg(PI/3) - pow(3,0.5)"),    0 );

        TestValueEps( _T("ctg(PI/2)"),                0 );
        TestValueEps( _T("ctg(PI/3) - pow(3,0.5)/3"), 0 );
        TestValueEps( _T("ctg(PI/4)"),                1 );
        TestValueEps( _T("ctg(PI/6) - pow(3,0.5)"),   0 );

    }

    template<>
    void TestCases::Test<7>()
    {
        TestValueEps( _T("ln(E)"),            1 );
        TestValueEps( _T("ln(E*E)"),          2 );
        TestValueEps( _T("ln(E*E*E)"),        3 );
        TestValueEps( _T("ln(pow(E,100))"), 100 );
    }


    template<>
    void TestCases::Test<8>()
    {
        TestValue( _T("100 - 10 - 20 - 30"), 40 );
        TestValue( _T("100 + -10 + -20 + -30"), 40 );
        TestValue( _T("1 + 2 * 3"), 7 );
        TestValue( _T("1 * 2 + 3"), 5 );
    }



//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//


    void TestCases::Ensure( bool cond, const wxString& failMsg )
    {
        if ( !cond )
        {
            TestError err;
            err.m_Msg = failMsg;
            throw err;
        }
    }

    Value TestCases::Execute( const wxString& code )
    {
        Parser p;
        Preprocessed out;
        Ensure( p.Parse( code, out ), wxString::Format( _("Failed to parse expression: '%s'"), code.c_str() ) );
        Executor e;
        Ensure( e.Execute( out, 0, 0 ), wxString::Format( _("Couldn't execute expression: '%s'"), code.c_str() ) );
        return e.GetResult();
    }


    void TestCases::TestCompile( const wxString& code )
    {
        Parser p;
        Preprocessed out;
        Ensure( p.Parse( code, out ), wxString::Format( _("Failed to parse expression: '%s'"), code.c_str() ) );
    }

    template< typename T >
    void TestCases::TestValue(const wxString& code, T value )
    {
        Value ret = Execute( code );

        std::ostringstream ost;
        ost << ret;
        wxString retStr( ost.str().c_str(), wxConvLocal );

        std::ostringstream ost2;
        ost2.str().clear();
        ost2 << value;
        wxString valStr( ost2.str().c_str(), wxConvLocal );

        Ensure( ret == value, wxString::Format( _("Invalid value returned for expression: '%s', got %s, should be %s"), code.c_str(), retStr.c_str(), valStr.c_str() ) );
    }

    template< typename T >
    void TestCases::TestValueEps(const wxString& code, T value, double epsilon )
    {
        Value ret = Execute( code );

        std::ostringstream ost;
        ost << ret;
        wxString retStr( ost.str().c_str(), wxConvLocal );

        std::ostringstream ost2;
        ost2.str().clear();
        ost2 << value;
        wxString valStr( ost2.str().c_str(), wxConvLocal );

        bool ok1 = ret >= ( value - epsilon );
        bool ok2 = ret <= ( value + epsilon );

        Ensure( ok1 && ok2, wxString::Format( _("Invalid value returned for expression: '%s', got %s, should be %s"), code.c_str(), retStr.c_str(), valStr.c_str() ) );
    }

    void TestCases::TestNoCompile( const wxString& code )
    {
        Parser p;
        Preprocessed out;
        Ensure( !p.Parse( code, out ), wxString::Format( _("Parsed invalid expression: '%s'"), code.c_str() ) );
    }

    static const int maxTests = 0x100;

    bool TestCases::PerformTests()
    {
        m_FailCnt = 0;
        m_PassCnt = 0;
        m_SkipCnt = 0;

        Runner< maxTests > ();

        AddLog( wxString::Format( _T("===============================") ) );
        AddLog( wxString::Format( _T("Summary:") ) );
        AddLog( wxString::Format( _T(" Passed: %d"), m_PassCnt ) );
        AddLog( wxString::Format( _T(" Failed: %d"), m_FailCnt ) );
        AddLog( wxString::Format( _T("  Total: %d"), m_PassCnt + m_FailCnt ) );

        return m_FailCnt == 0;
    }

    // Default test implementation - notify that there's no such test
    template< int >
    void TestCases::Test()
    {
        // Notify that there's no such error
        throw NoSuchTest();
    }

    // Default runner implementation - run all test before and then call itself
    template< int testNo >
    inline int TestCases::Runner( )
    {
        // Call all previous tests
        int prevTest = Runner< testNo-1 >();

        // Check if someone has requested tests to stop
        if ( StopTest() )
        {
            return testNo;
        }

        wxString result;
        bool pass = true;

        // Call our own test
        try
        {
            Test< testNo >();
        }
        catch ( const TestError& err )
        {
            // Display test's info
            pass = false;
            result = wxString::Format( _T("Test %d FAILED: %s"), testNo, err.m_Msg.c_str() );
        }
        catch ( NoSuchTest )
        {
            // There was no such test
            m_SkipCnt++;
            return prevTest;
        }

        // Enumerate skipped tests
        for ( int i = prevTest+1; i<testNo; i++ )
        {
            AddLog( wxString::Format( _T("Test %d skipped: not defined"), i)  );
        }

        // Log our results
        AddLog( pass ? wxString::Format( _T("Test %d passed"), testNo ) : result );

        ( pass ? m_PassCnt : m_FailCnt ) ++;

        return testNo;
    }

    // Stopper for test runner - we stop when test 0 is reached
    template<>
    inline int TestCases::Runner<0>()
    {
        return 0;
    }

}
