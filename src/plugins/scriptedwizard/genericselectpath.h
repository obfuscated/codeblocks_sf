/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#ifndef GENERICSELECTPATH_H
#define GENERICSELECTPATH_H

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(GenericSelectPath)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/button.h>
//*)

class wxString;

class GenericSelectPath: public wxPanel
{
	public:

		GenericSelectPath(wxWindow* parent,wxWindowID id = -1);
		virtual ~GenericSelectPath();

		// use this because it adjusts the sizer too
		void SetDescription(const wxString& descr)
		{
		    lblDescr->SetLabel(descr);

            GetSizer()->Fit(this);
            GetSizer()->SetSizeHints(this);
		}

		//(*Identifiers(GenericSelectPath)
		static const long ID_STATICTEXT1;
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL1;
		static const long ID_BUTTON1;
		//*)

		//(*Handlers(GenericSelectPath)
		//*)

		//(*Declarations(GenericSelectPath)
		wxButton* btnBrowse;
		wxStaticText* lblDescr;
		wxBoxSizer* BoxSizer2;
		wxTextCtrl* txtFolder;
		wxBoxSizer* BoxSizer1;
		wxStaticText* lblLabel;
		//*)

	private:

		DECLARE_EVENT_TABLE()
};

#endif
