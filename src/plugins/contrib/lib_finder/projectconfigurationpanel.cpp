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
* $Revision$
* $Id$
* $HeadURL$
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
#include <wx/choicdlg.h>

#include "lib_finder.h"
#include "headersdetectordlg.h"

//(*IdInit(ProjectConfigurationPanel)
const long ProjectConfigurationPanel::ID_LISTBOX1 = wxNewId();
const long ProjectConfigurationPanel::ID_BUTTON6 = wxNewId();
const long ProjectConfigurationPanel::ID_CHECKBOX2 = wxNewId();
const long ProjectConfigurationPanel::ID_BUTTON4 = wxNewId();
const long ProjectConfigurationPanel::ID_BUTTON1 = wxNewId();
const long ProjectConfigurationPanel::ID_BUTTON2 = wxNewId();
const long ProjectConfigurationPanel::ID_TREECTRL1 = wxNewId();
const long ProjectConfigurationPanel::ID_STATICTEXT1 = wxNewId();
const long ProjectConfigurationPanel::ID_TEXTCTRL2 = wxNewId();
const long ProjectConfigurationPanel::ID_CHECKBOX1 = wxNewId();
const long ProjectConfigurationPanel::ID_BUTTON5 = wxNewId();
const long ProjectConfigurationPanel::ID_TEXTCTRL1 = wxNewId();
const long ProjectConfigurationPanel::ID_BUTTON3 = wxNewId();
const long ProjectConfigurationPanel::ID_STATICTEXT2 = wxNewId();
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
            TreeItemData(const wxString& ShortCode): m_ShortCode(ShortCode) {}
            ~TreeItemData() {}

            const wxString& m_ShortCode;
    };

    class ListItemData: public wxClientData
    {
        // We don't use string's reference here since project's libraries set
        // should be much smaller than list of known libraries
        public:
            ListItemData(const wxString& ShortCode): m_ShortCode(ShortCode) {}
            ~ListItemData() {}

            const wxString m_ShortCode;
    };
}

