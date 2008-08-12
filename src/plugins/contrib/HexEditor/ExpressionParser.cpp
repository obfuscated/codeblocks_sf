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

#include "ExpressionParser.h"

#include <wx/intl.h>

namespace Expression
{

    const Parser::resType Parser::resNone;
    const Parser::resType Parser::resSignedInt;
    const Parser::resType Parser::resUnsignedInt;
    const Parser::resType Parser::resFloat;


    Parser::Parser()
    {
    }

    Parser::~Parser()
    {
    }

    wxArrayString Parser::GetSuggestions(const wxString& expression, int pos)
    {
        // TODO: Code this
        return wxArrayString();
    }

    wxString Parser::GetHelpString()
    {
        return _(
        "Recognized operators: +, -, *, /, %, ()\n"
        "Available constants: PI, E\n"
        "Current location in the data: @\n"
        "Reading at given offset:\n"
        "    byte( <offset> ) - read unsigned byte\n"
        "    char( <offset> ) - read signed byte\n"
        "    word) <offset> ) - read unsigned word\n"
        "    short( <offset> ) - read signed word\n"
        "    dword( <offset> ) - read unsigned dword\n"
        "    long( <offset> ) - read signed dword\n"
        "    qword( <offset> ) - read unsigned qword\n"
        "    llong( <offset> ) - read signed qword\n"
        "    float( <offset> ) - read float\n"
        "    double( <offset> ) - read double\n"
        "    ldouble( <offset> ) - read long double\n"
        "\n"
        "Exapmle:\n"
        "    word( 4 * dword( @ ) + 128 )\n"
        "  This code will read dword value at current cursor\n"
        "  position, multiply it by 4 and add 128 to it,\n"
        "  the result will be used as address to read word value"
        );
    }

    bool Parser::Parse(const wxString& expression, Preprocessed& output)
    {
        m_Output = &output;
        m_ErrorDesc.Clear();
        m_ErrorPos = -1;
        m_StartPos = expression.c_str();
        m_CurrentPos = m_StartPos;
        m_TreeStack.clear();
        m_PiArg = -1;
        m_EArg = -1;
        output.Clear();

        try
        {
            Parse();
            assert( m_TreeStack.size() == 1 );

            ParseTree* tree = PopTreeStack();

            GenerateCode( tree );

            Operation op;
            op.m_OpCode = Operation::endScript;
            op.m_Mod1 = Operation::modNone;
            op.m_Mod2 = Operation::modNone;
            op.m_ConstArgument = 0;

            m_Output->PushOperation( op );

            delete tree;
            return true;
        }
        catch ( bool )
        {
            for ( size_t i=0; i<m_TreeStack.size(); i++ )
            {
                delete m_TreeStack[i];
            }
            m_TreeStack.clear();
            return false;
        }
    }

    void Parser::Parse()
    {
        EatWhite();

        Expression();

        if ( *m_CurrentPos )
        {
            Error( wxString::Format( _("Unexpected character '%c'"), *m_CurrentPos ) );
        }
    }

    void Parser::Expression()
    {
        Add();
    }

    void Parser::Add()
    {
        Mult();
        for (;;)
        {
            if ( Match( _T('+') ) )
            {
                Mult();
                AddOp2( Operation::add );
            }
            else if ( Match( _T('-') ) )
            {
                Mult();
                AddOp1( Operation::neg, TopAfterNeg() );
                AddOp2( Operation::add );
            }
            else
            {
                break;
            }
        }
    }

    void Parser::Mult()
    {
        Unary();

        for (;;)
        {
            if ( Match( _T('*') ) )
            {
                Unary();
                AddOp2( Operation::mul );
            }
            else if ( Match( _T('/') ) )
            {
                Unary();
                AddOp2( Operation::div );
            }
            else if ( Match( _T('%') ) )
            {
                Unary();
                AddOp2( Operation::mod, ModResult2Top() );
            }
            else
            {
                break;
            }
        }
    }

