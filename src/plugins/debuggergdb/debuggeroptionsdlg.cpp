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

	XRCCTRL(*this, "txtInit", wxTextCtrl)->SetValue(ConfigManager::Get()->Read(_T("debugger_gdb/init_commands"), _T("")));
	XRCCTRL(*this, "chkWatchArgs", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("debugger_gdb/watch_args"), 1));
	XRCCTRL(*this, "chkWatchLocals", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("debugger_gdb/watch_locals"), 1));
	XRCCTRL(*this, "chkTooltipEval", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("debugger_gdb/eval_tooltip"), 0L));
	XRCCTRL(*this, "chkDebugLog", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("debugger_gdb/debug_log"), 0L));
	XRCCTRL(*this, "chkAddForeignDirs", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("debugger_gdb/add_other_search_dirs"), 0L));
}

DebuggerOptionsDlg::~DebuggerOptionsDlg()
{
	//dtor
}

void DebuggerOptionsDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK)
    {
        ConfigManager::Get()->Write(_T("debugger_gdb/init_commands"), XRCCTRL(*this, "txtInit", wxTextCtrl)->GetValue());
        ConfigManager::Get()->Write(_T("debugger_gdb/watch_args"), XRCCTRL(*this, "chkWatchArgs", wxCheckBox)->GetValue());
        ConfigManager::Get()->Write(_T("debugger_gdb/watch_locals"), XRCCTRL(*this, "chkWatchLocals", wxCheckBox)->GetValue());
        ConfigManager::Get()->Write(_T("debugger_gdb/eval_tooltip"), XRCCTRL(*this, "chkTooltipEval", wxCheckBox)->GetValue());
        ConfigManager::Get()->Write(_T("debugger_gdb/debug_log"), XRCCTRL(*this, "chkDebugLog", wxCheckBox)->GetValue());
        ConfigManager::Get()->Write(_T("debugger_gdb/add_other_search_dirs"), XRCCTRL(*this, "chkAddForeignDirs", wxCheckBox)->GetValue());
    }

    wxDialog::EndModal(retCode);
}
