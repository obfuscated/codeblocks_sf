/** \file wxsfontdialog.h
*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2010  Gary Harris
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
*/

#ifndef WXSFONTDIALOG_H
#define WXSFONTDIALOG_H

#include "../wxstool.h"

/** \brief Class for wxFontDialog dialogue. */
class wxsFontDialog: public wxsTool
{
    public:

        wxsFontDialog(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode();
        virtual void OnEnumToolProperties(long Flags);

        bool                    m_bAllowSymbols;    //!< Allow symbol fonts to be selected..
        bool                    m_bEnableEffects;        //!< Enable effects.
        bool                    m_bShowHelp;            //!< Show the help button.
        long                    m_iMinSize;                //!< The minimum allowed font size.
        long                    m_iMaxSize;                //!< The maximum allowed font size.
        wxsColourData    m_cdColour;                //!< The font colour..
};

#endif