    void Parser::Unary()
    {
        if ( Match( _T('+') ) )
        {
            Unary();
        }
        else if ( Match( _T('-') ) )
        {
            Unary();
            AddOp1( Operation::neg, TopAfterNeg() );
        }
        else
        {
            Primary();
        }
    }

    void Parser::Primary()
    {
        if ( Match( _T('(') ) )
        {
            Expression();
            Require( _T(')') );
        }
        else if ( Match( _T('@') ) )
        {
            AddOp( 0, Operation::pushCurrent, resUnsignedInt, resNone, resNone );
        }
        else if ( Number() )
        {
            EatWhite();
        }
        else if ( Const() )
        {
            EatWhite();
        }
        else if ( Memory() )
        {
        }
        else
        {
            Error( _("Number, '@', constant, memory read or '(' expected") );
        }
    }

    bool Parser::Number()
    {
        if ( !wxIsdigit( Get() ) ) return false;

        long long value = 0;
        while ( wxIsdigit( Get() ) )
        {
            value = value * 10 + Get() - _T('0');
            Next();
        }

        AddOp(
            0,
            Operation::loadArg,
            resSignedInt,
            resNone,
            resSignedInt,
            Operation::modNone,
            AddArg( value ) );

        return true;
    }

    bool Parser::Const()
    {
        if ( Match( _T("PI") ) )
        {
            ConstArg( m_PiArg, 3.1415926535897932384626433832795028841971L );
            return true;
        }

        if ( Match( _T("E") ) )
        {
            ConstArg( m_EArg,  2.7182818284590452353602874713526624977572L );
            return true;
        }

        return false;
    }

    inline bool Parser::Memory()
    {
        Operation::modifier argMod =
            Match( _T( "char"     ) ) ? Operation::modChar        :
            Match( _T( "byte"     ) ) ? Operation::modByte        :
            Match( _T( "short"    ) ) ? Operation::modShort       :
            Match( _T( "word"     ) ) ? Operation::modWord        :
            Match( _T( "long"     ) ) ? Operation::modLong        :
            Match( _T( "dword"    ) ) ? Operation::modDword       :
            Match( _T( "llong"    ) ) ? Operation::modLongLong    :
            Match( _T( "qword"    ) ) ? Operation::modQword       :
            Match( _T( "float"    ) ) ? Operation::modFloat       :
            Match( _T( "double"   ) ) ? Operation::modDouble      :
            Match( _T( "ldouble"  ) ) ? Operation::modLongDouble  :
                                  Operation::modNone        ;

        if ( argMod == Operation::modNone )
        {
            return false;
        }

        resType result = resNone;
        switch ( argMod )
        {
            case Operation::modChar       :
            case Operation::modShort      :
            case Operation::modLong       :
            case Operation::modLongLong   :
                result = resSignedInt;
                break;

            case Operation::modByte       :
            case Operation::modWord       :
            case Operation::modDword      :
            case Operation::modQword      :
                result = resUnsignedInt;
                break;

            case Operation::modFloat      :
            case Operation::modDouble     :
            case Operation::modLongDouble :
                result = resFloat;
                break;

            default:
                assert( false );
        }

        Require( _T("(") );
        Expression();
        Require( _T(")") );

        AddOp( 1, Operation::loadMem, result, resUnsignedInt, argMod );

        return true;
    }


    inline void Parser::ConstArg( int& argNum, long double value )
    {
        if ( argNum < 0 )
        {
            argNum = AddArg( value );
        }

        AddOp(
            0,
            Operation::loadArg,
            resFloat,
            resNone,
            resFloat,
            Operation::modNone,
            argNum );
    }


    void Parser::AddOp( int subArgs, Operation::opCode op, resType producedType, resType argumentsType, Operation::modifier mod1, Operation::modifier mod2, short opConst )
    {
        ParseTree* node = new ParseTree;
        node->m_Op.m_OpCode = op;
        node->m_Op.m_Mod1 = mod1;
        node->m_Op.m_Mod2 = mod2;
        node->m_Op.m_ConstArgument = opConst;
        node->m_OutType = producedType;
        node->m_InType = argumentsType;

        if ( subArgs > 1 ) node->m_SecondSub = PopTreeStack();
        if ( subArgs > 0 ) node->m_FirstSub  = PopTreeStack();

        PushTreeStack( node );
    }

