/*
* This file is part of HexEditor plugin for Code::Blocks studio
* Copyright (C) 2009 Bartlomiej Swiecki
*
* HexEditor plugin is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* HexEditor plugin is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with HexEditor plugin. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#ifndef TESTCASESHELPER_H
#define TESTCASESHELPER_H

#include "TestCasesBase.h"

template< typename T, int maxTests > class TestCasesHelper;

namespace Detail
{
    struct RunHelperBase
    {
        template< typename T, int maxTests, int testNo >
        inline int CallRunner( TestCasesHelper< T, maxTests >& hlpr, int prevTest )
        {
            return hlpr. template Runner< testNo >( prevTest );
        }
    };

    /** \brief Extra structure needed because of lack of partial function template specialization */
    template< typename T, int maxTests, int testNo >
    struct RunHelper: public RunHelperBase { public: inline int Run( TestCasesHelper< T, maxTests >& hlpr )
    {
        return CallRunner< T, maxTests, testNo >( hlpr, RunHelper< T, maxTests, testNo-1 >().Run( hlpr ) );
    }};

    template< typename T, int maxTests >
    struct RunHelper<T, maxTests, 0 > { public: inline int Run( TestCasesHelper< T, maxTests >& ) { return 0; } };
}

/** \brief Helper for automated test cases */
template< typename T, int maxTests = 50 >
class TestCasesHelper: public T, public TestCasesBase
{
    public:

        /** \brief Run the tests */
        virtual bool PerformTests()
        {
            return RunTests();
        }

        /** \brief Main function performing the test,
         *         Implementations are made by partial instantiation
         */
        template< int i >
        void Test()
        {
            m_NoSuchTest = true;
        }

        /** \brief Test condition */
        void Ensure( bool condition, const wxString& failMsg )
        {
            if ( !condition )
            {
                TestError err;
                err.m_Msg = failMsg;
                throw err;
            }
        }

    private:

        /** \brief Run all the tests */
        inline bool RunTests()
        {
            m_FailCnt = 0;
            m_PassCnt = 0;
            m_SkipCnt = 0;

            Detail::RunHelper< T, maxTests, maxTests > ().Run( *this );

            AddLog( wxString::Format( _T("===============================") ) );
            AddLog( wxString::Format( _T("Summary:") ) );
            AddLog( wxString::Format( _T(" Passed: %d"), m_PassCnt ) );
            AddLog( wxString::Format( _T(" Failed: %d"), m_FailCnt ) );
            AddLog( wxString::Format( _T("  Total: %d"), m_PassCnt + m_FailCnt ) );

            return m_FailCnt == 0;
        }

        /** \brief Run all test from 0 to testNo
         *  \return No of last available test
         */
        template< int testNo >
        inline int Runner( int prevTest )
        {
            // Check if someone has requested tests to stop
            if ( StopTest() )
            {
                return testNo;
            }

            wxString result;
            bool pass = true;
            m_NoSuchTest = false;

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

            if ( m_NoSuchTest )
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

        /** \brief error report */
        struct TestError { wxString m_Msg; };

        int m_FailCnt;
        int m_PassCnt;
        int m_SkipCnt;
        bool m_NoSuchTest;

        friend class Detail::RunHelperBase;
};



#endif
