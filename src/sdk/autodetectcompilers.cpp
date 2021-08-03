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
    #include <wx/checkbox.h>
    #include <wx/filename.h>
    #include <wx/intl.h>
    #include <wx/listctrl.h>
    #include <wx/radiobox.h>
    #include <wx/stattext.h>
    #include <wx/string.h>
    #include <wx/xrc/xmlres.h>

    #include "compiler.h"
    #include "compilerfactory.h"
    #include "configmanager.h"
    #include "logmanager.h"
    #include "manager.h"
    #include "macrosmanager.h"
#endif
#include <wx/tooltip.h>

#include "infowindow.h"

BEGIN_EVENT_TABLE(AutoDetectCompilers, wxScrollingDialog)
    EVT_UPDATE_UI(-1, AutoDetectCompilers::OnUpdateUI)
    EVT_BUTTON(XRCID("btnSetDefaultCompiler"), AutoDetectCompilers::OnDefaultCompilerClick)
    EVT_RADIOBOX(XRCID("rbCompilerShowOptions"), AutoDetectCompilers::OnUdateCompilerListUI)
    EVT_CHECKBOX(XRCID("cbShowCompilerPath"), AutoDetectCompilers::OnUdateCompilerListUI)
END_EVENT_TABLE()

AutoDetectCompilers::AutoDetectCompilers(wxWindow* parent)
{
    //ctor
    wxXmlResource::Get()->LoadObject(this, parent, _T("dlgAutoDetectCompilers"),_T("wxScrollingDialog"));
    XRCCTRL(*this, "wxID_OK", wxButton)->SetDefault();

    wxString defaultCompilerID = CompilerFactory::GetDefaultCompilerID();
    wxListCtrl* list = XRCCTRL(*this, "lcCompilers", wxListCtrl);
    if (list)
    {
        list->Connect(wxEVT_MOTION, wxMouseEventHandler(AutoDetectCompilers::OnMouseMotion));

        for (size_t i = 0; i < CompilerFactory::GetCompilersCount(); ++i)
        {
            Compiler* compiler = CompilerFactory::GetCompiler(i);
            if (!compiler)
                continue;

            CompilerItem cItem;
            cItem.wxsCompilerName = compiler->GetName();
            cItem.wxsStatus = _("Unknown");     // Default to Unknown
            cItem.wxsCompilerPath = _("");      // Default to empty string
            cItem.bDetected = false;            // Default to false

            wxString currentCompilerID = compiler->GetID();

            // Only check if an actual compiler, ignore "NO compiler"
            if (!currentCompilerID.IsSameAs("null"))
            {
                wxString path = compiler->GetMasterPath();
                wxString path_no_macros = compiler->GetMasterPath();
                Manager::Get()->GetMacrosManager()->ReplaceMacros(path_no_macros);

                if (    (path.IsEmpty() || !wxFileName::DirExists(path)) &&
                        Manager::Get()->GetConfigManager(wxT("compiler"))->Exists(wxT("/sets/") + compiler->GetID() + wxT("/name")) &&
                        defaultCompilerID.IsSameAs(currentCompilerID)
                    )
                {
                    Manager::Get()->GetLogManager()->LogError(wxString::Format(_("CompilerName : '%s'   , path : '%s' , path_no_macros : '%s'"), cItem.wxsCompilerName, path, path_no_macros ));
                    Manager::Get()->GetLogManager()->LogError(wxString::Format(_("Manager::Get()->GetConfigManager(wxT(\"compiler\"))->Exists('%s') : %s"), wxT("/sets/") + compiler->GetID() + wxT("/name") ,
                                              Manager::Get()->GetConfigManager(wxT("compiler"))->Exists(wxT("/sets/") + compiler->GetID() + wxT("/name"))?_("True"):_("False") ));

                    // Here, some user-interaction is required not to show this
                    // dialog again on each new start-up of C::B.
                    cItem.wxsStatus = _("Invalid");
                    cItem.bDetected = false;
                    // So we better clearly HIGHLIGHT this entry:
                    cItem.eHighlight = CompilerHighlightColor::eHighlightRed;
                }
                else // The compiler is *probably* invalid, but at least a master-path is set
                {
                    cItem.wxsStatus = _("Not found");
                    cItem.bDetected = false;
                    cItem.eHighlight = CompilerHighlightColor::eHighlightGrey;
                }

                // Inspect deeper and probably try to auto-detect invalid compilers:
                if (compiler->GetParentID().IsEmpty()) // built-in compiler
                {
                    // Try auto-detection (which is for built-in compilers only)
                    bool detected = compiler->AutoDetectInstallationDir() == adrDetected;
                    wxString pathDetected( compiler->GetMasterPath() );

                    //Revert the detected path back to the original path!!!!
                    if (!path.IsEmpty())
                        compiler->SetMasterPath(path);

                    // In case auto-detection was successful:
                    if (detected)
                    {
                        // Path is invalid
                        if (!path.IsEmpty() && !wxFileName::DirExists(path))
                        {
                            cItem.wxsStatus = _("Old path invalid, but new path detected");
                            cItem.bDetected = true;
                            cItem.wxsCompilerPath = pathDetected;
                            cItem.eHighlight = CompilerHighlightColor::eHighlightYellow;
                        }
                        else
                        {
                            if (path.IsEmpty() || path == pathDetected || path_no_macros == pathDetected)
                            {
                                cItem.wxsStatus = _("Detected");
                                cItem.bDetected = true;
                            }
                            else
                            {
                                cItem.wxsStatus = _("User-defined detected");
                                cItem.bDetected = false;
                            }
                            cItem.wxsCompilerPath = pathDetected;
                            cItem.eHighlight = CompilerHighlightColor::eHighlightGreen;
                        }
                    }
                    else
                    {
                        // In case auto-detection failed but a path was setup before:
                        if (!path.IsEmpty() )
                        {
                            // Check, if the master path is valid:
                            if ( wxFileName::DirExists(path_no_macros) && !(path == pathDetected || path_no_macros == pathDetected) )
                            {
                                cItem.wxsStatus = _("User-defined not detected");
                                cItem.bDetected = false;
                                cItem.eHighlight = CompilerHighlightColor::eHighlightRed;
                            }

                            // Assume the user did the setup on purpose, so reset the old settings anyways:
                            compiler->SetMasterPath(path);
                        }
                    }
                }
                else // no built-in, but user-defined (i.e. copied) compiler
                {
                    // Check, if the master path is valid:
                    if ( !path.IsEmpty() && wxFileName::DirExists(path_no_macros) )
                    {
                        cItem.wxsStatus = _("User-defined");
                        cItem.bDetected = true;
                        cItem.eHighlight = CompilerHighlightColor::eHighlightGreen;
                    }
                    else
                    {
                        cItem.wxsStatus = _("User-defined masterpath issue");
                        cItem.bDetected = false;
                        cItem.eHighlight = CompilerHighlightColor::eHighlightRed;
                    }
                }
            }
            else
            {
                cItem.wxsStatus = _("No compiler");
                cItem.wxsCompilerPath = _("N/A");
                cItem.bDetected = true;
                cItem.eHighlight = CompilerHighlightColor::eHighlightGreen;
            }

            vCompilerList.push_back(cItem);
        }
        UpdateCompilerDisplayList();
    }

    XRCCTRL(*this, "lblDefCompiler", wxStaticText)->SetLabel(CompilerFactory::GetDefaultCompiler()->GetName());
}

