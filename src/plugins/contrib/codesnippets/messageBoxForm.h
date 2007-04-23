/*
	This file is part of Code Snippets, a plugin for Code::Blocks
	Copyright (C) 2006 Arto Jonsson
	Copyright (C) 2007 Pecan Heber

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
// RCS-ID: $Id: messageBoxForm.h 58 2007-04-22 04:40:57Z Pecan $

#ifndef __messageBoxForm__
#define __messageBoxForm__

// Define WX_GCH in order to support precompiled headers with GCC compiler.
// You have to create the header "wx_pch.h" and include all files needed
// for compile your gui inside it.
// Then, compile it and place the file "wx_pch.h.gch" into the same
// directory that "wx_pch.h".
#ifdef WX_GCH
#include <wx_pch.h>
#else
#include <wx/wx.h>
#endif

#include <wx/statline.h>

///////////////////////////////////////////////////////////////////////////

#define ID_DEFAULT wxID_ANY // Default

/**
 * Class messageBoxForm
 */
class messageBoxForm : public wxDialog
{
	public:
		wxTextCtrl* m_messageBoxTextCtrl;
		wxStaticLine* m_staticline;
		wxStdDialogButtonSizer* m_sdbSizer;

	public:
		messageBoxForm( wxWindow* parent, int id = -1, wxString title = wxT("Properties"),
                    wxPoint pos = wxDefaultPosition,
                    wxSize size = wxSize( 537,228 ),
                    int dlgStyle = wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxSTAY_ON_TOP,
                    int txtStyle = wxTE_CENTRE|wxTE_MULTILINE|wxTE_READONLY|wxNO_BORDER );

    wxButton* m_pYesButton;
    wxButton* m_pNoButton;
    wxButton* m_pOkButton;
    wxButton* m_pCancelButton;
};

#endif //__messageBoxForm__
