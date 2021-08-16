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
    EVT_CLOSE(AutoDetectCompilers::OnClose)
END_EVENT_TABLE()

AutoDetectCompilers::AutoDetectCompilers(wxWindow* parent)
{
    //ctor
    wxXmlResource::Get()->LoadObject(this, parent, _T("dlgAutoDetectCompilers"),_T("wxScrollingDialog"));
    XRCCTRL(*this, "wxID_CLOSE", wxButton)->SetDefault();
    Bind(wxEVT_BUTTON, &AutoDetectCompilers::OnCloseClicked, this, wxID_CLOSE);

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

            CompilerItem compilerListItem;
            compilerListItem.compilerName = compiler->GetName();
            compilerListItem.status = "Unknown";          	// Default to Unknown
            compilerListItem.compilerPath = wxString();     // Default to empty string
            compilerListItem.detected = false;            	// Default to false

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
                    Manager::Get()->GetLogManager()->LogError(wxString::Format(_("CompilerName : '%s'   , path : '%s' , path_no_macros : '%s'"), compilerListItem.compilerName, path, path_no_macros ));
                    Manager::Get()->GetLogManager()->LogError(wxString::Format(_("Manager::Get()->GetConfigManager(wxT(\"compiler\"))->Exists('%s') : %s"), wxT("/sets/") + compiler->GetID() + wxT("/name") ,
                                              Manager::Get()->GetConfigManager(wxT("compiler"))->Exists(wxT("/sets/") + compiler->GetID() + wxT("/name"))?"True":"False"));

                    // Here, some user-interaction is required not to show this
                    // dialog again on each new start-up of C::B.
                    compilerListItem.status = "Invalid";
                    compilerListItem.detected = false;
                    // So we better clearly HIGHLIGHT this entry:
                    compilerListItem.colorHighlight = CompilerHighlightColor::colorHighlightRed;
                }
                else // The compiler is *probably* invalid, but at least a master-path is set
                {
                    compilerListItem.status = "Not found";
                    compilerListItem.detected = false;
                    compilerListItem.colorHighlight = CompilerHighlightColor::colorHighlightGrey;
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
                            compilerListItem.status = "Old path invalid, but new path detected";
                            compilerListItem.detected = true;
                            compilerListItem.compilerPath = pathDetected;
                            compilerListItem.colorHighlight = CompilerHighlightColor::colorHighlightYellow;
                        }
                        else
                        {
                            if (path.IsEmpty() || path == pathDetected || path_no_macros == pathDetected)
                            {
                                compilerListItem.status = "Detected";
                                compilerListItem.detected = true;
                            }
                            else
                            {
                                compilerListItem.status = "User-defined detected";
                                compilerListItem.detected = false;
                            }
                            compilerListItem.compilerPath = pathDetected;
                            compilerListItem.colorHighlight = CompilerHighlightColor::colorHighlightGreen;
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
                                compilerListItem.status = "User-defined not detected";
                                compilerListItem.detected = false;
                                compilerListItem.colorHighlight = CompilerHighlightColor::colorHighlightRed;
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
                        compilerListItem.status = "User-defined";
                        compilerListItem.detected = true;
                        compilerListItem.colorHighlight = CompilerHighlightColor::colorHighlightGreen;
                    }
                    else
                    {
                        compilerListItem.status = "User-defined masterpath issue";
                        compilerListItem.detected = false;
                        compilerListItem.colorHighlight = CompilerHighlightColor::colorHighlightRed;
                    }
                }
            }
            else
            {
                compilerListItem.status = "No compiler";
                compilerListItem.compilerPath = "N/A";
                compilerListItem.detected = true;
                compilerListItem.colorHighlight = CompilerHighlightColor::colorHighlightGreen;
            }

            if (defaultCompilerID.IsSameAs(currentCompilerID))
            {
                wxStaticText* controlLblDefCompiler = XRCCTRL(*this, "lblDefCompiler", wxStaticText);
                if (compilerListItem.detected)
                {
                    wxColour  colorTextDefault =  XRCCTRL(*this, "ID_STATICTEXT1", wxStaticText)->GetForegroundColour();
                    wxColour  colorBackgroundDefault =  XRCCTRL(*this, "ID_STATICTEXT1", wxStaticText)->GetBackgroundColour();

                    controlLblDefCompiler->SetLabel(compilerListItem.compilerName);
                    controlLblDefCompiler->SetForegroundColour(colorTextDefault);
                    controlLblDefCompiler->SetBackgroundColour(colorBackgroundDefault);
                }
                else
                {
                    controlLblDefCompiler->SetLabel(wxString::Format(_("INVALID: %s"),compilerListItem.compilerName));
                    controlLblDefCompiler->SetForegroundColour(wxColour(*wxWHITE));
                    controlLblDefCompiler->SetBackgroundColour(wxColour(*wxRED));
                }
            }

            m_CompilerList.push_back(compilerListItem);
        }
        UpdateCompilerDisplayList();
    }
}