    inline void Parser::AddOp1( Operation::opCode op, resType type )
    {
        AddOp( 1, op, type, type, type );
    }

    inline void Parser::AddOp1( Operation::opCode op )
    {
        AddOp1( op, HigherType2Top() );
    }

    inline void Parser::AddOp2( Operation::opCode op, resType type )
    {
        AddOp( 2, op, type, type, type );
    }

    inline void Parser::AddOp2( Operation::opCode op )
    {
        AddOp2( op, HigherType2Top() );
    }


    inline Parser::resType Parser::TopType( int pos )
    {
        assert( (int)m_TreeStack.size() > pos );
        return (resType)m_TreeStack[ m_TreeStack.size() - pos - 1 ]->m_OutType;
    }


    inline Parser::resType Parser::HigherType2Top()
    {
        return HigherType( TopType(0), TopType(1) );
    }

    inline Parser::resType Parser::TopAfterNeg()
    {
        return TopType(0) == resUnsignedInt ? resSignedInt : TopType(0);
    }

    inline Parser::resType Parser::ModResult2Top()
    {
        resType t1 = TopType(0);
        resType t2 = TopType(1);
        return ( t1 == resUnsignedInt && t2 == resUnsignedInt ) ? resUnsignedInt : resSignedInt;
    }


    inline wxChar Parser::Get()
    {
        return m_CurrentPos[ 0 ];
    }

    inline wxChar Parser::Get( int shift )
    {
        return m_CurrentPos[ shift ];
    }

    inline bool Parser::Match( wxChar ch )
    {
        if ( Get() == ch )
        {
            Next();
            EatWhite();
            return true;
        }
        return false;
    }

    inline bool Parser::Match( const wxChar* text )
    {
        unsigned i = 0;
        for ( ; text[i]; i++ )
        {
            if ( Get(i) != text[i] )
            {
                return false;
            }
        }

        m_CurrentPos += i;
        EatWhite();
        return true;
    }

    inline void Parser::Require( wxChar ch )
    {
        if ( !Match( ch ) ) Error( wxString::Format( _("'%c' expected"), ch ) );
    }

    inline void Parser::Require( const wxChar* text )
    {
        if ( !Match( text ) ) Error( wxString::Format( _("'%s' expected"), text ) );
    }

    inline void Parser::Error( const wxString& desc )
    {
        m_ErrorDesc = desc;
        throw false;
    }

    inline void Parser::EatWhite( )
    {
        while ( wxIsspace( Get() ) ) Next();
    }

    inline void Parser::Next()
    {
        m_CurrentPos++;
    }

    inline Parser::resType Parser::HigherType( resType t1, resType t2 )
    {
        if ( t1==resFloat || t2==resFloat )
        {
            return resFloat;
        }

        if ( t1==resSignedInt || t2==resSignedInt )
        {
            return resSignedInt;
        }

        return resUnsignedInt;
    }

    void Parser::GenerateCode( ParseTree* tree )
    {
        GenerateCodeAndConvert( tree->m_FirstSub,  tree->m_InType );
        GenerateCodeAndConvert( tree->m_SecondSub, tree->m_InType );
        m_Output->PushOperation( tree->m_Op );
    }

    void Parser::GenerateCodeAndConvert( ParseTree* tree, resType type )
    {
        if ( !tree ) return;
        GenerateCode( tree );

        if ( tree->m_OutType != type )
        {
            // We have to convert the result into new type
            Operation op;
            op.m_OpCode = Operation::conv;
            op.m_Mod1 = type;
            op.m_Mod2 = tree->m_OutType;
            op.m_ConstArgument = 0;

            m_Output->PushOperation( op );
        }
    }

}
