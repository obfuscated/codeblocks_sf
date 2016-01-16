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

#include "ExpressionExecutor.h"

#include <wx/intl.h>
#include <cmath>

namespace Expression
{
    namespace { namespace Functors
    {
        template< typename T > struct Adder
        {
            inline T operator()( const T& val1, const T& val2 ) { return val1 + val2; }
        };

        template< typename T > struct Multiplier
        {
            inline T operator()( const T& val1, const T& val2 ) { return val1 * val2; }
        };

        template< typename T > struct Divider
        {
            inline T operator()( const T& val1, const T& val2 ) { if ( !val2 ) throw errorDivByZero; return val1 / val2; }
        };

        template< typename T > struct Moduler
        {
            inline T operator()( const T& val1, const T& val2 ) { if ( !val2 ) throw errorDivByZero; return val1 % val2; }
        };

        template<> struct Moduler< long double >
        {
            inline long double operator()( const long double& /*val1*/, const long double& /*val2*/ ) { throw errorOperation; }
        };

        template< typename T > struct Negation
        {
            inline T operator()( const T& val ) { return -val; }
        };

        template< typename T1, typename T2 > struct Convert
        {
            inline T2 operator()( const T1& val ) { return (T2)val; }
        };
    } }

    Executor::Executor()
    {
    }

    Executor::~Executor()
    {
    }

    wxString Executor::ErrorDesc()
    {
        wxString pos = wxString::Format( _T(" (at %d)"), m_OperationPos-1 );
        switch ( m_Status )
        {
            case executedSuccessfully: return _("Executed successfully") + pos;
            case errorArgumentIndex:   return _("Invalid index of code arguments") + pos;
            case errorOperationIndex:  return _("Invalid index of operation") + pos;
            case errorStackIndex:      return _("Invalid index of stack") + pos;
            case errorContentIndex:    return _("Invalid address inside the content") + pos;
            case errorOperation:       return _("Invalid operation") + pos;
            case errorDivByZero:       return _("Divide by zero") + pos;
            case errorType:            return _("Type mismatch") + pos;
            case errorScript:          return _("Script error") + pos;
            default:                   return _("Unknown error") + pos;
        }
    }


    bool Executor::GetResult( unsigned long long& val )
    {
        if ( m_Stack.size() != 1 ) return false;
        if ( !m_Stack.front().IsUnsignedInt() ) return false;
        val = m_Stack.front().GetUnsignedInt();
        return true;
    }

    bool Executor::GetResult( long long& val )
    {
        if ( m_Stack.size() != 1 ) return false;
        if ( !m_Stack.front().IsSignedInt() ) return false;
        val = m_Stack.front().GetSignedInt();
        return true;
    }

    bool Executor::GetResult( long double& val )
    {
        if ( m_Stack.size() != 1 ) return false;
        if ( !m_Stack.front().IsFloat() ) return false;
        val = m_Stack.front().GetFloat();
        return true;
    }

    Value Executor::GetResult( )
    {
        if ( m_Stack.size() != 1 ) return Value( 0 );
        return m_Stack.front();
    }

    bool Executor::Run()
    {
        m_Stack.clear();
        m_OperationPos = 0;
        try
        {
            for (;;)
            {
                ExecuteOneOp();
            }
        }
        catch ( executionError& status )
        {
            // Note that we also send script-finish notification here
            m_Status = status;
        }

        if ( m_Status != executedSuccessfully ) return false;

        if ( m_Stack.size() != 1 )
        {
            m_Status = errorScript;
            return false;
        }

        return true;
    }

    inline void Executor::ExecuteOneOp()
    {
        const Operation& op = m_Code->GetOperation( m_OperationPos++ );

        switch ( op.m_OpCode )
        {
            case Operation::endScript:
                throw executedSuccessfully;

            case Operation::pushCurrent:
                PushStack( Value( m_Current ) );
                break;

            case Operation::loadMem:
            {
                unsigned long long address = GetStack().GetUnsignedInt() + op.m_ConstArgument;
                PopStack();
                PushAddress( op, address );
                break;
            }

            case Operation::loadArg:
                PushArgument( op, op.m_ConstArgument );
                break;

            case Operation::add:
                BinaryOp< Functors::Adder >( op );
                break;

            case Operation::mul:
                BinaryOp< Functors::Multiplier >( op );
                break;

            case Operation::div:
                BinaryOp< Functors::Divider >( op );
                break;

            case Operation::mod:
                BinaryOp< Functors::Moduler >( op );
                break;

            case Operation::neg:
                UnaryOp< Functors::Negation >( op );
                break;

            case Operation::conv:
                UnaryOp2< Functors::Convert >( op );
                break;

            case Operation::fnSin:
                ReplaceStack( Value( std::sin( GetStack().GetFloat() ) ) );
                break;

            case Operation::fnCos:
                ReplaceStack( Value( std::cos( GetStack().GetFloat() ) ) );
                break;

            case Operation::fnTan:
                ReplaceStack( Value( std::tan( GetStack().GetFloat() ) ) );
                break;

            case Operation::fnLn:
                ReplaceStack( Value( std::log( GetStack().GetFloat() ) ) );
                break;

            case Operation::fnPow:
            {
                Value p = GetStack(); PopStack();
                ReplaceStack( Value( std::pow( GetStack().GetFloat(), p.GetFloat() ) ) );
                break;
            }

            default:
                throw errorOperation;
        }
    }

