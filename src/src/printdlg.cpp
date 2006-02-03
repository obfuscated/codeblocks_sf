/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include <sdk.h>
#include "printdlg.h"
#include <manager.h>
#include <configmanager.h>
#include <editormanager.h>
#include <cbeditor.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/radiobox.h>

PrintDialog::PrintDialog(wxWindow* parent)
{
	//ctor
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgPrint"));

	cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
	if (ed)
	{
        bool hasSel = ed->GetControl()->GetSelectedText().Length();
        XRCCTRL(*this, "rbScope", wxRadioBox)->SetSelection(hasSel ? 0 : 1);
	}
	else
        XRCCTRL(*this, "rbScope", wxRadioBox)->SetSelection(1);

    int mode = Manager::Get()->GetConfigManager(_T("app"))->ReadInt(_T("/print_mode"), 1);
    XRCCTRL(*this, "rbColorMode", wxRadioBox)->SetSelection(mode);
}

PrintDialog::~PrintDialog()
{
	//dtor
}

PrintScope PrintDialog::GetPrintScope()
{
    return (PrintScope)XRCCTRL(*this, "rbScope", wxRadioBox)->GetSelection();
}

PrintColorMode PrintDialog::GetPrintColorMode()
{
    return (PrintColorMode)XRCCTRL(*this, "rbColorMode", wxRadioBox)->GetSelection();
}

void PrintDialog::EndModal(int retCode)
{
    if (retCode == wxID_OK)
    {
        int mode = XRCCTRL(*this, "rbColorMode", wxRadioBox)->GetSelection();
        Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/print_mode"), (int)mode);
    }
    return wxDialog::EndModal(retCode);
}
