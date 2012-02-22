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

#ifndef WXMEDIACTRL_H
#define WXMEDIACTRL_H

#if wxUSE_MEDIACTRL

#include "../wxswidget.h"

/** \brief Class for wxMediaCtrl widget */
class wxsMediaCtrl: public wxsWidget
{
    public:

        wxsMediaCtrl(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode();
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);

        wxString 			m_sMediaFile;			//!< The media file path or URI.
		wxString 			m_sProxy;				//!< Proxy URI.
        bool					m_bPlay;					//!< Play the animation.
        long					m_iControls;				//!< wxMediaCtrlPlayerControls enum value.
		long					m_iVolume;				//!< The volume level. This value is divided by 10 to obtain the actual level.
//		wxString 			m_sBackend;			//!< The media back-end.
//		wxValidator 		m_validator;				//!< The control validator.

};

#endif // wxUSE_MEDIACTRL

#endif // WXMEDIACTRL_H
