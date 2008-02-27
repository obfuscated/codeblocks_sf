/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef SELECTINCLUDEFILE_H
#define SELECTINCLUDEFILE_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
	//(*Headers(SelectIncludeFile)
	#include <wx/button.h>
	#include <wx/dialog.h>
	#include <wx/intl.h>
	#include <wx/listbox.h>
	#include <wx/sizer.h>
	#include <wx/xrc/xmlres.h>
	//*)
#endif

class SelectIncludeFile: public wxDialog
{
	public:

		SelectIncludeFile(wxWindow* parent,wxWindowID id = -1);
		virtual ~SelectIncludeFile();
		void AddListEntries(const wxArrayString& IncludeFile);
		wxString GetIncludeFile() const {return m_SelectedIncludeFile;}

		//(*Identifiers(SelectIncludeFile)
		//*)

	private:

		//(*Handlers(SelectIncludeFile)
		void OnOk(wxCommandEvent& event);
		void OnCancel(wxCommandEvent& event);
		//*)

		//(*Declarations(SelectIncludeFile)
		wxListBox* ListBox1;
		//*)

		wxString m_SelectedIncludeFile;
		DECLARE_EVENT_TABLE()
};

#endif // SELECTINCLUDEFILE_H
