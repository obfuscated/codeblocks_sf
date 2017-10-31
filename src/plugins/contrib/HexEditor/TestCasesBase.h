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


#ifndef TESTCASESBASE_H
#define TESTCASESBASE_H

/** \brief Base class for performing tests */
class TestCasesBase
{
    public:

        /** \brief Structure gathering the output from tests */
        struct Output
        {
            virtual void AddLog( const wxString& logLine ) = 0;
            virtual bool StopTest() = 0;
        };

        /** \brief Ctor */
        TestCasesBase(): m_Out( 0 ) {}

        /** \brief Initialize functors */
        inline void InitOutput( Output& out ) { m_Out = &out; }

        /** \brief Dctor */
        virtual ~TestCasesBase() {}

        /** \brief Perform all tests */
        virtual bool PerformTests() = 0;

    protected:

        /** \brief Output some test log */
        inline void AddLog( const wxString& logLine ) { m_Out->AddLog( logLine ); }

        /** \brief Check if we should stop */
        inline bool StopTest() { return m_Out->StopTest(); }

    private:

        Output* m_Out;  ///< \brief structure gathering the output
};

#endif
