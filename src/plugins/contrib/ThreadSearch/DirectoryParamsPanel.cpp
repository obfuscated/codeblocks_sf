/***************************************************************
 * Name:      DirectoryParamsPanel
 *
 * Purpose:   This class is a panel that allows the user to
 *            set the directory search parameters (dir,
 *            extensions...).
 *            It is used in the ThreadSearchView and the
 *            ThreadSearchConfPanel.
 *            It does nothing but forwarding events to the
 *            parent window.
 *
 * Author:    Jerome ANTOINE
 * Created:   2007-10-08
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#include "sdk.h"
#ifndef CB_PRECOMP
    #include <wx/button.h>
    #include <wx/checkbox.h>
    #include <wx/combobox.h>
    #include <wx/dirdlg.h>
    #include <wx/sizer.h>
    #include <wx/stattext.h>
#endif

#include "DirectoryParamsPanel.h"
#include "ThreadSearchControlIds.h"
#include "ThreadSearchFindData.h"


namespace
{

// Max number of items in search history combo box
const unsigned int MAX_NB_SEARCH_ITEMS = 20;

inline void AddItemToCombo(wxComboBox *combo, const wxString &str)
{
    int index = combo->FindString(str);

    // Removes item if already in combos box
    if (index != wxNOT_FOUND)
        combo->Delete(index);

    // Removes last item if max nb item is reached
    if (combo->GetCount() >= MAX_NB_SEARCH_ITEMS)
        combo->Delete(combo->GetCount()-1);

    // Adds it to combos
    combo->Insert(str, 0);
    combo->SetSelection(0);
}

} // anonymouse namespace

DirectoryParamsPanel::DirectoryParamsPanel(ThreadSearchFindData *findData, wxWindow* parent, int id, const wxPoint& pos,
                                           const wxSize& size, long WXUNUSED(style)):
    wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL),
    m_pFindData(findData)
{
    const wxString choices[] = {};

    // begin wxGlade: DirectoryParamsPanel::DirectoryParamsPanel
    m_pSearchDirPath = new wxComboBox(this, controlIDs.Get(ControlIDs::idSearchDirPath), wxEmptyString,
                                      wxDefaultPosition, wxDefaultSize, 0, choices, wxCB_DROPDOWN|wxTE_PROCESS_ENTER);
    m_pBtnSelectDir = new wxButton(this, controlIDs.Get(ControlIDs::idBtnDirSelectClick), _("..."));
    m_pChkSearchDirRecursively = new wxCheckBox(this, controlIDs.Get(ControlIDs::idChkSearchDirRecurse), _("Recurse"));
    m_pChkSearchDirHiddenFiles = new wxCheckBox(this, controlIDs.Get(ControlIDs::idChkSearchDirHidden), _("Hidden"));
    m_pMask = new wxComboBox(this, controlIDs.Get(ControlIDs::idSearchMask), wxT("*.*"),
                             wxDefaultPosition, wxDefaultSize, 0, choices, wxCB_DROPDOWN|wxTE_PROCESS_ENTER);

    set_properties();
    do_layout();
    // end wxGlade
}


BEGIN_EVENT_TABLE(DirectoryParamsPanel, wxPanel)
    // begin wxGlade: DirectoryParamsPanel::event_table
    EVT_TEXT_ENTER(controlIDs.Get(ControlIDs::idSearchDirPath), DirectoryParamsPanel::OnSearchDirTextEvent)
    EVT_TEXT(controlIDs.Get(ControlIDs::idSearchDirPath), DirectoryParamsPanel::OnSearchDirTextEvent)
    EVT_COMBOBOX(controlIDs.Get(ControlIDs::idSearchDirPath), DirectoryParamsPanel::OnSearchDirTextEvent)
    EVT_BUTTON(controlIDs.Get(ControlIDs::idBtnDirSelectClick), DirectoryParamsPanel::OnBtnDirSelectClick)
    EVT_CHECKBOX(controlIDs.Get(ControlIDs::idChkSearchDirRecurse), DirectoryParamsPanel::OnChkSearchDirRecurse)
    EVT_CHECKBOX(controlIDs.Get(ControlIDs::idChkSearchDirHidden), DirectoryParamsPanel::OnChkSearchDirHidden)
    EVT_TEXT_ENTER(controlIDs.Get(ControlIDs::idSearchMask), DirectoryParamsPanel::OnSearchMaskTextEvent)
    EVT_TEXT(controlIDs.Get(ControlIDs::idSearchMask), DirectoryParamsPanel::OnSearchMaskTextEvent)
    EVT_COMBOBOX(controlIDs.Get(ControlIDs::idSearchMask), DirectoryParamsPanel::OnSearchMaskTextEvent)
    // end wxGlade
END_EVENT_TABLE();


void DirectoryParamsPanel::OnSearchDirTextEvent(wxCommandEvent &event)
{
    m_pFindData->SetSearchPath(event.GetString());
    event.Skip();
}

void DirectoryParamsPanel::OnSearchMaskTextEvent(wxCommandEvent &event)
{
    m_pFindData->SetSearchMask(event.GetString());
    event.Skip();
}


void DirectoryParamsPanel::OnBtnDirSelectClick(wxCommandEvent &event)
{
    wxString dir = m_pSearchDirPath->GetValue();
    if (dir.empty())
        dir = wxGetCwd();
    wxDirDialog DlgDir(this, _("Select directory"), dir);
    if ( DlgDir.ShowModal() == wxID_OK )
    {
        m_pSearchDirPath->SetValue(DlgDir.GetPath());
        m_pFindData->SetSearchPath(DlgDir.GetPath());
    }

    event.Skip();
}


void DirectoryParamsPanel::OnChkSearchDirRecurse(wxCommandEvent &event)
{
    m_pFindData->SetRecursiveSearch(event.IsChecked());
    event.Skip();
}
void DirectoryParamsPanel::OnChkSearchDirHidden(wxCommandEvent &event)
{
    m_pFindData->SetHiddenSearch(event.IsChecked());
    event.Skip();
}


// wxGlade: add DirectoryParamsPanel event handlers


void DirectoryParamsPanel::set_properties()
{
    // begin wxGlade: DirectoryParamsPanel::set_properties
    m_pSearchDirPath->SetToolTip(_("Directory to search in files"));
    m_pBtnSelectDir->SetToolTip(_("Browse for directory to search in"));
    m_pChkSearchDirRecursively->SetToolTip(_("Search in directory files recursively"));
    m_pChkSearchDirRecursively->SetValue(1);
    m_pChkSearchDirHiddenFiles->SetToolTip(_("Search in directory hidden files"));
    m_pChkSearchDirHiddenFiles->SetValue(1);
    m_pMask->SetToolTip(wxT("*.cpp;*.c;*.h"));
    // end wxGlade
}


void DirectoryParamsPanel::do_layout()
{
#if wxCHECK_VERSION(2, 9, 0)
    #define wxADJUST_MINSIZE 0
#endif
    // begin wxGlade: DirectoryParamsPanel::do_layout
    wxBoxSizer* SizerTop = new wxBoxSizer(wxHORIZONTAL);
    SizerTop->Add(m_pSearchDirPath, 2, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SizerTop->Add(m_pBtnSelectDir, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SizerTop->Add(m_pChkSearchDirRecursively, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SizerTop->Add(m_pChkSearchDirHiddenFiles, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SizerTop->Add(m_pMask, 1, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    wxStaticText* m_pStatTxtMask = new wxStaticText(this, -1, _("mask"));
    SizerTop->Add(m_pStatTxtMask, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SetAutoLayout(true);
    SetSizer(SizerTop);
    SizerTop->Fit(this);
    SizerTop->SetSizeHints(this);
    // end wxGlade
}

// Getters
wxString DirectoryParamsPanel::GetSearchDirPath()        const {return m_pSearchDirPath->GetValue();}
bool     DirectoryParamsPanel::GetSearchDirRecursively() const {return m_pChkSearchDirRecursively->IsChecked();}
bool     DirectoryParamsPanel::GetSearchDirHidden()      const {return m_pChkSearchDirHiddenFiles->IsChecked();}
wxString DirectoryParamsPanel::GetSearchMask()           const {return m_pMask->GetValue();}

// Setters
void     DirectoryParamsPanel::SetSearchDirPath(const wxString& sDirPath) {m_pSearchDirPath->SetValue(sDirPath);}
void     DirectoryParamsPanel::SetSearchDirRecursively(bool bRecurse)     {m_pChkSearchDirRecursively->SetValue(bRecurse);}
void     DirectoryParamsPanel::SetSearchDirHidden(bool bSearchHidden)     {m_pChkSearchDirHiddenFiles->SetValue(bSearchHidden);}
void     DirectoryParamsPanel::SetSearchMask(const wxString& sMask)       {m_pMask->SetValue(sMask);}

void DirectoryParamsPanel::SetSearchHistory(const wxArrayString& searchDirs, const wxArrayString& searchMasks)
{
    for (wxArrayString::const_iterator it = searchDirs.begin(); it != searchDirs.end(); ++it)
    {
        if (!it->empty())
            m_pSearchDirPath->Append(*it);
    }
    for (wxArrayString::const_iterator it = searchMasks.begin(); it != searchMasks.end(); ++it)
    {
        if (!it->empty())
            m_pMask->Append(*it);
    }
}

wxArrayString DirectoryParamsPanel::GetSearchDirsHistory() const
{
    return m_pSearchDirPath->GetStrings();
}

wxArrayString DirectoryParamsPanel::GetSearchMasksHistory() const
{
    return m_pMask->GetStrings();
}

void DirectoryParamsPanel::AddExpressionToCombos(const wxString& path, const wxString& mask)
{
    AddItemToCombo(m_pSearchDirPath, path);
    AddItemToCombo(m_pMask, mask);
}
