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

#ifndef CHARACTERVIEW_H
#define CHARACTERVIEW_H

#include "HexEditViewBase.h"

/** \brief Class representing the character view in hes editor */
class CharacterView: public HexEditViewBase
{
    public:

        /** \brief Ctor */
        CharacterView( HexEditPanel* panel );

    protected:

        /** \brief Called when switching the active state */
        virtual void OnActivate( bool becomesActive );

        /** \brief Called when changing current cursor offset */
        virtual void OnOffsetChange(
            OffsetT screenStartOffset,
            OffsetT currentOffset,
            OffsetT blockStart,
            OffsetT blockEnd );

        /** \brief Called when character is sent to this view */
        virtual void OnProcessChar( wxChar ch );

        /** \brief Called when moving left */
        virtual void OnMoveLeft();

        /** \brief Called when moving right */
        virtual void OnMoveRight();

        /** \brief Called when moving up */
        virtual void OnMoveUp();

        /** \brief Called when moving down */
        virtual void OnMoveDown();

        /** \brief Called when there's need to fill line buffer for this content */
        virtual void OnPutLine( OffsetT startOffset, HexEditLineBuffer& buff, char* content, int bytes );

        /** \brief Get block sizes */
        virtual void OnGetBlockSizes( int& blockLength, int& blockBytes, int& spacing );

        /** \brief Calculate offset from the beginning of the line at given position */
        virtual int OnGetOffsetFromColumn( int column, int& positionFlags );

        /** \brief Convert character for the one that will be displayable for sure */
        inline char Strip( char ch );

};

#endif
