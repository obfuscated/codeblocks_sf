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

#include "sdk_precomp.h"
#include "autodetectcompilers.h"

#ifndef CB_PRECOMP
    #include "compiler.h"
    #include "compilerfactory.h"
    #include <wx/button.h>
    #include <wx/intl.h>
    #include <wx/listctrl.h>
    #include <wx/stattext.h>
    #include <wx/string.h>
    #include <wx/xrc/xmlres.h>
#endif

BEGIN_EVENT_TABLE(AutoDetectCompilers, wxDialog)
    EVT_UPDATE_UI(-1, AutoDetectCompilers::OnUpdateUI)
    EVT_BUTTON(XRCID("btnDefault"), AutoDetectCompilers::OnDefaultClick)
END_EVENT_TABLE()

AutoDetectCompilers::AutoDetectCompilers(wxWindow* parent)
{
	//ctor
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgAutoDetectCompilers"));

    wxListCtrl* list = XRCCTRL(*this, "lcCompilers", wxListCtrl);
    if (list)
    {
        list->ClearAll();
		list->InsertColumn(0, _("Compiler"), wxLIST_FORMAT_LEFT, 240);
		list->InsertColumn(1, _("Status"), wxLIST_FORMAT_LEFT, 76);

        for (size_t i = 0; i < CompilerFactory::GetCompilersCount(); ++i)
        {
            Compiler* compiler = CompilerFactory::GetCompiler(i);
            list->InsertItem(list->GetItemCount(), compiler->GetName());

            int idx = list->GetItemCount() - 1;
            if (!compiler->GetParentID().IsEmpty()) // not built-in
                list->SetItem(idx, 1, _("User-defined"));
            else
            {
                wxString path = compiler->GetMasterPath();
                bool detected = compiler->AutoDetectInstallationDir() == adrDetected;
                if (detected && (path.IsEmpty() || path == compiler->GetMasterPath()))
                    list->SetItem(idx, 1, _("Detected"));
                else if (!path.IsEmpty())
                {
                    list->SetItem(idx, 1, _("User-defined"));
                    compiler->SetMasterPath(path);
                }
            }
        }
    }

    XRCCTRL(*this, "lblDefCompiler", wxStaticText)->SetLabel(CompilerFactory::GetDefaultCompiler()->GetName());
}

AutoDetectCompilers::~AutoDetectCompilers()
{
	//dtor
}

void AutoDetectCompilers::OnDefaultClick(wxCommandEvent& /*event*/)
{
    wxListCtrl* list = XRCCTRL(*this, "lcCompilers", wxListCtrl);
    int idx = list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (idx != -1)
    {
        CompilerFactory::SetDefaultCompiler(idx);
        XRCCTRL(*this, "lblDefCompiler", wxStaticText)->SetLabel(CompilerFactory::GetDefaultCompiler()->GetName());
    }
}

void AutoDetectCompilers::OnUpdateUI(wxUpdateUIEvent& event)
{
    wxListCtrl* list = XRCCTRL(*this, "lcCompilers", wxListCtrl);
    bool en = list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED) != -1;
    XRCCTRL(*this, "btnDefault", wxButton)->Enable(en);

    event.Skip();
}
