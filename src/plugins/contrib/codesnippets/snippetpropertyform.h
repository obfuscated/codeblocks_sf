/*
	This file is part of CodeSnippets, a plugin for Code::Blocks
	Copyright (C) 2007 Pecan Heber

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/
// RCS-ID: $Id$

#ifndef SNIPPETPROPERTYFORM_H
#define SNIPPETPROPERTYFORM_H


///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jul 24 2006)
// http://wxformbuilder.sourceforge.net/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////


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

#include <wx/button.h>
#include "scrollingdialog.h"
//-#include "edit.h"

///////////////////////////////////////////////////////////////////////////

#define ID_DEFAULT wxID_ANY // Default
#define ID_FILESELECTBUTTON 1000
#define ID_SNIPPETBUTTON 1001

class wxScintilla;
/**
 * Class SnippetPropertyForm
 */
class SnippetPropertyForm : public wxScrollingDialog
{
    friend class CodeSnippetsTreeCtrl;
	private:

	protected:
		wxStaticText*   m_ItemLabelStaticText;
		wxTextCtrl*     m_ItemLabelTextCtrl;
		wxStaticText*   m_SnippetStaticText;
		wxScintilla*    m_SnippetEditCtrl;
		wxButton*       m_SnippetButton;
		wxButton*       m_FileSelectButton;
		wxStdDialogButtonSizer* m_sdbSizer1;

	public:
		SnippetPropertyForm( wxWindow* parent, int id = -1, wxString title = wxT("Properties"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 527,212 ), int style = wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxRESIZE_BORDER );

};

#endif // SNIPPETPROPERTYFORM_H
