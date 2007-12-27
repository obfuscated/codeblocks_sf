/*
* This file is part of lib_finder plugin for Code::Blocks Studio
* Copyright (C) 2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision: 4504 $
* $Id: wxsmithpluginregistrants.cpp 4504 2007-10-02 21:52:30Z byo $
* $HeadURL: svn+ssh://byo@svn.berlios.de/svnroot/repos/codeblocks/trunk/src/plugins/contrib/wxSmith/plugin/wxsmithpluginregistrants.cpp $
*/

#include "projectconfigurationpanel.h"

//(*InternalHeaders(ProjectConfigurationPanel)
#include <wx/treectrl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/listbox.h>
#include <wx/intl.h>
#include <wx/button.h>
#include <wx/string.h>
//*)

#include <wx/tokenzr.h>

//(*IdInit(ProjectConfigurationPanel)
const long ProjectConfigurationPanel::ID_LISTBOX1 = wxNewId();
const long ProjectConfigurationPanel::ID_BUTTON1 = wxNewId();
const long ProjectConfigurationPanel::ID_BUTTON2 = wxNewId();
const long ProjectConfigurationPanel::ID_TREECTRL1 = wxNewId();
const long ProjectConfigurationPanel::ID_STATICTEXT1 = wxNewId();
const long ProjectConfigurationPanel::ID_TEXTCTRL2 = wxNewId();
const long ProjectConfigurationPanel::ID_CHECKBOX1 = wxNewId();
const long ProjectConfigurationPanel::ID_TEXTCTRL1 = wxNewId();
const long ProjectConfigurationPanel::ID_BUTTON3 = wxNewId();
const long ProjectConfigurationPanel::ID_TIMER1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(ProjectConfigurationPanel,cbConfigurationPanel)
	//(*EventTable(ProjectConfigurationPanel)
	//*)
END_EVENT_TABLE()

namespace
{
    class TreeItemData: public wxTreeItemData
    {
        // Warning: since we use reference to string instead of string itself,
        // the base string must exist as long as this item data exists
        public:
            TreeItemData(const wxString& GlobalVar): m_GlobalVar(GlobalVar) {}
            ~TreeItemData() {}

            const wxString& m_GlobalVar;
    };

    class ListItemData: public wxClientData
    {
        // We don't use string's reference here since project's libraries set
        // should be much smaller than list of known libraries
        public:
            ListItemData(const wxString& GlobalVar): m_GlobalVar(GlobalVar) {}
            ~ListItemData() {}

            const wxString m_GlobalVar;
    };
}


