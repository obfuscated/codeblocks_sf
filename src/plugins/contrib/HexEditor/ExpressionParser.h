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
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision:$
* $Id:$
* $HeadURL:$
*/

#ifndef EXPRESSIONPARSER_H
#define EXPRESSIONPARSER_H

#include "FileContentBase.h"
#include "ExpressionPreprocessed.h"

#include <wx/string.h>
#include <wx/arrstr.h>

namespace Expression
{

    /** \brief Class responsible for parsing and evaluating expressions */
    class Parser
    {
        public:

            /** \brief Ctor */
            Parser();

            /** \brief Dctor */
            ~Parser();

            /** \brief Parsing the expression */
            bool Parse( const wxString& expression, Preprocessed& output );

            /** \brief Get description of error code from last parse
             *  \param pos here position of the error will be stored
             *  \return Error description
             */
            inline wxString ParseErrorDesc( int& pos ) { pos = m_ErrorPos; return m_ErrorDesc; }

            /** \brief Produce suggestions for given position in the edited code
             *  \param expression edited expression
             *  \param pos position of the cursor in the expression
             *  \return array of suggested values
             */
            wxArrayString GetSuggestions( const wxString& expression, int pos );

        private:

            wxString m_ErrorDesc;
            int      m_ErrorPos;

            Preprocessed* m_Output;

            const wxChar* m_StartPos;
            const wxChar* m_CurrentPos;

            struct ParseTree
            {
                Operation op;
                ParseTree* first;
                ParseTree* second;

                ParseTree(): first(0), second(0) {}
                ~ParseTree() { delete first; delete second; }
            };

            typedef Operation::modifier resType;

            static const resType resSignedInt   = Operation::modLongLong;
            static const resType resUnsignedInt = Operation::modQword;
            static const resType resFloat       = Operation::modLongDouble;

            std::vector< ParseTree* > m_TreeStack;


            int m_PiArg;
            int m_EArg;

            void Parse();
            void Expression();
            void Add();
            void Mult();
            void Unary();
            void Primary();
            bool Number();
            bool Const();
            bool Memory();
            void ConstArg( int& argNum, long double value );

            void AddOp(
                int subArgs,
                Operation::opCode op,
                Operation::modifier mod1 = Operation::modNone,
                Operation::modifier mod2 = Operation::modNone,
                short opConst = 0 );

            inline ParseTree* PopTreeStack()
            {
                assert( !m_TreeStack.empty() );
                ParseTree* ret = m_TreeStack.back();
                m_TreeStack.pop_back();
                return ret;
            }

            inline void PushTreeStack( ParseTree* t )
            {
                m_TreeStack.push_back( t );
            }

            inline resType TopType()
            {
                assert( !m_TreeStack.empty() );
                return (resType)m_TreeStack.back()->op.m_Mod1;
            }

            inline resType TopType( int pos )
            {
                assert( (int)m_TreeStack.size() > pos );
                return (resType)m_TreeStack[ m_TreeStack.size() - pos - 1 ]->op.m_Mod1;
            }

            inline resType HigherType2Top();

            template< typename T >
            inline int AddArg( T value ) { return m_Output->PushArgument( Value(value) ); }

            inline wxChar Get();
            inline wxChar Get( int shift );

            inline bool Match( wxChar ch );
            inline bool Match( const wxChar* text );
            inline void Require( wxChar ch );
            inline void Require( const wxChar* text );
            inline void EatWhite();
            inline void Next();

            inline void Error( const wxString& desc );

            inline resType HigherType( resType t1, resType m2 );

            void GenerateCode( ParseTree* tree );
            void GenerateCodeAndConvert( ParseTree* tree, resType type );
    };
}

#endif