ProjectConfigurationPanel::ProjectConfigurationPanel(wxWindow* parent,ProjectConfiguration* Configuration,cbProject* Project,TypedResults& KnownLibs):
    m_Configuration(Configuration),
    m_Project(Project),
    m_KnownLibs(KnownLibs)
{
	//(*Initialize(ProjectConfigurationPanel)
	wxBoxSizer* BoxSizer4;
	wxStaticBoxSizer* StaticBoxSizer2;
	wxBoxSizer* BoxSizer6;
	wxBoxSizer* BoxSizer5;
	wxBoxSizer* BoxSizer7;
	wxBoxSizer* BoxSizer2;
	wxStaticBoxSizer* StaticBoxSizer3;
	wxBoxSizer* BoxSizer1;
	wxStaticBoxSizer* StaticBoxSizer1;
	wxBoxSizer* BoxSizer3;
	wxStaticBoxSizer* m_DisableAuto;

	Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	BoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
	BoxSizer5 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Libraries used in project"));
	m_UsedLibraries = new wxListBox(this, ID_LISTBOX1, wxDefaultPosition, wxSize(147,123), 0, 0, 0, wxDefaultValidator, _T("ID_LISTBOX1"));
	StaticBoxSizer1->Add(m_UsedLibraries, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button2 = new wxButton(this, ID_BUTTON6, _("Try to detect missing ones"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON6"));
	StaticBoxSizer1->Add(Button2, 0, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer5->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_DisableAuto = new wxStaticBoxSizer(wxVERTICAL, this, _("Extra settings"));
	m_NoAuto = new wxCheckBox(this, ID_CHECKBOX2, _("Don\'t setup automatically"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
	m_NoAuto->SetValue(false);
	m_NoAuto->SetToolTip(_("If you check this option, lib_finder won\'t add settings of libraries automatically.\nNote that automatic setup is available only after applying extra patch to code::blocks available at forums."));
	m_DisableAuto->Add(m_NoAuto, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_AddScript = new wxButton(this, ID_BUTTON4, _("Add manual build script"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON4"));
	m_AddScript->SetToolTip(_("When you use this option, extra script will be added to the project.\nThis script does invoke lib_finder and will add settings of libraries\njust as in case of automatic setup.\nIt may be usefull when no extra-event patch has been applied."));
	m_DisableAuto->Add(m_AddScript, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer5->Add(m_DisableAuto, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer6->Add(BoxSizer5, 3, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer2 = new wxBoxSizer(wxVERTICAL);
	m_Add = new wxButton(this, ID_BUTTON1, _("<"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON1"));
	m_Add->Disable();
	m_Add->SetToolTip(_("Add selected library to project"));
	BoxSizer2->Add(m_Add, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_Remove = new wxButton(this, ID_BUTTON2, _(">"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON2"));
	m_Remove->Disable();
	m_Remove->SetToolTip(_("Remove selected library from project"));
	BoxSizer2->Add(m_Remove, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer6->Add(BoxSizer2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
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
	BoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
	m_Tree = new wxCheckBox(this, ID_CHECKBOX1, _("Show as tree"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	m_Tree->SetValue(true);
	BoxSizer7->Add(m_Tree, 1, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button1 = new wxButton(this, ID_BUTTON5, _("Edit"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON5"));
	Button1->Hide();
	BoxSizer7->Add(Button1, 0, wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2->Add(BoxSizer7, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer3->Add(StaticBoxSizer2, 1, wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer3 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Unknown library"));
	m_UnknownLibrary = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	StaticBoxSizer3->Add(m_UnknownLibrary, 1, wxTOP|wxBOTTOM|wxLEFT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_AddUnknown = new wxButton(this, ID_BUTTON3, _("Add"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON3"));
	m_AddUnknown->Disable();
	StaticBoxSizer3->Add(m_AddUnknown, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer3->Add(StaticBoxSizer3, 0, wxTOP|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer6->Add(BoxSizer3, 4, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer6, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_EventText = new wxStaticText(this, ID_STATICTEXT2, _("Note: Because there\'s not yet ability to update project\'s build options\nfrom plugin, the only way to automatically add library configurations\nis to use build script"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE, _T("ID_STATICTEXT2"));
	m_EventText->Hide();
	BoxSizer1->Add(m_EventText, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	Timer1.SetOwner(this, ID_TIMER1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);

	Connect(ID_LISTBOX1,wxEVT_COMMAND_LISTBOX_SELECTED,(wxObjectEventFunction)&ProjectConfigurationPanel::Onm_UsedLibrariesSelect);
	Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ProjectConfigurationPanel::OnButton2Click);
	Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ProjectConfigurationPanel::Onm_AddScriptClick);
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

	m_EventText->Show(!lib_finder::IsExtraEvent());

	if ( m_Project->GetBuildScripts().Index(_T("lib_finder.script")) != wxNOT_FOUND )
	    m_AddScript->Disable(); // There's a build script yet

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
    m_NoAuto->SetValue(m_ConfCopy.m_DisableAuto);
}

wxString ProjectConfigurationPanel::GetUserListName(const wxString& Name)
{
    for ( int i=0; i<rtCount; i++ )
    {
        if ( m_KnownLibs[i].IsShortCode(Name) )
        {
            switch ( i )
            {
                case rtPkgConfig:
                    return Name + _T(" (pkg-config)");

                default:
                    return Name + _T(": ") + m_KnownLibs[i].GetShortCode(Name)[0]->LibraryName;
            }
        }
    }

    return Name + _T(" (Unknown library)");
}

void ProjectConfigurationPanel::StoreData()
{
    m_ConfCopy.m_DisableAuto = m_NoAuto->GetValue();
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

    // Collecting list of all global variable names,
    // currently without pkg-config entries (will be used later)
    wxArrayString Vars;
    for ( int i=0; i<rtCount; i++ )
    {
        if ( i==rtPkgConfig ) continue;
        m_KnownLibs[i].GetShortCodes(Vars);
    }

    for ( size_t i=0; i<Vars.Count(); i++ )
    {
        ResultArray* Array = 0;
        for ( int j=0; j<rtCount; j++ )
        {
            if ( j!=rtPkgConfig )
            {
                if ( m_KnownLibs[j].IsShortCode(Vars[i]) )
                    Array = &m_KnownLibs[j].GetShortCode(Vars[i]);
            }
        }

        if ( !Array || Array->IsEmpty() ) continue;

        if ( !Filter.IsEmpty() )
        {
            if ( (*Array)[0]->LibraryName.Upper().Find(Filter) == wxNOT_FOUND &&
                 (*Array)[0]->ShortCode.Upper().Find(Filter)   == wxNOT_FOUND )
            {
                // This result has been filtered
                continue;
            }
        }

        if ( Tree )
        {
            wxArrayString& Categories = (*Array)[0]->Categories;
            if ( Categories.IsEmpty() )
                BuildEntry(OtherCategoryId(),*Array);
            else
            {
                for ( size_t i=0; i<Categories.Count(); i++ )
                    BuildEntry(CategoryId(Categories[i]),*Array);
            }
        }
        else
            BuildEntry(m_KnownLibrariesTree->GetRootItem(),*Array);
    }

    // Now add pkg-config entries in separate category at the end
    Vars.Clear();
    m_KnownLibs[rtPkgConfig].GetShortCodes(Vars);

    if ( !Vars.IsEmpty() )
    {
        for ( size_t i=0; i<Vars.Count(); i++ )
        {
            ResultArray& Array = m_KnownLibs[rtPkgConfig].GetShortCode(Vars[i]);
            if ( Array.IsEmpty() ) continue;

            if ( !Filter.IsEmpty() )
            {
                if ( Array[0]->LibraryName.Upper().Find(Filter) == wxNOT_FOUND &&
                     Array[0]->ShortCode.Upper().Find(Filter)   == wxNOT_FOUND )
                {
                    // This result has been filtered
                    continue;
                }
            }

            BuildEntry(Tree ? PkgConfigId() : m_KnownLibrariesTree->GetRootItem(),Array);
        }
    }

    // Ok, let's present all results
//    m_KnownLibrariesTree->ExpandAll();
    m_KnownLibrariesTree->Thaw();
}

void ProjectConfigurationPanel::BuildEntry(const wxTreeItemId& Id,ResultArray& Array)
{
    wxString Name = Array[0]->ShortCode;
    if ( !Array[0]->LibraryName.IsEmpty() )
        Name = Name + _T(": ") + Array[0]->LibraryName;
    m_KnownLibrariesTree->AppendItem(Id,Name,0,0,new TreeItemData(Array[0]->ShortCode));
}

wxTreeItemId ProjectConfigurationPanel::OtherCategoryId()
{
    if ( m_IsOtherCategory )
        return m_CategoryMap[_T(".other")];

    m_IsOtherCategory = true;
    return m_CategoryMap[_T(".other")] = m_KnownLibrariesTree->AppendItem(m_KnownLibrariesTree->GetRootItem(),_("Other"));
}

wxTreeItemId ProjectConfigurationPanel::PkgConfigId()
{
    if ( m_IsPkgConfig )
        return m_CategoryMap[_T(".pkg-config")];
    m_IsPkgConfig = true;
    return m_CategoryMap[_T(".pkg-config")] = m_KnownLibrariesTree->AppendItem(m_KnownLibrariesTree->GetRootItem(),_("Available in pkg-config"));
}

wxTreeItemId ProjectConfigurationPanel::CategoryId(const wxString& Category)
{
    if ( m_CategoryMap.find(Category.Lower()) != m_CategoryMap.end() )
        return m_CategoryMap[Category.Lower()];

    wxStringTokenizer Tokens(Category,_T("."),wxTOKEN_STRTOK);
    wxString PathSoFar = _T("");
    wxTreeItemId IdSoFar = m_KnownLibrariesTree->GetRootItem();
    bool FirstElem = true;
    while ( Tokens.HasMoreTokens() )
    {
        // Iterate through items already added to map
        wxString Part = Tokens.GetNextToken();
        PathSoFar += Part.Lower();
        if ( m_CategoryMap.find(PathSoFar) == m_CategoryMap.end() )
        {
            // Ok, found first node which is not yet added, this mean
            // that all subnodes are also not yet added
            int SkipLast = FirstElem ? (m_IsOtherCategory?1:0) + (m_IsPkgConfig?1:0) : 0;

            // First elem of the path must take into consideration
            // that some categoies must remain at the end
            if ( SkipLast )
            {
                IdSoFar = m_CategoryMap[PathSoFar] =
                    m_KnownLibrariesTree->InsertItem(
                        IdSoFar,
                        m_KnownLibrariesTree->GetChildrenCount(IdSoFar,false)-SkipLast,
                        Part);
                FirstElem = false;
            }
            else
            {
                IdSoFar = m_CategoryMap[PathSoFar] =
                    m_KnownLibrariesTree->AppendItem(IdSoFar,Part);
            }

            // Next items are always added at the end
            while ( Tokens.HasMoreTokens() )
            {
                Part = Tokens.GetNextToken();
                PathSoFar += _T(".");
                PathSoFar = Part.Lower();
                IdSoFar = m_CategoryMap[PathSoFar] =
                    m_KnownLibrariesTree->AppendItem(IdSoFar,Part);
            }

            // If we're here, all remaining path has been added, so we
            // finished here
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
            wxString Library = Data->m_ShortCode;
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
                m_UsedLibraries->GetSelection()))->m_ShortCode;
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
            wxString Library = Data->m_ShortCode;
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

void ProjectConfigurationPanel::Onm_AddScriptClick(wxCommandEvent& event)
{
    wxFile Fl(m_Project->GetBasePath() + wxFileName::GetPathSeparator() + _T("lib_finder.script"),wxFile::write);
    if ( !Fl.IsOpened() )
    {
        wxMessageBox(_("Couldn't create file \"lib_finder.script\" in project's base path"),_("lib_finder.script error"),wxOK|wxICON_ERROR,this);
        return;
    }

    if ( !Fl.Write(
        _T("function SetBuildOptions(base)\n")
        _T("{\n")
        _T("\tif ( \"LibFinder\" in getroottable() )\n")
        _T("\t{\n")
        _T("\t\tLibFinder.SetupTarget(base);\n")
        _T("\t}\n")
        _T("}\n"),
        wxConvUTF8) )
    {
        wxMessageBox(_("Couldn't write script file \"lib_finder.script\"."),_("lib_finder.script error"),wxOK|wxICON_ERROR,this);
        return;
    }
    Fl.Close();

    m_Project->AddBuildScript(_T("lib_finder.script"));
    m_AddScript->Disable();
    m_NoAuto->SetValue(true);

    wxMessageBox(_("Script \"lib_finder.script\" successfully added."),_("lib_finder.script Success"),wxOK|wxICON_INFORMATION,this);
}

void ProjectConfigurationPanel::OnButton2Click(wxCommandEvent& event)
{
    wxArrayString HeadersBase;
    if ( HeadersDetectorDlg(this,m_Project,HeadersBase).ShowModal() != wxID_OK )
    {
        cbMessageBox( _("Cancelled the search"), _("Cancelled"), wxOK | wxICON_WARNING, this );
        return;
    }

    if ( HeadersBase.IsEmpty() )
    {
        cbMessageBox( _("Didn't found any #include directive."), _("Error"), wxOK | wxICON_ERROR, this );
        return;
    }

    // Getting array of all known libraries
    ResultArray AllArray;
    for ( int i=0; i<rtCount; i++ )
        m_KnownLibs[i].GetAllResults(AllArray);
    wxArrayString NewLibs;

    // Sorting and removing duplicates and processing results
    HeadersBase.Sort();
    wxString Previous;
    for ( size_t i=0; i<HeadersBase.Count(); i++ )
    {
        if ( Previous != HeadersBase[i] )
        {
            Previous = HeadersBase[i];
            DetectNewLibs( Previous, AllArray, NewLibs );
        }
    }

    // Filtering detected results
    wxArrayString NewLibsFiltered;
    NewLibs.Sort();
    Previous.Clear();
    for ( size_t i=0; i<NewLibs.Count(); i++ )
    {
        if ( Previous != NewLibs[i] )
        {
            Previous = NewLibs[i];
            if ( m_ConfCopy.m_GlobalUsedLibs.Index(Previous) == wxNOT_FOUND )
                NewLibsFiltered.Add( Previous );
        }
    }

    if ( NewLibsFiltered.IsEmpty() )
    {
        cbMessageBox(
            _("Didn't found any missing library for your project.\n"
              "\n"
              "This may mean that you project is fully configured\n"
              "or that missing libraries are not yet recognized\n"
              "or fully supported in lib_finder plugin"),
            _("No libraries found"),
            wxOK | wxICON_ASTERISK,
            this );
        return;
    }

    wxArrayInt Choices;
    wxGetMultipleChoices(
        Choices,
        _("Select libraries to include in your project"),
        _("Adding new libraries"),
        NewLibsFiltered,
        this);

    if ( Choices.IsEmpty() )
        return;

    for ( size_t i=0; i<Choices.Count(); i++ )
    {
        wxString Library = NewLibsFiltered[ Choices[i] ];
        m_ConfCopy.m_GlobalUsedLibs.Add(Library);
        m_UsedLibraries->Append(GetUserListName(Library),new ListItemData(Library));
    }

    // Make sure that after the scan, used won't be able to manually
    // add currently selected "known" library (through '<' button)
    // which has just been added automatically
    wxTreeEvent ev;
    Onm_KnownLibrariesTreeSelectionChanged(ev);
}

void ProjectConfigurationPanel::DetectNewLibs( const wxString& IncludeName, ResultArray& known, wxArrayString& LibsList )
{
    wxString FixedInclude = IncludeName;
    FixedInclude.MakeLower();
    FixedInclude.Replace(_T("\\"),_T("/"),true);
    for ( size_t i=0; i<known.Count(); i++ )
    {
        for ( size_t j=0; j<known[i]->Headers.Count(); j++ )
        {
            if ( FixedInclude.Matches( known[i]->Headers[j].Lower() ) )
            {
                LibsList.Add( known[i]->ShortCode );
                break;
            }
        }
    }
}
