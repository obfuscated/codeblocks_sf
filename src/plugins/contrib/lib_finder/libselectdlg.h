/*
* This file is part of lib_finder plugin for Code::Blocks Studio
* Copyright (C) 2007  Bartlomiej Swiecki
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

#ifndef LIBSELECTDLG_H
#define LIBSELECTDLG_H

//(*Headers(LibSelectDlg)
#include "scrollingdialog.h"
class wxStdDialogButtonSizer;
class wxCheckBox;
class wxRadioButton;
class wxStaticText;
class wxBoxSizer;
class wxStaticBoxSizer;
class wxCheckListBox;
//*)

class LibSelectDlg: public wxScrollingDialog
{
	public:

		LibSelectDlg(wxWindow* parent,const wxArrayString& Names,bool addOnly);
		virtual ~LibSelectDlg();

        /** \brief Set list of selected items */
		void SetSelections(const wxArrayInt& Selections);

        /** \brief Get lis of selected items */
		wxArrayInt GetSelections();

		/** \brief Check if we should setup global variables */
		bool GetSetupGlobalVars();

		/** \brief Check if we should not clear previous results */
		bool GetDontClearPrevious();

		/** \brief Check if we should clear previous results only for selected items */
		bool GetClearSelectedPrevious();

		/** \brief Check if we should clear all previous results */
		bool GetClearAllPrevious();

	private:

		//(*Declarations(LibSelectDlg)
		wxStaticText* StaticText1;
		wxCheckListBox* m_Libraries;
		wxRadioButton* m_ClearAll;
		wxCheckBox* m_SetupGlobalVars;
		wxRadioButton* m_DontClear;
		wxRadioButton* m_ClearSelected;
		//*)

		//(*Identifiers(LibSelectDlg)
		static const long ID_STATICTEXT1;
		static const long ID_CHECKLISTBOX1;
		static const long ID_RADIOBUTTON1;
		static const long ID_RADIOBUTTON2;
		static const long ID_RADIOBUTTON3;
		static const long ID_CHECKBOX1;
		//*)

		//(*Handlers(LibSelectDlg)
		void OnOk(wxCommandEvent& event);
		//*)

		bool m_AddOnly;

		DECLARE_EVENT_TABLE()
};

#endif
