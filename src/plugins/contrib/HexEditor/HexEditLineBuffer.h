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

#ifndef HEXEDITLINEBUFFER_H
#define HEXEDITLINEBUFFER_H

#include <wx/dc.h>

enum CharacterStyles
{
    stNormal,   ///< \brief Normal character
    stSelect,   ///< \brief Character in selection
    stCurCar,   ///< \brief Current block, current character
    stCurNon,   ///< \brief Current block, other character
    /******/
    stCount
};

/** \brief Class used to gather informations about one line to render and rendering it */
class HexEditLineBuffer
{
    public:

        /** \brief Ctor
         *  \param length length of the line
         */
        HexEditLineBuffer( unsigned length );

        /** \brief Dctor */
        ~HexEditLineBuffer();

        /** \brief Reset the line and rewind to the beginning of it */
        void Reset( char defaultChar = ' ', char defaultStyle = 0 );

        /** \brief Add one character */
        void PutChar( char ch, char style = 0 );

        /** \brief Add string */
        inline void PutString( const char* ch, char style = 0 ) { while ( *ch ) PutChar( *ch++, style ); }

        /** \brief Draw line buffer on given dc
         *  \param dc context used to draw
         *  \param x start horizontal position
         *  \param y start vertical position
         *  \param fontX width of current font
         *  \param fontY height of current font
         */
        void Draw( wxDC& dc, int x, int y, int fontX, int fontY, wxColour* foregrounds, wxColour* backgrounds );

    private:

        char* m_Buffer;     ///< \brief Beginning of the buffer
        char* m_Position;   ///< \brief Current position within the buffer
        char* m_End;        ///< \brief End of buffer
};

#endif
