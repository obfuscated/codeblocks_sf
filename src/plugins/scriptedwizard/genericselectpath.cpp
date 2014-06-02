/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#ifndef CB_PRECOMP
    //(*InternalHeadersPCH(GenericSelectPath)
    #include <wx/intl.h>
    #include <wx/string.h>
    //*)
#endif
#include "genericselectpath.h"

//(*IdInit(GenericSelectPath)
const long GenericSelectPath::ID_STATICTEXT1 = wxNewId();
const long GenericSelectPath::ID_STATICTEXT2 = wxNewId();
const long GenericSelectPath::ID_TEXTCTRL1 = wxNewId();
const long GenericSelectPath::ID_BUTTON1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(GenericSelectPath,wxPanel)
	//(*EventTable(GenericSelectPath)
	//*)
END_EVENT_TABLE()

GenericSelectPath::GenericSelectPath(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(GenericSelectPath)
	Create(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("id"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	lblDescr = new wxStaticText(this, ID_STATICTEXT1, _("Please select the location of XXX\non your computer. This is the top-level folder where\nXXX is installed."), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer1->Add(lblDescr, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 8);
	lblLabel = new wxStaticText(this, ID_STATICTEXT2, _("Location of XXX:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	BoxSizer1->Add(lblLabel, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 8);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	txtFolder = new wxTextCtrl(this, ID_TEXTCTRL1, _("Text"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	BoxSizer2->Add(txtFolder, 1, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
	btnBrowse = new wxButton(this, ID_BUTTON1, _("..."), wxDefaultPosition, wxSize(22,22), 0, wxDefaultValidator, _T("ID_BUTTON1"));
	BoxSizer2->Add(btnBrowse, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
	BoxSizer1->Add(BoxSizer2, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 8);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)

	// Set the control's name so that calls to wxWindow::FindWindowByName() succeed...
	txtFolder->SetName(_T("txtFolder"));
}

GenericSelectPath::~GenericSelectPath()
{
}
