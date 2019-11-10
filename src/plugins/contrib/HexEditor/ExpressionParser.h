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

#ifndef EXPRESSIONPARSER_H
#define EXPRESSIONPARSER_H

#include "FileContentBase.h"
#include "ExpressionPreprocessed.h"

#include <wx/string.h>
#include <wx/arrstr.h>

#include <map>

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

            /** \brief Get help string to be shown in dialog boxes */
            static wxString GetHelpString();

        private:

            wxString m_ErrorDesc;           ///< \brief Error description if parsing failed
            int      m_ErrorPos;            ///< \brief Error position if parsing failed

            Preprocessed* m_Output;         ///< \brief Here the output should be stored

            const wxChar* m_StartPos;       ///< \brief Beginning of the parse buffer
            const wxChar* m_CurrentPos;     ///< \brief Current parsing positon

            typedef Operation::modifier resType;

            static const resType resNone        = Operation::modNone;
            static const resType resSignedInt   = Operation::modLongLong;
            static const resType resUnsignedInt = Operation::modQword;
            static const resType resFloat       = Operation::modLongDouble;

            /** \brief One node in the parsed syntactic tree */
            struct ParseTree
            {
                resType    m_OutType;       ///< \brief Produced type
                resType    m_InType;        ///< \brief Required type of inputs
                Operation  m_Op;            ///< \brief Executed operation
                ParseTree* m_FirstSub;      ///< \brief First input tree (NULL if no first input)
                ParseTree* m_SecondSub;     ///< \brief Second input tree (NULL if no second input)
                Value      m_ArgValue;      ///< \brief Value of argument in case of loadArg operation


                /** \brief Ctor */
                inline ParseTree()
                    : m_OutType( Operation::modNone )
                    , m_InType( Operation::modNone )
                    , m_FirstSub( 0 )
                    , m_SecondSub( 0 )
                    , m_ArgValue( 0 )
                {}

                /** \brief Dctor - recursively erase the parse tree */
                inline ~ParseTree()
                {
                    delete m_FirstSub;
                    delete m_SecondSub;
                    m_FirstSub = m_SecondSub = 0;
                }
            };

            std::vector< ParseTree* > m_TreeStack;      ///< \brief Current parsing tree
            std::map< Value, int >    m_ArgMap;         ///< \brief Map for finding argument duplicates

            /** \brief Base parsing function */
            void Parse();

            /** \brief Parse at expression level */
            void Expression();

            /** \brief Parse at additive operations level */
            void Add();

            /** \brief Parse at multiplying operations level */
            void Mult();

            /** \brief Parse at unary operators level */
            void Unary();

            /** \brief Parse at primary expression level */
            void Primary();

            /** \brief Try to parse number */
            bool Number();

            /** \brief Try to parse predefined constant */
            bool Const();

            /** \brief Try to parse memory-accessing expression */
            bool Memory();

            /** \brief Try to parse function invocation */
            bool Function();

            /** \brief Generic operation emiting function
             *  \param subArgs number of sub arguments, must be 0, 1 or 2
             *  \param op operation code
             *  \param producedType type of data this operation produces
             *  \param argumentsType required type of arguments
             *  \param mod1 first operation modifier (interpretation depends on operation code)
             *  \param mod2 second operation modifier (interpretation depends on operation code)
             *  \param opConst const helper value (interpretation depends on operation code)
             */
            inline void AddOp(
                int subArgs,
                Operation::opCode op,
                resType producedType,
                resType argumentsType,
                Operation::modifier mod1,
                Operation::modifier mod2 = Operation::modNone,
                short opConst = 0 );

            /** \brief Emit operation requiring one argument
             *  \param op operation code
             *  \param type produced type
             *
             * Following assumptions are given:
             *  required type <- type,
             *  produced type <- type,
             *  mod1 <- type,
             *  mod2 <- none,
             *  const <- 0
             *
             */
            inline void AddOp1( Operation::opCode op, resType type );

            /** \brief Emit operation requiring one argument
             *  \param op operation code
             *
             * This function works as the other version of AddOp1, type is taken from
             * the argument value
             */
            inline void AddOp1( Operation::opCode op );

            /** \brief Emit operation requiring two argument
             *  \param op operation code
             *  \param type produced type
             *
             * Following assumptions are given:
             *  required type <- type,
             *  produced type <- type,
             *  mod1 <- type,
             *  mod2 <- none,
             *  const <- 0
             *
             */
            inline void AddOp2( Operation::opCode op, resType type );

            /** \brief Emit operation requiring two argument
             *  \param op operation code
             *
             * This function works as the other version of AddOp1,
             * type is calculated from HighetType2Top() (more precise type
             * of two arguments used)
             */
            inline void AddOp2( Operation::opCode op );

            /** \brief Emit operation that will emit the value on the stack */
            template< class T >
            inline void ConstArg( T value, resType type );

            /** \brief Pop argument from tree stack and return it */
            inline ParseTree* PopTreeStack()
            {
                assert( !m_TreeStack.empty() );
                ParseTree* ret = m_TreeStack.back();
                m_TreeStack.pop_back();
                return ret;
            }

            /** \brief Push argument onto tree stack */
            inline void PushTreeStack( ParseTree* t )
            {
                m_TreeStack.push_back( t );
            }

            /** \brief Fetch produced type from tree stack at given position */
            inline resType TopType( int pos );

            /** \brief Calculate "higher" (more precise) type from last two arguments on tree stack */
            inline resType HigherType2Top();

            /** \brief Calculate type of top after the negation (unsigned->signed) */
            inline resType TopAfterNeg();

            /** \brief Calculate type of modulo operation at two top arguments on tree stack */
            inline resType ModResult2Top();

            /** \brief Add argument to args code section in produced output
             *
             * This function finds duplicates of arguments and retuns previous id
             * if argument was used before
             */
            inline int AddArg( const Value& value );

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
