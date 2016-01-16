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
    #include <wx/button.h>
    #include <wx/intl.h>
    #include <wx/listbox.h>
    #include <wx/xrc/xmlres.h>

    #include <cbproject.h>
    #include <cbstyledtextctrl.h>
    #include <globals.h>
    #include <logmanager.h>
    #include <manager.h>
#endif

#include <editpathdlg.h>

#include "ccoptionsprjdlg.h"

BEGIN_EVENT_TABLE(CCOptionsProjectDlg, wxPanel)
    EVT_UPDATE_UI(-1, CCOptionsProjectDlg::OnUpdateUI)
    EVT_BUTTON(XRCID("btnAdd"), CCOptionsProjectDlg::OnAdd)
    EVT_BUTTON(XRCID("btnEdit"), CCOptionsProjectDlg::OnEdit)
    EVT_BUTTON(XRCID("btnDelete"), CCOptionsProjectDlg::OnDelete)
END_EVENT_TABLE()

CCOptionsProjectDlg::CCOptionsProjectDlg(wxWindow* parent, cbProject* project, NativeParser* np) :
    m_Project(project),
    m_NativeParser(np),
    m_Parser(&np->GetParser())
{
    wxXmlResource::Get()->LoadPanel(this, parent, _T("pnlProjectCCOptions"));
    m_OldPaths = m_NativeParser->GetProjectSearchDirs(m_Project);

    wxListBox* control = XRCCTRL(*this, "lstPaths", wxListBox);
    control->Clear();
    for (size_t i = 0; i < m_OldPaths.GetCount(); ++i)
        control->Append(m_OldPaths[i]);
}

CCOptionsProjectDlg::~CCOptionsProjectDlg()
{
}

void CCOptionsProjectDlg::OnAdd(cb_unused wxCommandEvent& event)
{
    wxListBox* control = XRCCTRL(*this, "lstPaths", wxListBox);

    EditPathDlg dlg(this,
                    m_Project ? m_Project->GetBasePath() : _T(""),
                    m_Project ? m_Project->GetBasePath() : _T(""),
                    _("Add directory"));

    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxString path = dlg.GetPath();
        control->Append(path);
    }
}

void CCOptionsProjectDlg::OnEdit(cb_unused wxCommandEvent& event)
{
    wxListBox* control = XRCCTRL(*this, "lstPaths", wxListBox);
    int sel = control->GetSelection();
    if (sel < 0)
        return;

    EditPathDlg dlg(this,
                    control->GetString(sel),
                    m_Project ? m_Project->GetBasePath() : _T(""),
                    _("Edit directory"));

    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxString path = dlg.GetPath();
        control->SetString(sel, path);
    }
}

void CCOptionsProjectDlg::OnDelete(cb_unused wxCommandEvent& event)
{
    wxListBox* control = XRCCTRL(*this, "lstPaths", wxListBox);
    int sel = control->GetSelection();
    if (sel < 0)
        return;

    control->Delete(sel);
}

void CCOptionsProjectDlg::OnUpdateUI(cb_unused wxUpdateUIEvent& event)
{
    wxListBox* control = XRCCTRL(*this, "lstPaths", wxListBox);
    bool en = control->GetSelection() >= 0;

    XRCCTRL(*this, "btnEdit", wxButton)->Enable(en);
    XRCCTRL(*this, "btnDelete", wxButton)->Enable(en);
}

void CCOptionsProjectDlg::OnApply()
{
    wxArrayString newpaths;
    wxListBox* control = XRCCTRL(*this, "lstPaths", wxListBox);
    for (int i = 0; i < (int)control->GetCount(); ++i)
        newpaths.Add(control->GetString(i));

    if (m_OldPaths != newpaths)
    {
        for (size_t i = 0; i < newpaths.GetCount(); ++i)
        {
            if (m_Parser)
                m_Parser->AddIncludeDir(newpaths[i]);
        }

        wxArrayString& pdirs = m_NativeParser->GetProjectSearchDirs(m_Project);
        pdirs = newpaths;

        cbMessageBox(_("You have changed the C/C++ parser search paths for this project.\n"
                       "These paths will be taken into account for next parser runs.\n"
                       "If you want them to take effect immediately, you will have to close "
                       "and re-open your project."),
                       _("Information"), wxICON_INFORMATION);
    }
}
