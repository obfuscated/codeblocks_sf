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
* $Revision$
* $Id$
* $HeadURL$
*/

#include <sdk.h>
#include "debuggeroptionsdlg.h"
#include <configmanager.h>
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/choice.h>

#include "debuggergdb.h"

DebuggerOptionsDlg::DebuggerOptionsDlg(wxWindow* parent, DebuggerGDB* plugin)
    : m_pPlugin(plugin)
{
    //ctor
    wxXmlResource::Get()->LoadPanel(this, parent, _T("dlgDebuggerOptions"));

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("debugger"));
    XRCCTRL(*this, "txtInit",           wxTextCtrl)->SetValue(cfg->Read(_T("init_commands"), wxEmptyString));
    XRCCTRL(*this, "txtLayoutStart",    wxTextCtrl)->SetValue(cfg->Read(_T("layout_start"),  wxEmptyString));
    XRCCTRL(*this, "txtLayoutEnd",      wxTextCtrl)->SetValue(cfg->Read(_T("layout_end"),    wxEmptyString));
    XRCCTRL(*this, "chkAutoBuild",      wxCheckBox)->SetValue(cfg->ReadBool(_T("auto_build"),            true));
    XRCCTRL(*this, "chkWatchArgs",      wxCheckBox)->SetValue(cfg->ReadBool(_T("watch_args"),            true));
    XRCCTRL(*this, "chkWatchLocals",    wxCheckBox)->SetValue(cfg->ReadBool(_T("watch_locals"),          true));
    XRCCTRL(*this, "chkTooltipEval",    wxCheckBox)->SetValue(cfg->ReadBool(_T("eval_tooltip"),          false));
    XRCCTRL(*this, "chkDebugLog",       wxCheckBox)->SetValue(cfg->ReadBool(_T("debug_log"),             false));
    XRCCTRL(*this, "chkAddForeignDirs", wxCheckBox)->SetValue(cfg->ReadBool(_T("add_other_search_dirs"), false));
    XRCCTRL(*this, "chkDoNotRun",       wxCheckBox)->SetValue(cfg->ReadBool(_T("do_not_run"),            false));
    XRCCTRL(*this, "choDisassemblyFlavor", wxChoice)->SetSelection(cfg->ReadInt(_T("disassembly_flavor"), 0));
    XRCCTRL(*this, "txtInstructionSet", wxTextCtrl)->SetValue(cfg->Read(_T("instruction_set"), wxEmptyString));
    XRCCTRL(*this, "spnArrayElems",     wxSpinCtrl)->SetValue(cfg->ReadInt(_T("single_line_array_elem_count"), 8));
}

DebuggerOptionsDlg::~DebuggerOptionsDlg()
{
    //dtor
}

void DebuggerOptionsDlg::OnApply()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("debugger"));
    cfg->Write(_T("init_commands"),         XRCCTRL(*this, "txtInit",           wxTextCtrl)->GetValue());
    cfg->Write(_T("layout_start"),          XRCCTRL(*this, "txtLayoutStart",    wxTextCtrl)->GetValue());
    cfg->Write(_T("layout_end"),            XRCCTRL(*this, "txtLayoutEnd",      wxTextCtrl)->GetValue());
    cfg->Write(_T("auto_build"),            XRCCTRL(*this, "chkAutoBuild",      wxCheckBox)->GetValue());
    cfg->Write(_T("watch_args"),            XRCCTRL(*this, "chkWatchArgs",      wxCheckBox)->GetValue());
    cfg->Write(_T("watch_locals"),          XRCCTRL(*this, "chkWatchLocals",    wxCheckBox)->GetValue());
    cfg->Write(_T("eval_tooltip"),          XRCCTRL(*this, "chkTooltipEval",    wxCheckBox)->GetValue());
    cfg->Write(_T("debug_log"),             XRCCTRL(*this, "chkDebugLog",       wxCheckBox)->GetValue());
    cfg->Write(_T("add_other_search_dirs"), XRCCTRL(*this, "chkAddForeignDirs", wxCheckBox)->GetValue());
    cfg->Write(_T("do_not_run"),            XRCCTRL(*this, "chkDoNotRun",       wxCheckBox)->GetValue());
    cfg->Write(_T("disassembly_flavor"),    XRCCTRL(*this, "choDisassemblyFlavor", wxChoice)->GetSelection());
    cfg->Write(_T("instruction_set"),       XRCCTRL(*this, "txtInstructionSet", wxTextCtrl)->GetValue());
    cfg->Write(_T("single_line_array_elem_count"), XRCCTRL(*this, "spnArrayElems", wxSpinCtrl)->GetValue());

    m_pPlugin->RefreshConfiguration();
}
