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
* $Revision: 3296 $
* $Id: ccoptionsdlg.cpp 3296 2006-11-28 19:30:10Z mandrav $
* $HeadURL: svn+ssh://mandrav@svn.berlios.de/svnroot/repos/codeblocks/trunk/src/plugins/codecompletion/ccoptionsdlg.cpp $
*/

#include <sdk.h>
#include "ccoptionsprjdlg.h"
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include <cbproject.h>
#include <editpathdlg.h>
#include <manager.h>
#include <globals.h>

BEGIN_EVENT_TABLE(CCOptionsProjectDlg, wxPanel)
    EVT_UPDATE_UI(-1, CCOptionsProjectDlg::OnUpdateUI)
	EVT_BUTTON(XRCID("btnAdd"), CCOptionsProjectDlg::OnAdd)
	EVT_BUTTON(XRCID("btnEdit"), CCOptionsProjectDlg::OnEdit)
	EVT_BUTTON(XRCID("btnDelete"), CCOptionsProjectDlg::OnDelete)
END_EVENT_TABLE()

CCOptionsProjectDlg::CCOptionsProjectDlg(wxWindow* parent, cbProject* project, NativeParser* np)
	: m_pProject(project),
	m_pNativeParser(np),
	m_pParser(m_pNativeParser->FindParserFromProject(project))
{
	wxXmlResource::Get()->LoadPanel(this, parent, _T("pnlProjectCCOptions"));
    m_OldPaths = m_pNativeParser->GetProjectSearchDirs(m_pProject);

    wxListBox* control = XRCCTRL(*this, "lstPaths", wxListBox);
    control->Clear();
    for (size_t i = 0; i < m_OldPaths.GetCount(); ++i)
    {
        control->Append(m_OldPaths[i]);
    }
}

CCOptionsProjectDlg::~CCOptionsProjectDlg()
{
}

void CCOptionsProjectDlg::OnAdd(wxCommandEvent& event)
{
    wxListBox* control = XRCCTRL(*this, "lstPaths", wxListBox);

    EditPathDlg dlg(this,
            m_pProject ? m_pProject->GetBasePath() : _T(""),
            m_pProject ? m_pProject->GetBasePath() : _T(""),
            _("Add directory"));

    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxString path = dlg.GetPath();
        control->Append(path);
    }
}

void CCOptionsProjectDlg::OnEdit(wxCommandEvent& event)
{
    wxListBox* control = XRCCTRL(*this, "lstPaths", wxListBox);
    int sel = control->GetSelection();
    if (sel < 0)
        return;

    EditPathDlg dlg(this,
            control->GetString(sel),
            m_pProject ? m_pProject->GetBasePath() : _T(""),
            _("Edit directory"));

    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxString path = dlg.GetPath();
        control->SetString(sel, path);
    }
}

void CCOptionsProjectDlg::OnDelete(wxCommandEvent& event)
{
    wxListBox* control = XRCCTRL(*this, "lstPaths", wxListBox);
    int sel = control->GetSelection();
    if (sel < 0)
        return;

    control->Delete(sel);
}

void CCOptionsProjectDlg::OnUpdateUI(wxUpdateUIEvent& event)
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
    for (int i = 0; i < control->GetCount(); ++i)
    {
        newpaths.Add(control->GetString(i));
    }

    if (m_OldPaths != newpaths)
    {
        for (size_t i = 0; i < newpaths.GetCount(); ++i)
        {
            m_pParser->AddIncludeDir(newpaths[i]);
        }

        wxArrayString& pdirs = m_pNativeParser->GetProjectSearchDirs(m_pProject);
        pdirs = newpaths;

        cbMessageBox(_("You have changed the C/C++ parser search paths for this project.\n"
                        "These paths will be taken into account for next parser runs.\n"
                        "If you want them to take effect immediately, you will have to close "
                        "and re-open your project."),
                        _("Information"), wxICON_INFORMATION);
    }
}