AutoDetectCompilers::~AutoDetectCompilers()
{
    //dtor
}

bool AutoDetectCompilers::closeCheckOkay()
{
    wxString defaultCompiler = CompilerFactory::GetDefaultCompiler()->GetName();

    for (std::vector<CompilerItem>::iterator it = m_CompilerList.begin(); it != m_CompilerList.end(); ++it)
    {
        // Find default compiler in the list and if it was not detected double check with the user that this is okay.
        if (it->compilerName.IsSameAs(defaultCompiler) && !it->detected)
        {
            if (wxMessageBox("Are you sure you want to configure CodeBlocks for an invalid compiler?", "",wxYES_NO) == wxNO)
            {
                return false;
            }
        }
    }
    return true;
}

void AutoDetectCompilers::OnClose(wxCloseEvent& event)
{
    if ( event.CanVeto())
    {
        if (!closeCheckOkay())
        {
            event.Veto();
            return;
        }
    }
    event.Skip(); // the default event handler does call Destroy()
}

void AutoDetectCompilers::OnCloseClicked(wxCommandEvent& event)
{
    if (closeCheckOkay())
    {
        EndModal(0); // DO NOT call Destroy(); as it will cause an exception!
    }
    else
    {
        event.Skip();
    }
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

        wxColour  colorDefault =  XRCCTRL(*this, "ID_STATICTEXT1", wxStaticText)->GetBackgroundColour();
        wxStaticText* controlLblDefCompiler = XRCCTRL(*this, "lblDefCompiler", wxStaticText);
        controlLblDefCompiler->SetLabel(CompilerFactory::GetDefaultCompiler()->GetName());
        controlLblDefCompiler->SetBackgroundColour(colorDefault);
    }
}

void AutoDetectCompilers::OnUdateCompilerListUI(cb_unused wxCommandEvent& event)
{
    UpdateCompilerDisplayList();
}

void AutoDetectCompilers::UpdateCompilerDisplayList()
{
    bool bShowAllCompilerOptions = (XRCCTRL(*this, "rbCompilerShowOptions", wxRadioBox)->GetSelection() == 1);

    wxListCtrl* list = XRCCTRL(*this, "lcCompilers", wxListCtrl);
    if (list)
    {
        list->ClearAll();
        list->InsertColumn(ccnNameColumn, "Compiler", wxLIST_FORMAT_LEFT, 380);
        list->InsertColumn(ccnStatusColumn, "Status", wxLIST_FORMAT_LEFT, 100);
        list->InsertColumn(ccnDetectedPathColumn, "Compiler Path", wxLIST_FORMAT_LEFT, 200);

        for (std::vector<CompilerItem>::iterator it = m_CompilerList.begin(); it != m_CompilerList.end(); ++it)
        {
            if (bShowAllCompilerOptions && !it->detected)
                continue;

            int idx = list->GetItemCount();
            list->InsertItem(idx, it->compilerName);
            list->SetItem(idx, ccnStatusColumn, it->status);
            list->SetItem(idx, ccnDetectedPathColumn, it->compilerPath);

            wxColour colourText = wxNullColour;
            wxColour colourBackground = wxNullColour;
            switch(it->colorHighlight)
            {
            case CompilerHighlightColor::colorHighlightGrey:
                    colourText  = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
                    colourText  = wxColour(*wxLIGHT_GREY);
                    break;
                case CompilerHighlightColor::colorHighlightGreen:
                    colourText  = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
                    colourBackground  = wxColour(*wxGREEN);
                    break;
                case CompilerHighlightColor::colorHighlightRed:
                    colourText  = wxColour(*wxWHITE);
                    colourBackground  = wxColour(*wxRED);
                    break;
                case CompilerHighlightColor::colorHighlightYellow:
                    colourText  = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
                    colourBackground  = wxColour(*wxYELLOW);
                    break;
                case CompilerHighlightColor::colorHighlightNone:
                default:
                    // colourText = wxNullColour;
                    // colourBackground = wxNullColour;
                    break;
            }
            list->SetItemTextColour(idx, colourText);
            list->SetItemBackgroundColour(idx, colourBackground);
        }

        // Resize columns so one can read the whole stuff:
        list->SetColumnWidth(ccnNameColumn, wxLIST_AUTOSIZE);
        list->SetColumnWidth(ccnStatusColumn, wxLIST_AUTOSIZE);
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

    event.Skip();
}