    inline void Executor::PushArgument( const Operation& /*op*/, long long address )
    {
        PushStack( m_Code->GetArgument( address ) );
    }

    inline void Executor::PushAddress( const Operation& op, long long address )
    {
        switch ( op.m_Mod1 )
        {
            case Operation::modChar:       PushStack( Content< char               >(address) ); break;
            case Operation::modByte:       PushStack( Content< unsigned char      >(address) ); break;
            case Operation::modShort:      PushStack( Content< short              >(address) ); break;
            case Operation::modWord:       PushStack( Content< unsigned short     >(address) ); break;
            case Operation::modLong:       PushStack( Content< long               >(address) ); break;
            case Operation::modDword:      PushStack( Content< unsigned long      >(address) ); break;
            case Operation::modLongLong:   PushStack( Content< long long          >(address) ); break;
            case Operation::modQword:      PushStack( Content< unsigned long long >(address) ); break;
            case Operation::modFloat:      PushStack( Content< float              >(address) ); break;
            case Operation::modDouble:     PushStack( Content< double             >(address) ); break;
            case Operation::modLongDouble: PushStack( Content< long double        >(address) ); break;
            default: throw errorOperation;
        }
    }

    template< template<typename> class T >
    inline void Executor::BinaryOp( const Operation& op )
    {
        switch ( op.m_Mod1 )
        {
            case Operation::modLongLong:
                GetStack(1) = Value( T<long long>()( GetStack(1).GetSignedInt(), GetStack(0).GetSignedInt() ) );
                break;

            case Operation::modQword:
                GetStack(1) = Value( T<unsigned long long>()( GetStack(1).GetUnsignedInt(), GetStack(0).GetUnsignedInt() ) );
                break;

            case Operation::modLongDouble:
                GetStack(1) = Value( T<long double>()( GetStack(1).GetFloat(), GetStack(0).GetFloat() ) );
                break;

            default:
                throw errorOperation;
        }

        PopStack();
    }

    template< template<typename> class T >
    inline void Executor::UnaryOp( const Operation& op )
    {
        switch ( op.m_Mod1 )
        {
            case Operation::modLongLong:
                GetStack() = Value( T<long long>()( GetStack().GetSignedInt() ) );
                break;

            case Operation::modQword:
                GetStack() = Value( T<unsigned long long>()( GetStack().GetUnsignedInt() ) );
                break;

            case Operation::modLongDouble:
                GetStack() = Value( T<long double>()( GetStack().GetFloat() ) );
                break;

            default:
                throw errorOperation;
        }
    }

    template< template<typename,typename> class T >
    inline void Executor::UnaryOp2( const Operation& op )
    {
        switch ( op.m_Mod2 )
        {
            case Operation::modLongLong:
                UnaryOp2Helper<T>( op, GetStack().GetSignedInt() );
                break;

            case Operation::modQword:
                UnaryOp2Helper<T>( op, GetStack().GetUnsignedInt() );
                break;

            case Operation::modLongDouble:
                UnaryOp2Helper<T>( op, GetStack().GetFloat() );
                break;

            default:
                throw errorOperation;
        }
    }


    template<  template<typename,typename> class T1, typename T2 >
    inline void Executor::UnaryOp2Helper( const Operation& op, T2 value )
    {
        switch ( op.m_Mod1 )
        {
            case Operation::modLongLong:
                GetStack() = Value( T1< T2, long long >()( value ) );
                break;

            case Operation::modQword:
                GetStack() = Value( T1< T2, unsigned long long >()( value ) );
                break;

            case Operation::modLongDouble:
                GetStack() = Value( T1< T2, long double >()( value ) );
                break;

            default:
                throw errorOperation;
        }
    }
}
