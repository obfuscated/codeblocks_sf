/*
* This file is part of lib_finder plugin for Code::Blocks Studio
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
* $Revision: 4504 $
* $Id: wxsmithpluginregistrants.cpp 4504 2007-10-02 21:52:30Z byo $
* $HeadURL: svn+ssh://byo@svn.berlios.de/svnroot/repos/codeblocks/trunk/src/plugins/contrib/wxSmith/plugin/wxsmithpluginregistrants.cpp $
*/

#ifndef DIRLISTDLG_H
#define DIRLISTDLG_H

#include <wx/arrstr.h>

// NOTE : once the new wxSmith is up and running these includes should be
// moved to the cpp, forward declarations is what we need here

//(*Headers(DirListDlg)
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

class wxCommandEvent;

class DirListDlg: public wxDialog
{
	public:

		DirListDlg(wxWindow* parent,wxWindowID id = -1);
		virtual ~DirListDlg();

		//(*Identifiers(DirListDlg)
		static const long ID_TEXTCTRL1;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		static const long ID_BUTTON3;
		static const long ID_BUTTON4;
		//*)

		wxArrayString Dirs;

	protected:

		//(*Handlers(DirListDlg)
		void OnButton1Click(wxCommandEvent& event);
		void OnButton2Click(wxCommandEvent& event);
		void OnButton3Click(wxCommandEvent& event);
		void OnButton4Click(wxCommandEvent& event);
		//*)

		//(*Declarations(DirListDlg)
		wxButton* Button4;
		wxBoxSizer* BoxSizer2;
		wxButton* Button3;
		wxTextCtrl* DirList;
		wxBoxSizer* BoxSizer1;
		wxFlexGridSizer* FlexGridSizer1;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif
