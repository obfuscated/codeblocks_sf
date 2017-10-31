/*
* This file is part of HexEditor plugin for Code::Blocks Studio
* Copyright (C) 2009 Bartlomiej Swiecki
*
* HexEditor plugin is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Frsee Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* HexEditor pluging is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with HexEditor. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

//(*Headers(SearchDialog)
#include <wx/combobox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/radiobut.h>
#include "scrollingdialog.h"
#include <wx/radiobox.h>
//*)

#include "FileContentBase.h"

class SearchDialog: public wxScrollingDialog
{
	public:

		SearchDialog( wxWindow* parent, FileContentBase* content, FileContentBase::OffsetT current );
		virtual ~SearchDialog();
		inline FileContentBase::OffsetT GetOffset() { return m_Offset; }

	private:

		void BuildContent(wxWindow* parent);

		//(*Identifiers(SearchDialog)
		static const long ID_COMBOBOX1;
		static const long ID_RADIOBUTTON1;
		static const long ID_RADIOBUTTON2;
		static const long ID_RADIOBUTTON3;
		static const long ID_BUTTON1;
		static const long ID_RADIOBOX2;
		static const long ID_RADIOBOX1;
		//*)

		//(*Declarations(SearchDialog)
		wxRadioButton* m_SearchTypeString;
		wxComboBox* m_SearchValue;
		wxRadioButton* m_SearchTypeExpression;
		wxButton* Button1;
		wxRadioBox* m_Direction;
		wxRadioBox* m_StartFrom;
		wxRadioButton* m_SearchTypeHex;
		//*)

		//(*Handlers(SearchDialog)
		void OnOk( wxCommandEvent& event );
		void OnButton1Click(wxCommandEvent& event);
		//*)

        void SearchAscii( const char* text );
        void SearchHex( const wxChar* text );
		void SearchBuffer( const unsigned char* data, size_t length );
		void SearchExpression( const wxString& expression );

		void NotFound();
		void FoundAt( FileContentBase::OffsetT pos );
		void ReadError();
		void Cancel();

		static int BlockCompare( const unsigned char* searchIn, size_t inLength, const unsigned char* searchFor, size_t forLength, bool backwards );

		FileContentBase*         m_Content;
		FileContentBase::OffsetT m_Offset;
};

#endif
