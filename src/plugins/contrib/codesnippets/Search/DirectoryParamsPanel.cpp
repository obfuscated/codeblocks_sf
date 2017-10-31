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
#if defined(CB_PRECOMP)
#include "sdk.h"
#endif
#ifndef CB_PRECOMP
    #include <wx/textctrl.h>
    #include <wx/button.h>
    #include <wx/checkbox.h>
    #include <wx/dirdlg.h>
    #include <wx/sizer.h>
    #include <wx/stattext.h>
#endif

#include "DirectoryParamsPanel.h"
#include "ThreadSearchControlIds.h"


DirectoryParamsPanel::DirectoryParamsPanel(wxWindow* parent, int id, const wxPoint& pos, const wxSize& size, long WXUNUSED(style)):
    wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL)
{
    // begin wxGlade: DirectoryParamsPanel::DirectoryParamsPanel
    m_pTxtSearchDirPath = new wxTextCtrl(this, idTxtSearchDirPath, wxEmptyString);
    m_pBtnSelectDir = new wxButton(this, idBtnDirSelectClick, _("..."));
    m_pChkSearchDirRecursively = new wxCheckBox(this, idChkSearchDirRecurse, _("Recurse"));
    m_pChkSearchDirHiddenFiles = new wxCheckBox(this, idChkSearchDirHidden, _("Hidden"));
    m_pTxtMask = new wxTextCtrl(this, idTxtSearchMask, wxT("*.*"));

    set_properties();
    do_layout();
    // end wxGlade
}


BEGIN_EVENT_TABLE(DirectoryParamsPanel, wxPanel)
    // begin wxGlade: DirectoryParamsPanel::event_table
    EVT_TEXT(idTxtSearchDirPath, DirectoryParamsPanel::OnTxtTextEvent)
    EVT_BUTTON(idBtnDirSelectClick, DirectoryParamsPanel::OnBtnDirSelectClick)
    EVT_CHECKBOX(idChkSearchDirRecurse, DirectoryParamsPanel::OnChkClickEvent)
    EVT_CHECKBOX(idChkSearchDirHidden, DirectoryParamsPanel::OnChkClickEvent)
    EVT_TEXT(idTxtSearchMask, DirectoryParamsPanel::OnTxtTextEvent)
    // end wxGlade
END_EVENT_TABLE();


void DirectoryParamsPanel::OnTxtTextEvent(wxCommandEvent &event)
{
    event.Skip();
}


void DirectoryParamsPanel::OnBtnDirSelectClick(wxCommandEvent &event)
{
    wxDirDialog DlgDir(this, _("Select directory"), wxGetCwd());
    if ( DlgDir.ShowModal() == wxID_OK )
    {
        m_pTxtSearchDirPath->SetValue(DlgDir.GetPath());
    }

    event.Skip();
}


void DirectoryParamsPanel::OnChkClickEvent(wxCommandEvent &event)
{
    event.Skip();
}


// wxGlade: add DirectoryParamsPanel event handlers


void DirectoryParamsPanel::set_properties()
{
    // begin wxGlade: DirectoryParamsPanel::set_properties
    m_pTxtSearchDirPath->SetToolTip(_("Directory to search in files"));
    m_pBtnSelectDir->SetToolTip(_("Browse for directory to search in"));
    m_pChkSearchDirRecursively->SetToolTip(_("Search in directory files recursively"));
    m_pChkSearchDirRecursively->SetValue(1);
    m_pChkSearchDirHiddenFiles->SetToolTip(_("Search in directory hidden files"));
    m_pChkSearchDirHiddenFiles->SetValue(1);
    m_pTxtMask->SetToolTip(wxT("*.cpp;*.c;*.h"));
    // end wxGlade
}


void DirectoryParamsPanel::do_layout()
{
#if wxCHECK_VERSION(3, 0, 0)
    #define wxADJUST_MINSIZE 0
#endif
    // begin wxGlade: DirectoryParamsPanel::do_layout
    wxBoxSizer* SizerTop = new wxBoxSizer(wxHORIZONTAL);
    SizerTop->Add(m_pTxtSearchDirPath, 2, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SizerTop->Add(m_pBtnSelectDir, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SizerTop->Add(m_pChkSearchDirRecursively, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SizerTop->Add(m_pChkSearchDirHiddenFiles, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SizerTop->Add(m_pTxtMask, 1, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    wxStaticText* m_pStatTxtMask = new wxStaticText(this, -1, _("mask"));
    SizerTop->Add(m_pStatTxtMask, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL|wxADJUST_MINSIZE, 4);
    SetAutoLayout(true);
    SetSizer(SizerTop);
    SizerTop->Fit(this);
    SizerTop->SetSizeHints(this);
    // end wxGlade
}

// Getters
wxString DirectoryParamsPanel::GetSearchDirPath()        const {return m_pTxtSearchDirPath->GetValue();}
bool     DirectoryParamsPanel::GetSearchDirRecursively() const {return m_pChkSearchDirRecursively->IsChecked();}
bool     DirectoryParamsPanel::GetSearchDirHidden()      const {return m_pChkSearchDirHiddenFiles->IsChecked();}
wxString DirectoryParamsPanel::GetSearchMask()           const {return m_pTxtMask->GetValue();}

// Setters
void     DirectoryParamsPanel::SetSearchDirPath(const wxString& sDirPath) {m_pTxtSearchDirPath->SetValue(sDirPath);}
void     DirectoryParamsPanel::SetSearchDirRecursively(bool bRecurse)     {m_pChkSearchDirRecursively->SetValue(bRecurse);}
void     DirectoryParamsPanel::SetSearchDirHidden(bool bSearchHidden)     {m_pChkSearchDirHiddenFiles->SetValue(bSearchHidden);}
void     DirectoryParamsPanel::SetSearchMask(const wxString& sMask)       {m_pTxtMask->SetValue(sMask);}
