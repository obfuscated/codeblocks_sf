/** \file wxspagesetupdialog.h
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

#ifndef WXSPAGESETUPDIALOG_H
#define WXSPAGESETUPDIALOG_H

#include "../wxstool.h"

/** \brief Class for wxPageSetupDialog dialogue. */
class wxsPageSetupDialog: public wxsTool
{
    public:

        wxsPageSetupDialog(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode();
        virtual void OnEnumToolProperties(long Flags);

        bool        m_bEnableHelp;                //!< Enable the help button.
        bool        m_bEnableMargins;            //!< Enable the margin block.
        bool        m_bEnableOrientation;        //!< Enable the orientation block.
        bool        m_bEnablePaper;                //!< Enable the paper block.
        bool        m_bDefaultInfo;                    //!< Use default info. If true, the dialogue will simply return default printer information (such as orientation) instead of displaying.
        bool        m_bDefaultMinMargins;        //!< Use the default margins.
        bool        m_bEnablePrinter;                //!< Enable printer selection.
        long        m_iMarginLeft;                    //!< The left margin in mm.
        long        m_iMarginTop;                    //!< The top margin in mm.
        long        m_iMarginRight;                //!< The right margin in mm.
        long        m_iMarginBottom;                //!< The bottom margin in mm.
        long        m_iMarginMinLeft;                //!< The minimum value for the left margin, in mm.
        long        m_iMarginMinTop;                //!< The minimum value for the top margin, in mm.
        long        m_iMarginMinRight;            //!< The minimum value for the right margin, in mm.
        long        m_iMarginMinBottom;        //!< The minimum value for the bottom margin, in mm.
        long        m_iPaperID;                        //!< The paper ID.
        long        m_iPaperWidth;                    //!< The paper width.
        long        m_iPaperHeight;                //!< The paper height.

        wxArrayString arrPaperIDs;                //!< Array of paper ID constant names.

};

#endif
