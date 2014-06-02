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
    //(*InternalHeadersPCH(InfoPanel)
    #include <wx/intl.h>
    #include <wx/string.h>
    //*)
#endif // CB_PRECOMP

#include "infopanel.h"


//(*IdInit(InfoPanel)
const long InfoPanel::ID_STATICTEXT1 = wxNewId();
const long InfoPanel::ID_CHECKBOX1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(InfoPanel,wxPanel)
	//(*EventTable(InfoPanel)
	//*)
END_EVENT_TABLE()

InfoPanel::InfoPanel(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(InfoPanel)
	Create(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("id"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	lblIntro = new wxStaticText(this, ID_STATICTEXT1, _("Welcome to the new console application wizard!\n\n\n\n\n\n\n\n\n\n\n\n\n\n"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE, _T("ID_STATICTEXT1"));
	BoxSizer1->Add(lblIntro, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 8);
	chkSkip = new wxCheckBox(this, ID_CHECKBOX1, _("Skip this page next time"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	chkSkip->SetValue(false);
	BoxSizer1->Add(chkSkip, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 8);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)
}

InfoPanel::~InfoPanel()
{
}

