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

#ifndef WXSANIMATIONCTRL_H
#define WXSANIMATIONCTRL_H

#include "../wxswidget.h"

/** \brief Class for wxAnimationCtrl widget */
class wxsAnimationCtrl: public wxsWidget
{
    public:

        wxsAnimationCtrl(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode();
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);

        wxString             m_sAnimation;            //!< The animation file.
        wxsBitmapData    m_bmpInactive;            //!< The inactivity bitmap.
        bool                    m_bPlay;                    //!< Play the animation.
};

#endif // WXSANIMATIONCTRL_H
