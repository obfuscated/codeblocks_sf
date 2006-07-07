/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Revision$
* $Id$
* $HeadURL$
*/
#if CB_PRECOMP
    #include "sdk.h"
#else
    #include <wx/intl.h>
    #include <wx/textctrl.h>
    #include <wx/xrc/xmlres.h>
    #include "manager.h"
#endif

#include "enterinfodlg.h"

BEGIN_EVENT_TABLE(EnterInfoDlg, wxDialog)
	EVT_BUTTON(XRCID("btnOK"), EnterInfoDlg::OnOKClick)
END_EVENT_TABLE()

EnterInfoDlg::EnterInfoDlg(const PluginInfo& info)
{
	m_Info = info;
	wxXmlResource::Get()->LoadDialog(this, (wxWindow*)Manager::Get()->GetAppWindow(), _T("dlgPluginInfo"));
}

EnterInfoDlg::~EnterInfoDlg()
{
}
// events

void EnterInfoDlg::OnOKClick(wxCommandEvent& event)
{
	m_Info.title = XRCCTRL(*this, "txtTitle", wxTextCtrl)->GetValue();
	m_Info.version = XRCCTRL(*this, "txtVersion", wxTextCtrl)->GetValue();
	m_Info.description = XRCCTRL(*this, "txtDescription", wxTextCtrl)->GetValue();
	m_Info.author = XRCCTRL(*this, "txtAuthor", wxTextCtrl)->GetValue();
	m_Info.authorEmail = XRCCTRL(*this, "txtEmail", wxTextCtrl)->GetValue();
	m_Info.authorWebsite = XRCCTRL(*this, "txtWebsite", wxTextCtrl)->GetValue();
	m_Info.thanksTo = XRCCTRL(*this, "txtThanksTo", wxTextCtrl)->GetValue();

	EndModal(wxID_OK);
}
