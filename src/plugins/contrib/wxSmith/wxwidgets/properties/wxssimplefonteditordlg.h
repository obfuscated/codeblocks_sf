/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision$
* $Id$
* $HeadURL$
*/

#ifndef WXSSIMPLEFONTEDITORDLG_H
#define WXSSIMPLEFONTEDITORDLG_H

#include "wxsfontproperty.h"

//(*Headers(wxsSimpleFontEditorDlg)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

class wxsSimpleFontEditorDlg: public wxDialog
{
	public:

		wxsSimpleFontEditorDlg(wxWindow* parent,wxsFontData& Data,wxWindowID id = -1);
		virtual ~wxsSimpleFontEditorDlg();

		//(*Identifiers(wxsSimpleFontEditorDlg)
		static const long ID_STATICTEXT1;
		static const long ID_STATICLINE2;
		static const long ID_BUTTON1;
		static const long ID_BUTTON3;
		static const long ID_STATICLINE1;
		static const long ID_BUTTON2;
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_STATICTEXT3;
		//*)

	private:

		//(*Handlers(wxsSimpleFontEditorDlg)
		void OnOK(wxCommandEvent& event);
		void OnButton1Click(wxCommandEvent& event);
		void OnButton2Click(wxCommandEvent& event);
		void OnButton3Click(wxCommandEvent& event);
		//*)

		//(*Declarations(wxsSimpleFontEditorDlg)
		wxBoxSizer* BoxSizer4;
		wxStaticBoxSizer* StaticBoxSizer2;
		wxStaticText* StaticText2;
		wxButton* Button1;
		wxTextCtrl* TestArea;
		wxFlexGridSizer* FlexGridSizer2;
		wxBoxSizer* BoxSizer2;
		wxStaticText* StaticText3;
		wxButton* Button2;
		wxStaticLine* StaticLine2;
		wxStaticText* FontDescription;
		wxButton* Button3;
		wxStaticLine* StaticLine1;
		wxBoxSizer* BoxSizer1;
		wxStaticBoxSizer* StaticBoxSizer1;
		wxFlexGridSizer* FlexGridSizer1;
		wxBoxSizer* BoxSizer3;
		wxStdDialogButtonSizer* StdDialogButtonSizer1;
		//*)

		wxsFontData& m_Data;
		wxsFontData  m_WorkingCopy;

		void UpdateFontDescription();

		DECLARE_EVENT_TABLE()
};

#endif
