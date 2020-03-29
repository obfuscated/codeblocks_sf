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

#include "HexEditLineBuffer.h"

#include <logmanager.h>

HexEditLineBuffer::HexEditLineBuffer( unsigned length )
{
    if ( length )
    {
        m_Buffer = new char[ 2 * length ];
        m_Position = m_Buffer;
        m_End = m_Buffer + 2 * length;
    }
    else
    {
        m_Buffer = 0L;
        m_Position = 0L;
        m_End = 0L;
    }
    Reset();
}

HexEditLineBuffer::~HexEditLineBuffer()
{
    delete[] m_Buffer;
}

void HexEditLineBuffer::Reset( char defaultChar, char defaultStyle )
{
    for ( char* ptr = m_Buffer; ptr < m_End; ptr += 2 )
    {
        ptr[ 0 ] = defaultChar;
        ptr[ 1 ] = defaultStyle;
    }
    m_Position = m_Buffer;
}

void HexEditLineBuffer::PutChar( char ch, char style )
{
    if ( m_Position < m_End )
    {
        *m_Position++ = ch;
        *m_Position++ = style;
    }
}

void HexEditLineBuffer::Draw( wxDC& dc, int x, int y, int fontX, int fontY, wxColour* foregrounds, wxColour* backgrounds )
{
    for ( char* ptr = m_Buffer; ptr < m_End; )
    {
        // Searching for continous block with same style
        wxString str;

        do
        {
            str += wxChar( ptr[ 0 ] );
            ptr += 2;
        }
        while ( ( ptr < m_End ) && ( ptr[1] == ptr[-1] ) );

        char style = ptr[-1];

        dc.SetBrush( backgrounds[ (int)style ] );
        dc.SetPen( backgrounds[ (int)style ] );

        dc.DrawRectangle( x, y, fontX * str.length(), fontY );

        dc.SetPen( foregrounds[ (int)style ] );
        dc.SetTextForeground( foregrounds[ (int)style ] );
        dc.SetTextBackground( backgrounds[ (int)style ] );

        dc.DrawText( str, x, y );
        x += fontX * str.length();
    }
}

