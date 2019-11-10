/** \file wxssearchctrl.h
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

#ifndef WXSSEARCHCTRL_H
#define WXSSEARCHCTRL_H

#include "../wxswidget.h"

/** \brief Class for wxSearchCtrl widget */
class wxsSearchCtrl: public wxsWidget
{
    public:

        wxsSearchCtrl(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode();
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);

        wxString             m_sValue;                                    //!< The default text value.
        bool                    m_bShowSearchBtn;                    //!< If true, show the "Search" button.
        bool                    m_bShowCancelBtn;                    //!< If true, show the "Cancel" button.
};

#endif // WXSSEARCHCTRL_H
