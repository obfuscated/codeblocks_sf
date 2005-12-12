/*
* This file is part of Code::Blocks, an open-source cross-platform IDE
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
* $Id$
* $Date$
*/

#include <sdk.h>
#include "debuggeroptionsdlg.h"
#include <configmanager.h>
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>

DebuggerOptionsDlg::DebuggerOptionsDlg(wxWindow* parent)
{
	//ctor
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgDebuggerOptions"));

	XRCCTRL(*this, "txtInit", wxTextCtrl)->SetValue(Manager::Get()->GetConfigManager(_T("debugger"))->Read(_T("init_commands"), wxEmptyString));
	XRCCTRL(*this, "chkWatchArgs", wxCheckBox)->SetValue(Manager::Get()->GetConfigManager(_T("debugger"))->ReadBool(_T("watch_args"), true));
	XRCCTRL(*this, "chkWatchLocals", wxCheckBox)->SetValue(Manager::Get()->GetConfigManager(_T("debugger"))->ReadBool(_T("watch_locals"), true));
	XRCCTRL(*this, "chkTooltipEval", wxCheckBox)->SetValue(Manager::Get()->GetConfigManager(_T("debugger"))->ReadBool(_T("eval_tooltip"), false));
	XRCCTRL(*this, "chkDebugLog", wxCheckBox)->SetValue(Manager::Get()->GetConfigManager(_T("debugger"))->ReadBool(_T("debug_log"), false));
	XRCCTRL(*this, "chkAddForeignDirs", wxCheckBox)->SetValue(Manager::Get()->GetConfigManager(_T("debugger"))->ReadBool(_T("add_other_search_dirs"), false));
}

DebuggerOptionsDlg::~DebuggerOptionsDlg()
{
	//dtor
}

void DebuggerOptionsDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK)
    {
        Manager::Get()->GetConfigManager(_T("debugger"))->Write(_T("init_commands"), XRCCTRL(*this, "txtInit", wxTextCtrl)->GetValue());
        Manager::Get()->GetConfigManager(_T("debugger"))->Write(_T("watch_args"), XRCCTRL(*this, "chkWatchArgs", wxCheckBox)->GetValue());
        Manager::Get()->GetConfigManager(_T("debugger"))->Write(_T("watch_locals"), XRCCTRL(*this, "chkWatchLocals", wxCheckBox)->GetValue());
        Manager::Get()->GetConfigManager(_T("debugger"))->Write(_T("eval_tooltip"), XRCCTRL(*this, "chkTooltipEval", wxCheckBox)->GetValue());
        Manager::Get()->GetConfigManager(_T("debugger"))->Write(_T("debug_log"), XRCCTRL(*this, "chkDebugLog", wxCheckBox)->GetValue());
        Manager::Get()->GetConfigManager(_T("debugger"))->Write(_T("add_other_search_dirs"), XRCCTRL(*this, "chkAddForeignDirs", wxCheckBox)->GetValue());
    }

    wxDialog::EndModal(retCode);
}
