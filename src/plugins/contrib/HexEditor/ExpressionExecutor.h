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

#ifndef EXPRESSIONEXECUTOR_H
#define EXPRESSIONEXECUTOR_H

#include "FileContentBase.h"
#include "ExpressionPreprocessed.h"

namespace Expression
{

    /** \brief Executor of preprocessed expression */
    class Executor
    {
        public:

            /** \brief Ctor */
            Executor();

            /** \brief Dctor */
            ~Executor();

            /** \brief Execute code */
            inline bool Execute( const Preprocessed& code, FileContentBase* content, FileContentBase::OffsetT current )
            {
                m_Code = &code;
                m_Content = content;
                m_Current = current;
                return Run();
            }

            /** \brief Get error description */
            wxString ErrorDesc();

            bool GetResult( unsigned long long& val );
            bool GetResult( long long& val );
            bool GetResult( long double& val );
            Value GetResult();

        private:

            bool Run();
            inline void ExecuteOneOp();

            inline Value& GetStack( int pos = 0 )
            {
                if ( (unsigned) pos >= m_Stack.size() ) throw errorStackIndex;

                return m_Stack[ m_Stack.size() - pos - 1 ];
            }

            inline void PopStack( )
            {
                if ( m_Stack.empty() ) throw errorStackIndex;
                m_Stack.pop_back();
            }

            inline void PushStack( const Value& v )
            {
                m_Stack.push_back( v );
            }

            inline void ReplaceStack( const Value& v, int pos = 0 )
            {
                if ( (unsigned) pos >= m_Stack.size() ) throw errorStackIndex;
                m_Stack[ m_Stack.size() - pos - 1 ] = v;
            }

            template< typename T > Value Content( long long address )
            {
                T val;
                if ( m_Content->Read( &val, address, sizeof(T) ) != sizeof(T) )
                {
                    throw errorContentIndex;
                }
                return Value( val );
            }

            inline void PushArgument( const Operation& op, long long address );

            inline void PushAddress( const Operation& op, long long address );

            template< template<typename> class T >
            inline void BinaryOp( const Operation& op );

            template< template<typename> class T >
            inline void UnaryOp( const Operation& op );

            template< template<typename,typename> class T >
            inline void UnaryOp2( const Operation& op );

            template<  template<typename,typename> class T1, typename T2 >
            inline void UnaryOp2Helper( const Operation& op, T2 value );

            std::vector< Value >        m_Stack;
            const Preprocessed*         m_Code;
            int                         m_OperationPos;
            FileContentBase*            m_Content;
            FileContentBase::OffsetT    m_Current;
            executionError              m_Status;

    };
}

#endif
