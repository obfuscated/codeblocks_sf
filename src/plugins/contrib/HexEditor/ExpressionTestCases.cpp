/*
* This file is part of HexEditor plugin for Code::Blocks Studio
* Copyright (C) 2008-2009 Bartlomiej Swiecki
*
* HexEditor plugin is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* HexEditor pluging is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with HexEditor. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "ExpressionTestCases.h"
#include "ExpressionParser.h"
#include "ExpressionExecutor.h"
#include "TestCasesHelper.h"

#include <wx/intl.h>
#include <sstream>

namespace Expression
{
    struct ExpressionTests
    {
        Value Execute( const wxString& code );
        void TestCompile( const wxString& code );
        void TestNoCompile( const wxString& code );

        template< typename T > void TestValue(const wxString& code, T value );
        template< typename T > void TestValueEps(const wxString& code, T value, double epsilon = 0.000000000001 );

        inline void Ensure( bool condition, const wxString& msg )
        {
            ((TestCasesHelper< ExpressionTests >*)this)->Ensure( condition, msg );
        }
    };

}

using namespace Expression;

typedef TestCasesHelper< ExpressionTests > TestCases;

//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//

template<>
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
template<>
void TestCases::Test<3>()
{
    TestValue( _T("1"),   1 );
    TestValue( _T("-1"), -1 );
    TestValue( _T("10"), 10 );
    TestValueEps( _T("E - E"), 0 );
}

template<>
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
template<>
void TestCases::Test<7>()
{
    TestValueEps( _T("ln(E)"),            1 );
    TestValueEps( _T("ln(E*E)"),          2 );
    TestValueEps( _T("ln(E*E*E)"),        3 );
    TestValueEps( _T("ln(pow(E,100))"), 100 );
}


template<>
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

namespace Expression
{
    Value ExpressionTests::Execute( const wxString& code )
    {
        Parser p;
        Preprocessed out;
        Ensure( p.Parse( code, out ), wxString::Format( _("Failed to parse expression: '%s'"), code.c_str() ) );
        Executor e;
        Ensure( e.Execute( out, 0, 0 ), wxString::Format( _("Couldn't execute expression: '%s'"), code.c_str() ) );
        return e.GetResult();
    }


    void ExpressionTests::TestCompile( const wxString& code )
    {
        Parser p;
        Preprocessed out;
        Ensure( p.Parse( code, out ), wxString::Format( _("Failed to parse expression: '%s'"), code.c_str() ) );
    }

    template< typename T >
    void ExpressionTests::TestValue(const wxString& code, T value )
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
    void ExpressionTests::TestValueEps(const wxString& code, T value, double epsilon )
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

    void ExpressionTests::TestNoCompile( const wxString& code )
    {
        Parser p;
        Preprocessed out;
        Ensure( !p.Parse( code, out ), wxString::Format( _("Parsed invalid expression: '%s'"), code.c_str() ) );
    }

    TestCases tests;

    TestCasesBase& GetTests() { return tests; }
}
