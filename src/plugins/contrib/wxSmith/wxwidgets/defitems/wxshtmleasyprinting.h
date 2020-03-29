/*
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

#ifndef WXSHTMLEASYPRINTING_H
#define WXSHTMLEASYPRINTING_H

#include "../wxstool.h"

/** \brief Class for wxHyperlinkCtrl widget */
class wxsHtmlEasyPrinting: public wxsTool
{
    public:

        wxsHtmlEasyPrinting(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode();
        virtual void OnEnumToolProperties(long Flags);

        wxString     m_sHeader;                    //!< The header text.
        wxString     m_sFooter;                    //!< The footer text.
        long            m_iHeaderPages;        //!< The pages on which the header should appear.
        long            m_iFooterPages;        //!< The pages on which the footer should appear.
};


#endif // WXSHTMLEASYPRINTING_H
