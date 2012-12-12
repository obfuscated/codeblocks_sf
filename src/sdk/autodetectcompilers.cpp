/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"
#include "autodetectcompilers.h"

#ifndef CB_PRECOMP
    #include <wx/button.h>
    #include <wx/filename.h>
    #include <wx/intl.h>
    #include <wx/listctrl.h>
    #include <wx/stattext.h>
    #include <wx/string.h>
    #include <wx/xrc/xmlres.h>

    #include "compiler.h"
    #include "compilerfactory.h"
    #include "configmanager.h"
    #include "manager.h"
    #include "macrosmanager.h"
#endif
#include <wx/tooltip.h>

#include "infowindow.h"

BEGIN_EVENT_TABLE(AutoDetectCompilers, wxScrollingDialog)
    EVT_UPDATE_UI(-1, AutoDetectCompilers::OnUpdateUI)
    EVT_BUTTON(XRCID("btnDefault"), AutoDetectCompilers::OnDefaultClick)
END_EVENT_TABLE()

AutoDetectCompilers::AutoDetectCompilers(wxWindow* parent)
{
    //ctor
    wxXmlResource::Get()->LoadObject(this, parent, _T("dlgAutoDetectCompilers"),_T("wxScrollingDialog"));

    wxListCtrl* list = XRCCTRL(*this, "lcCompilers", wxListCtrl);
    if (list)
    {
        list->Connect(wxEVT_MOTION, wxMouseEventHandler(AutoDetectCompilers::OnMouseMotion));
        list->ClearAll();
        list->InsertColumn(0, _("Compiler"), wxLIST_FORMAT_LEFT, 380);
        list->InsertColumn(1, _("Status"),   wxLIST_FORMAT_LEFT, 100);

        for (size_t i = 0; i < CompilerFactory::GetCompilersCount(); ++i)
        {
            Compiler* compiler = CompilerFactory::GetCompiler(i);
            if (!compiler)
                continue;

            list->InsertItem(list->GetItemCount(), compiler->GetName());

            wxString path = compiler->GetMasterPath();
            wxString path_no_macros = compiler->GetMasterPath();
            Manager::Get()->GetMacrosManager()->ReplaceMacros(path_no_macros);

            int idx = list->GetItemCount() - 1;
            int highlight = 0;
            if (path.IsEmpty() && Manager::Get()->GetConfigManager(wxT("compiler"))->Exists(wxT("/sets/") + compiler->GetID() + wxT("/name")))
            {
                // Here, some user-interaction is required not to show this
                // dialog again on each new start-up of C::B.
                list->SetItem(idx, 1, _("Invalid"));
                // So we better clearly HIGHLIGHT this entry:
                highlight = 1;
            }
            else // The compiler is *probably* invalid, but at least a master-path is set
            {
                list->SetItem(idx, 1, _("Not found"));
                highlight = -1;
            }

            // Inspect deeper and probably try to auto-detect invalid compilers:
            if (compiler->GetParentID().IsEmpty()) // built-in compiler
            {
                // Try auto-detection (which is for built-in compilers only)
                bool detected = compiler->AutoDetectInstallationDir() == adrDetected;
                wxString pathDetected( compiler->GetMasterPath() );

                // In case auto-detection was successful:
                if (detected)
                {
                    // No path setup before OR path detected as it was setup before
                    if (path.IsEmpty() || path == pathDetected || path_no_macros == pathDetected)
                        list->SetItem(idx, 1, _("Detected")); // OK
                    else
                        list->SetItem(idx, 1, _("User-defined")); // OK
                    highlight = 0;
                }
                // In case auto-detection failed but a path was setup before:
                else if ( !path.IsEmpty() )
                {
                    // Check, if the master path is valid:
                    if ( wxFileName::DirExists(path_no_macros) && !(path == pathDetected || path_no_macros == pathDetected) )
                    {
                        list->SetItem(idx, 1, _("User-defined")); // OK
                        highlight = 0;
                    }

                    // Assume the user did the setup on purpose, so reset the old settings anyways:
                    compiler->SetMasterPath(path);
                }
            }
            else // no built-in, but user-defined (i.e. copied) compiler
            {
                // Check, if the master path is valid:
                if ( !path.IsEmpty() && wxFileName::DirExists(path_no_macros) )
                {
                    list->SetItem(idx, 1, _("User-defined")); // OK
                    highlight = 0;
                }
            }

            if (highlight == 1)
                list->SetItemBackgroundColour(idx, *wxRED);
            else if (highlight == -1)
                list->SetItemTextColour(idx, *wxLIGHT_GREY);
        }
        // Resize columns so one can read the whole stuff:
        list->SetColumnWidth(0, wxLIST_AUTOSIZE);
        list->SetColumnWidth(1, wxLIST_AUTOSIZE);
    }

    XRCCTRL(*this, "lblDefCompiler", wxStaticText)->SetLabel(CompilerFactory::GetDefaultCompiler()->GetName());
}

AutoDetectCompilers::~AutoDetectCompilers()
{
    //dtor
}

void AutoDetectCompilers::OnDefaultClick(cb_unused wxCommandEvent& event)
{
    wxListCtrl* list = XRCCTRL(*this, "lcCompilers", wxListCtrl);
    int idx = list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (idx != -1)
    {
        CompilerFactory::SetDefaultCompiler(idx);
        XRCCTRL(*this, "lblDefCompiler", wxStaticText)->SetLabel(CompilerFactory::GetDefaultCompiler()->GetName());
    }
}

void AutoDetectCompilers::OnMouseMotion(wxMouseEvent& event)
{
    wxListCtrl* list = XRCCTRL(*this, "lcCompilers", wxListCtrl);
    int flags = 0;
    int idx = list->HitTest(event.GetPosition(), flags);
    wxString txt = wxEmptyString;
    if (idx != wxNOT_FOUND)
    {
        wxListItem itm;
        itm.m_itemId = idx;
        itm.m_col = 1;
        itm.m_mask = wxLIST_MASK_TEXT;
        if (list->GetItem(itm))
            txt = itm.m_text;
    }
    if (txt == wxT("Detected") || txt == wxT("User-defined"))
        txt = CompilerFactory::GetCompiler(idx)->GetMasterPath();
    else
        txt = wxEmptyString;
    if (list->GetToolTip())
    {
        if (txt.IsEmpty())
            list->UnsetToolTip();
        else if (txt != list->GetToolTip()->GetTip())
            list->SetToolTip(txt);
    }
    else if (!txt.IsEmpty())
        list->SetToolTip(txt);
}

void AutoDetectCompilers::OnUpdateUI(wxUpdateUIEvent& event)
{
    wxListCtrl* list = XRCCTRL(*this, "lcCompilers", wxListCtrl);
    bool en = list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED) != -1;
    XRCCTRL(*this, "btnDefault", wxButton)->Enable(en);

    event.Skip();
}
