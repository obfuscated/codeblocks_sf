/** \file wxsrichtextstylelistctrl.h
*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2010 Gary Harris
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

#ifndef WXSRICHTEXTSTYLELISTCTRL_H
#define WXSRICHTEXTSTYLELISTCTRL_H

#include "../wxswidget.h"

/** \brief Class for wxsRichTextStyleListCtrl widget
 *    \note If you set the control or a style sheet for this control in wxSmith you must ensure that it has been declared
 *    beforehand, or use SetRichTextCtrl() and SetStyleSheet(). If set via wxSmith, these functions are called late enough
 *    that you can using the style sheet declared for wxRichTextStyleOrganiserDialog. Alternatively, you could declare one
 *    manually before the wxSmith-managed code block.
*/
class wxsRichTextStyleListCtrl: public wxsWidget
{
    public:

        wxsRichTextStyleListCtrl(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode();
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);

        wxString    m_sControl;                            //!< The associated wxRichTextCtrl.
        wxString    m_sStyleSheet;                    //!< The associated style sheet.
        int                m_iStyleType;                        //!< The style type to display.
};

#endif