AutoDetectCompilers::~AutoDetectCompilers()
{
    //dtor
}

void AutoDetectCompilers::OnDefaultCompilerClick(cb_unused wxCommandEvent& event)
{
    wxListCtrl* list = XRCCTRL(*this, "lcCompilers", wxListCtrl);
    int idxList = list->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (idxList != -1)
    {
        wxString wsSelection = list->GetItemText(idxList);
        Compiler* compiler = CompilerFactory::GetCompilerByName(wsSelection);
        int idxComiler = CompilerFactory::GetCompilerIndex(compiler);

        CompilerFactory::SetDefaultCompiler(idxComiler);
        XRCCTRL(*this, "lblDefCompiler", wxStaticText)->SetLabel(CompilerFactory::GetDefaultCompiler()->GetName());
    }
}

void AutoDetectCompilers::OnUdateCompilerListUI(cb_unused wxCommandEvent& event)
{
    UpdateCompilerDisplayList();
}

void AutoDetectCompilers::UpdateCompilerDisplayList()
{
    bool bShowCompilerPath= XRCCTRL(*this,"cbShowCompilerPath", wxCheckBox)->GetValue();
    bool bShowAllCompilerOptions = (XRCCTRL(*this, "rbCompilerShowOptions", wxRadioBox)->GetSelection() == 1);

    wxListCtrl* list = XRCCTRL(*this, "lcCompilers", wxListCtrl);
    if (list)
    {
        list->ClearAll();
        list->InsertColumn(ccnNameColumn, _("Compiler"), wxLIST_FORMAT_LEFT, 380);
        list->InsertColumn(ccnStatusColumn, _("Status"), wxLIST_FORMAT_LEFT, 100);

        if (bShowCompilerPath)
            list->InsertColumn(ccnDetectedPathColumn, _("Compiler Path"), wxLIST_FORMAT_LEFT, 200);

        for (auto itCL = std::begin(vCompilerList); itCL != std::end(vCompilerList); ++itCL)
        {
            if (bShowAllCompilerOptions && !itCL->bDetected)
                continue;

            int idx = list->GetItemCount();
            list->InsertItem(idx, itCL->wxsCompilerName);
            list->SetItem(idx, ccnStatusColumn, itCL->wxsStatus);
            if (bShowCompilerPath)
                list->SetItem(idx, ccnDetectedPathColumn, itCL->wxsCompilerPath);

            switch(itCL->eHighlight)
            {
            case CompilerHighlightColor::eHighlightGrey:
                    list->SetItemTextColour(idx, *wxLIGHT_GREY);
                    break;
                case CompilerHighlightColor::eHighlightGreen:
                    list->SetItemBackgroundColour(idx, *wxGREEN);
                    break;
                case CompilerHighlightColor::eHighlightRed:
                    list->SetItemBackgroundColour(idx, *wxRED);
                    break;
                case CompilerHighlightColor::eHighlightYellow:
                    list->SetItemBackgroundColour(idx, *wxYELLOW);
                    break;
                case CompilerHighlightColor::eHighlightNone:
                default:
                    break;
            }
        }

        // Resize columns so one can read the whole stuff:
        list->SetColumnWidth(ccnNameColumn, wxLIST_AUTOSIZE);
        list->SetColumnWidth(ccnStatusColumn, wxLIST_AUTOSIZE);
        if (bShowCompilerPath)
            list->SetColumnWidth(ccnDetectedPathColumn, wxLIST_AUTOSIZE);
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
        itm.m_col = ccnStatusColumn;
        itm.m_mask = wxLIST_MASK_TEXT;
        if (list->GetItem(itm))
            txt = itm.m_text;
    }
    if (txt == wxT("Detected") || txt == wxT("User-defined"))
    {
        wxListItem itm;
        itm.m_itemId = idx;
        itm.m_col = ccnNameColumn;
        itm.m_mask = wxLIST_MASK_TEXT;
        if (list->GetItem(itm))
        {
            Compiler* compiler = CompilerFactory::GetCompilerByName(itm.m_text);
            idx = CompilerFactory::GetCompilerIndex(compiler);
        }
        txt = CompilerFactory::GetCompiler(idx)->GetMasterPath();
    }
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
    XRCCTRL(*this, "btnSetDefaultCompiler", wxButton)->Enable(en);

    wxString sDefaultCompiler = CompilerFactory::GetDefaultCompiler()->GetName();
    for (auto itCL = std::begin(vCompilerList); itCL != std::end(vCompilerList); ++itCL)
    {
        // Find default compiler in the list.
        if (itCL->wxsCompilerName.IsSameAs(sDefaultCompiler))
        {
            // Only enable the ok button if the default compiler has been detected
            XRCCTRL(*this, "wxID_OK", wxButton)->Enable(itCL->bDetected);
            break; // Exit already found default compiler.
        }
    }
    event.Skip();
}
