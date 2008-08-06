/*
* This file is part of HexEditor plugin for Code::Blocks Studio
* Copyright (C) 2008 Bartlomiej Swiecki
*
* HexEditor plugin is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* HexEditor pluging is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision:$
* $Id:$
* $HeadURL:$
*/

#ifndef EXPRESSIONTESTER_H
#define EXPRESSIONTESTER_H

//(*Headers(ExpressionTester)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

#include "FileContentBase.h"

class ExpressionTester: public wxDialog
{
	public:

		ExpressionTester( wxWindow* parent, FileContentBase* content, FileContentBase::OffsetT current );
		virtual ~ExpressionTester();

	private:

		//(*Declarations(ExpressionTester)
		wxStaticText* StaticText2;
		wxButton* Button1;
		wxStaticText* m_Result;
		wxTextCtrl* m_Expr;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxButton* Button2;
		wxStaticText* StaticText4;
		wxStaticText* m_Parsing;
		wxTextCtrl* m_Dump;
		//*)

		//(*Identifiers(ExpressionTester)
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_STATICTEXT3;
		static const long ID_STATICTEXT4;
		static const long ID_STATICTEXT5;
		static const long ID_STATICTEXT6;
		static const long ID_TEXTCTRL2;
		static const long ID_BUTTON2;
		static const long ID_BUTTON1;
		//*)

		//(*Handlers(ExpressionTester)
		void OnButton1Click(wxCommandEvent& event);
		void OnButton2Click(wxCommandEvent& event);
		//*)

		FileContentBase* m_Content;
		FileContentBase::OffsetT m_Current;

		void BuildContent(wxWindow* parent);

		DECLARE_EVENT_TABLE()
};

#endif