ProjectConfigurationPanel::ProjectConfigurationPanel(wxWindow* parent,ProjectConfiguration* Configuration,ResultMap& KnownLibs,ResultMap& KnownPkgConfigLibs):
    m_Configuration(Configuration),
    m_KnownLibs(KnownLibs),
    m_KnownPkgConfigLibs(KnownPkgConfigLibs)
{
	//(*Initialize(ProjectConfigurationPanel)
	wxBoxSizer* BoxSizer4;
	wxStaticBoxSizer* StaticBoxSizer2;
	wxBoxSizer* BoxSizer2;
	wxStaticBoxSizer* StaticBoxSizer3;
	wxBoxSizer* BoxSizer1;
	wxStaticBoxSizer* StaticBoxSizer1;
	wxBoxSizer* BoxSizer3;
	
	Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Libraries used in project"));
	m_UsedLibraries = new wxListBox(this, ID_LISTBOX1, wxDefaultPosition, wxSize(147,123), 0, 0, 0, wxDefaultValidator, _T("ID_LISTBOX1"));
	StaticBoxSizer1->Add(m_UsedLibraries, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(StaticBoxSizer1, 3, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer2 = new wxBoxSizer(wxVERTICAL);
	m_Add = new wxButton(this, ID_BUTTON1, _("<"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON1"));
	m_Add->Disable();
	m_Add->SetToolTip(_("Add selected library to project"));
	BoxSizer2->Add(m_Add, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_Remove = new wxButton(this, ID_BUTTON2, _(">"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON2"));
	m_Remove->Disable();
	m_Remove->SetToolTip(_("Remove selected library from project"));
	BoxSizer2->Add(m_Remove, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer3 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxVERTICAL, this, _("Known libraries"));
	m_KnownLibrariesTree = new wxTreeCtrl(this, ID_TREECTRL1, wxDefaultPosition, wxDefaultSize, wxTR_HIDE_ROOT|wxTR_SINGLE|wxTR_DEFAULT_STYLE|wxSUNKEN_BORDER, wxDefaultValidator, _T("ID_TREECTRL1"));
	StaticBoxSizer2->Add(m_KnownLibrariesTree, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Filter:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer4->Add(StaticText1, 0, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_Filter = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	BoxSizer4->Add(m_Filter, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2->Add(BoxSizer4, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_Tree = new wxCheckBox(this, ID_CHECKBOX1, _("Show as tree"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	m_Tree->SetValue(true);
	StaticBoxSizer2->Add(m_Tree, 0, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer3->Add(StaticBoxSizer2, 1, wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer3 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Unknown library"));
	m_UnknownLibrary = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	StaticBoxSizer3->Add(m_UnknownLibrary, 1, wxTOP|wxBOTTOM|wxLEFT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_AddUnknown = new wxButton(this, ID_BUTTON3, _("Add"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON3"));
	m_AddUnknown->Disable();
	StaticBoxSizer3->Add(m_AddUnknown, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer3->Add(StaticBoxSizer3, 0, wxTOP|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer3, 4, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	Timer1.SetOwner(this, ID_TIMER1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	
	Connect(ID_LISTBOX1,wxEVT_COMMAND_LISTBOX_SELECTED,(wxObjectEventFunction)&ProjectConfigurationPanel::Onm_UsedLibrariesSelect);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ProjectConfigurationPanel::Onm_AddClick);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ProjectConfigurationPanel::Onm_RemoveClick);
	Connect(ID_TREECTRL1,wxEVT_COMMAND_TREE_SEL_CHANGED,(wxObjectEventFunction)&ProjectConfigurationPanel::Onm_KnownLibrariesTreeSelectionChanged);
	Connect(ID_TEXTCTRL2,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&ProjectConfigurationPanel::Onm_FilterText);
	Connect(ID_TEXTCTRL2,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&ProjectConfigurationPanel::Onm_FilterTextEnter);
	Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&ProjectConfigurationPanel::Onm_TreeClick);
	Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&ProjectConfigurationPanel::Onm_UnknownLibraryText);
	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ProjectConfigurationPanel::Onm_AddUnknownClick);
	Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&ProjectConfigurationPanel::OnTimer1Trigger);
	//*)

	m_ConfCopy = *m_Configuration;
	FillKnownLibraries();
	LoadData();
}

ProjectConfigurationPanel::~ProjectConfigurationPanel()
{
	//(*Destroy(ProjectConfigurationPanel)
	//*)
}

wxString ProjectConfigurationPanel::GetTitle() const
{
    return _T("Libraries");
}

wxString ProjectConfigurationPanel::GetBitmapBaseName() const
{
    return _T("");
}

void ProjectConfigurationPanel::OnApply()
{
    StoreData();
    *m_Configuration = m_ConfCopy;
}

void ProjectConfigurationPanel::OnCancel()
{
}

void ProjectConfigurationPanel::LoadData()
{
    m_UsedLibraries->Freeze();
    for ( size_t i=0; i<m_ConfCopy.m_GlobalUsedLibs.Count(); i++ )
    {
        wxString Name = m_ConfCopy.m_GlobalUsedLibs[i];
        m_UsedLibraries->Append(GetUserListName(Name),new ListItemData(Name));
    }
    m_UsedLibraries->Thaw();
}

wxString ProjectConfigurationPanel::GetUserListName(const wxString& Name)
{
    if ( m_KnownLibs.IsGlobalVar(Name) )
    {
        return Name + _T(": ") + m_KnownLibs.GetGlobalVar(Name)[0]->LibraryName;
    }
    else if ( m_KnownPkgConfigLibs.IsGlobalVar(Name) )
    {
        return Name + _T(" (pkg-config)");
    }
    else
    {
        return Name + _T(" (Unknown library)");
    }
}

void ProjectConfigurationPanel::StoreData()
{
}

void ProjectConfigurationPanel::FillKnownLibraries()
{
    Timer1.Stop();
    m_KnownLibrariesTree->Freeze();
    m_KnownLibrariesTree->DeleteAllItems();
    m_KnownLibrariesTree->AddRoot(_("Known libraries"));

    m_CategoryMap.clear();
    m_IsOtherCategory = false;
    m_IsPkgConfig = false;

    wxString Filter = m_Filter->GetValue().Upper();
    bool Tree = m_Tree->GetValue();

    wxArrayString Vars;
    m_KnownLibs.GetGlobalVarNames(Vars);

    for ( size_t i=0; i<Vars.Count(); i++ )
    {
        ResultArray& Array = m_KnownLibs.GetGlobalVar(Vars[i]);
        if ( Array.IsEmpty() ) continue;

        if ( !Filter.IsEmpty() )
        {
            if ( Array[0]->LibraryName.Upper().Find(Filter) == wxNOT_FOUND &&
                 Array[0]->GlobalVar.Upper().Find(Filter)   == wxNOT_FOUND )
            {
                // This result has been filtered
                continue;
            }
        }

        if ( Tree )
        {
            wxArrayString& Categories = Array[0]->Categories;
            if ( Categories.IsEmpty() )
            {
                BuildEntry(OtherCategoryId(),Array);
            }
            else
            {
                for ( size_t i=0; i<Categories.Count(); i++ )
                {
                    BuildEntry(CategoryId(Categories[i]),Array);
                }
            }
        }
        else
        {
            BuildEntry(m_KnownLibrariesTree->GetRootItem(),Array);
        }
    }

    Vars.Clear();
    m_KnownPkgConfigLibs.GetGlobalVarNames(Vars);

    if ( !Vars.IsEmpty() )
    {
        for ( size_t i=0; i<Vars.Count(); i++ )
        {
            ResultArray& Array = m_KnownPkgConfigLibs.GetGlobalVar(Vars[i]);
            if ( Array.IsEmpty() ) continue;

            if ( !Filter.IsEmpty() )
            {
                if ( Array[0]->LibraryName.Upper().Find(Filter) == wxNOT_FOUND &&
                     Array[0]->GlobalVar.Upper().Find(Filter)   == wxNOT_FOUND )
                {
                    // This result has been filtered
                    continue;
                }
            }

            BuildEntry(Tree ? PkgConfigId() : m_KnownLibrariesTree->GetRootItem(),Array);
        }
    }

    m_KnownLibrariesTree->ExpandAll();
    m_KnownLibrariesTree->Thaw();
}

void ProjectConfigurationPanel::BuildEntry(const wxTreeItemId& Id,ResultArray& Array)
{
    wxString Name = Array[0]->GlobalVar;
    if ( !Array[0]->LibraryName.IsEmpty() )
    {
        Name = Name + _T(": ") + Array[0]->LibraryName;
    }
    m_KnownLibrariesTree->AppendItem(Id,Name,0,0,new TreeItemData(Array[0]->GlobalVar));
}

wxTreeItemId ProjectConfigurationPanel::OtherCategoryId()
{
    if ( m_IsOtherCategory )
    {
        return m_CategoryMap[_("other")];
    }

    m_IsOtherCategory = true;
    return m_CategoryMap[_("other")] = m_KnownLibrariesTree->AppendItem(m_KnownLibrariesTree->GetRootItem(),_("Other"));
}

wxTreeItemId ProjectConfigurationPanel::PkgConfigId()
{
    if ( m_IsPkgConfig ) return m_PkgConfigId;
    m_IsPkgConfig = true;
    return m_PkgConfigId = m_KnownLibrariesTree->AppendItem(
        m_KnownLibrariesTree->GetRootItem(),
        _("Available in pkg-config"));
}

wxTreeItemId ProjectConfigurationPanel::CategoryId(const wxString& Category)
{
    if ( m_CategoryMap.find(Category.Lower()) != m_CategoryMap.end() )
    {
        return m_CategoryMap[Category.Lower()];
    }

    wxStringTokenizer Tokens(Category,_T("."));
    wxString PathSoFar = _T("");
    wxTreeItemId IdSoFar = m_KnownLibrariesTree->GetRootItem();
    bool FirstElem = true;
    while ( Tokens.HasMoreTokens() )
    {
        wxString Part = Tokens.GetNextToken().Lower();
        PathSoFar += Part.Lower();
        if ( m_CategoryMap.find(PathSoFar) == m_CategoryMap.end() )
        {
            // Have to generate the path to the end
            if ( FirstElem && m_IsOtherCategory )
            {
                IdSoFar = m_CategoryMap[PathSoFar] =
                    m_KnownLibrariesTree->InsertItem(
                        IdSoFar,
                        m_KnownLibrariesTree->GetChildrenCount(IdSoFar,false)-1,
                        Part);
                FirstElem = false;
            }
            else
            {
                IdSoFar = m_CategoryMap[PathSoFar] =
                    m_KnownLibrariesTree->AppendItem(IdSoFar,Part);
            }
            while ( Tokens.HasMoreTokens() )
            {
                Part = Tokens.GetNextToken();
                PathSoFar += _T(".");
                PathSoFar = Part.Lower();
                IdSoFar = m_CategoryMap[PathSoFar] =
                    m_KnownLibrariesTree->AppendItem(IdSoFar,Part);
            }
            break;
        }
        FirstElem = false;
        PathSoFar += _T(".");
    }
    // Just for sure if there are multiple dots
    m_CategoryMap[Category.Lower()] = IdSoFar;
    return IdSoFar;
}

void ProjectConfigurationPanel::Onm_TreeClick(wxCommandEvent& event)
{
    FillKnownLibraries();
}

void ProjectConfigurationPanel::OnTimer1Trigger(wxTimerEvent& event)
{
    FillKnownLibraries();
}

void ProjectConfigurationPanel::Onm_FilterText(wxCommandEvent& event)
{
    Timer1.Start(500,true);
}

void ProjectConfigurationPanel::Onm_FilterTextEnter(wxCommandEvent& event)
{
    FillKnownLibraries();
}

void ProjectConfigurationPanel::Onm_KnownLibrariesTreeSelectionChanged(wxTreeEvent& event)
{
    if ( m_KnownLibrariesTree->GetSelection().IsOk() )
    {
        TreeItemData* Data = (TreeItemData*)m_KnownLibrariesTree->GetItemData(m_KnownLibrariesTree->GetSelection());
        if ( Data )
        {
            wxString Library = Data->m_GlobalVar;
            if ( m_ConfCopy.m_GlobalUsedLibs.Index(Library) == wxNOT_FOUND )
            {
                m_Add->Enable();
                return;
            }
        }
    }
    m_Add->Disable();
}

void ProjectConfigurationPanel::Onm_UsedLibrariesSelect(wxCommandEvent& event)
{
    m_Remove->Enable( m_UsedLibraries->GetSelection() != wxNOT_FOUND );
}

void ProjectConfigurationPanel::Onm_RemoveClick(wxCommandEvent& event)
{
    if ( m_UsedLibraries->GetSelection() != wxNOT_FOUND )
    {
        wxString Library =
            ((ListItemData*)m_UsedLibraries->GetClientObject(
                m_UsedLibraries->GetSelection()))->m_GlobalVar;
        m_ConfCopy.m_GlobalUsedLibs.Remove(Library);
        m_UsedLibraries->Delete(m_UsedLibraries->GetSelection());
        m_Remove->Disable();
        wxTreeEvent ev;
        Onm_KnownLibrariesTreeSelectionChanged(ev);
    }
}

void ProjectConfigurationPanel::Onm_AddClick(wxCommandEvent& event)
{
    if ( m_KnownLibrariesTree->GetSelection().IsOk() )
    {
        TreeItemData* Data = (TreeItemData*)m_KnownLibrariesTree->GetItemData(m_KnownLibrariesTree->GetSelection());
        if ( Data )
        {
            wxString Library = Data->m_GlobalVar;
            if ( m_ConfCopy.m_GlobalUsedLibs.Index(Library) == wxNOT_FOUND )
            {
                m_ConfCopy.m_GlobalUsedLibs.Add(Library);
                m_UsedLibraries->Append(GetUserListName(Library),new ListItemData(Library));
                m_Add->Disable();
                return;
            }
        }
    }
}

void ProjectConfigurationPanel::Onm_UnknownLibraryText(wxCommandEvent& event)
{
    m_AddUnknown->Enable(m_UnknownLibrary->GetLastPosition()!=0);
}

void ProjectConfigurationPanel::Onm_AddUnknownClick(wxCommandEvent& event)
{
    wxString Library = m_UnknownLibrary->GetValue();
    if ( !Library.IsEmpty() )
    {
        if ( m_ConfCopy.m_GlobalUsedLibs.Index(Library) == wxNOT_FOUND )
        {
            m_ConfCopy.m_GlobalUsedLibs.Add(Library);
            m_UsedLibraries->Append(GetUserListName(Library),new ListItemData(Library));
            wxTreeEvent ev;
            Onm_KnownLibrariesTreeSelectionChanged(ev);
        }
    }
}
