/*
* This file is part of HexEditor plugin for Code::Blocks Studio
* Copyright (C) 2008 Bartlomiej Swiecki
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

#ifndef EXPRESSIONTESTCASES_H
#define EXPRESSIONTESTCASES_H

#include <wx/string.h>

#include "ExpressionPreprocessed.h"

namespace Expression
{

    class TestCases
    {
        public:

            /** \brief Execute tests */
            bool PerformTests();

        protected:

            /** \brief Test condition */
            void Ensure( bool condition, const wxString& failMsg );

            /** \brief Execute some code and return calculated value */
            Value Execute( const wxString& code );

            /** \brief Test if given code compiles */
            void TestCompile( const wxString& code );

            /** \brief Test if given code does NOT compile */
            void TestNoCompile( const wxString& code );

            /** \brief Test if given expression gives required value */
            template< typename T >
            void TestValue( const wxString& code, T value );

            /** \brief Test if given expression gives required value with some epsilon */
            template< typename T >
            void TestValueEps(const wxString& code, T value, double epsilon = 0.000000000001 );


            /** \brief Add line to log */
            virtual void AddLog( const wxString& logLine ) = 0;

            /** \brief Check if we should stop testing */
            virtual bool StopTest() = 0;

        private:

            /** \brief type used to send NoSuchTest exceptions */
            struct NoSuchTest {};

            /** \brief error report */
            struct TestError { wxString m_Msg; };

            /** \brief Perform given test */
            template< int testNo >
            void Test();

            /** \brief Run all test from 0 to testNo
             *  \return No of last available test
             */
            template< int testNo >
            inline int Runner();

            int m_FailCnt;
            int m_PassCnt;
            int m_SkipCnt;
    };

}

#endif
