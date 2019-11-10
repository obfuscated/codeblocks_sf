/////////////////////////////////////////////////////////////////////////////
// Name:        
// Purpose:     
// Author:      
// Modified by: 
// Created:     Mon 10 May 2004 10:44:57 PM EDT
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/protocol/ftp.h>
#include <wx/filename.h>
#include <wx/wfstream.h>

////@begin includes
////@end includes
#include "DictionaryWizard.h"
#include "EngineDictionaryDownloader.h"

////@begin XPM images

#include "wiztest.xpm"
////@end XPM images

/*!
 * DictionaryWizard type definition
 */

IMPLEMENT_CLASS( DictionaryWizard, wxWizard )

/*!
 * DictionaryWizard event table definition
 */

BEGIN_EVENT_TABLE( DictionaryWizard, wxWizard )

////@begin DictionaryWizard event table entries
////@end DictionaryWizard event table entries

END_EVENT_TABLE()

/*!
 * DictionaryWizard constructors
 */

DictionaryWizard::DictionaryWizard( )
{
  m_pDownloader = NULL;
}

DictionaryWizard::DictionaryWizard( wxWindow* parent, wxWindowID id, const wxPoint& pos )
{
    m_pDownloader = NULL;
    Create(parent, id, pos);
}

/*!
 * DictionaryWizard creator
 */

bool DictionaryWizard::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos )
{
////@begin DictionaryWizard member initialisation
////@end DictionaryWizard member initialisation

////@begin DictionaryWizard creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS|wxWIZARD_EX_HELPBUTTON);
    wxBitmap wizardBitmap(wiztest_xpm);
    wxWizard::Create( parent, id, _("Download Dictionary"), wizardBitmap, pos );

    CreateControls();
////@end DictionaryWizard creation
    return TRUE;
}

/*!
 * Control creation for DictionaryWizard
 */

void DictionaryWizard::CreateControls()
{    
////@begin DictionaryWizard content construction

    wxWizard* item1 = this;

    WizardPage1* item5 = new WizardPage1( item1 );
    item1->FitToPage(item5);
    WizardPage2* item15 = new WizardPage2( item1 );
    item1->FitToPage(item15);
    wxWizardPageSimple::Chain(item5, item15);
////@end DictionaryWizard content construction
}

/*!
 * Runs the wizard.
 */

bool DictionaryWizard::Run()
{
    if (GetChildren().GetCount() > 0)
    {
        wxWizardPage* startPage = wxDynamicCast(GetChildren().GetFirst()->GetData(), wxWizardPage);
        if (startPage) return RunWizard(startPage);
    }
    return FALSE;
}

/*!
 * Should we show tooltips?
 */

bool DictionaryWizard::ShowToolTips()
{
    return TRUE;
}

void DictionaryWizard::PopulateDictionariesToDownload()
{
    m_DictionariesToDownload.Clear();
    WizardPage1* pWizardPage = wxDynamicCast(GetChildren().GetFirst()->GetData(), WizardPage1);
    if (pWizardPage)
    {
      wxCheckListBox* pCheckListBox = (wxCheckListBox*)(pWizardPage->FindWindow(CheckListBoxDictionaries));
      if (pCheckListBox == NULL)
      {
        ::wxMessageBox("Unable to find available dictionary checklistbox");
      }
      else
      {
        // Iterate through the items in wxCheckListBox and add the checked ones to the list
        int nItemCount = pCheckListBox->GetCount();
        for (int i=0; i<nItemCount; i++)
        {
          if (pCheckListBox->IsChecked(i))
            m_DictionariesToDownload.Add(pCheckListBox->GetString(i));
        }
      }
    }
}

/*!
 * WizardPage1 type definition
 */

IMPLEMENT_DYNAMIC_CLASS( WizardPage1, wxWizardPageSimple )

/*!
 * WizardPage1 event table definition
 */

BEGIN_EVENT_TABLE( WizardPage1, wxWizardPageSimple )

////@begin WizardPage1 event table entries
////@end WizardPage1 event table entries
  EVT_BUTTON(ButtonDownloadList, WizardPage1::OnButtonDownloadListClick)

END_EVENT_TABLE()

/*!
 * WizardPage1 constructors
 */

WizardPage1::WizardPage1( )
{
}

WizardPage1::WizardPage1( wxWizard* parent )
{
    Create( parent );
}

/*!
 * WizardPage1 creator
 */

bool WizardPage1::Create( wxWizard* parent )
{
////@begin WizardPage1 member initialisation
////@end WizardPage1 member initialisation

////@begin WizardPage1 creation
    wxBitmap wizardBitmap;
    wxWizardPageSimple::Create( parent, NULL, NULL, wizardBitmap );

    CreateControls();
    GetSizer()->Fit(this);
////@end WizardPage1 creation
    return TRUE;
}

/*!
 * Control creation for WizardPage1
 */

void WizardPage1::CreateControls()
{    
////@begin WizardPage1 content construction

    WizardPage1* item5 = this;

    wxBoxSizer* item6 = new wxBoxSizer(wxVERTICAL);
    item5->SetSizer(item6);
    item5->SetAutoLayout(TRUE);

    wxStaticText* item4 = new wxStaticText( item5, wxID_STATIC, _("A list of available dictionaries must be downloaded."), wxDefaultPosition, wxDefaultSize, 0 );
    item6->Add(item4, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* item7 = new wxBoxSizer(wxHORIZONTAL);
    item6->Add(item7, 0, wxGROW|wxRIGHT|wxTOP|wxBOTTOM, 5);

    wxButton* item8 = new wxButton( item5, ButtonDownloadList, _("Begin Download"), wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add(item8, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    item7->Add(5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* item10 = new wxStaticText( item5, wxID_STATIC, _("Downloading dictionary list"), wxDefaultPosition, wxDefaultSize, 0 );
    item6->Add(item10, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    wxGauge* item11 = new wxGauge( item5, GaugeDownloadList, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
    item11->SetValue(1);
    item6->Add(item11, 0, wxGROW|wxALL, 5);

    item6->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* item13 = new wxStaticText( item5, wxID_STATIC, _("Available dictionaries"), wxDefaultPosition, wxDefaultSize, 0 );
    item6->Add(item13, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    wxString* item14Strings = NULL;
    wxCheckListBox* item14 = new wxCheckListBox( item5, CheckListBoxDictionaries, wxDefaultPosition, wxDefaultSize, 0, item14Strings, 0 );
    item6->Add(item14, 1, wxGROW|wxALL, 5);

////@end WizardPage1 content construction
}

/*!
 * Should we show tooltips?
 */

bool WizardPage1::ShowToolTips()
{
    return TRUE;
}

void WizardPage1::OnButtonDownloadListClick( wxCommandEvent& event )
{
  // Try to give some feedback that we're doing something
  wxWindowDisabler Disabler;
  wxBusyCursor Busy;
  
  if (GetParent())
  {
    EngineDictionaryDownloader* pDownloader = ((DictionaryWizard*)GetParent())->GetEngineDownloader();
    if (pDownloader)
    {
      wxCheckListBox* pCheckListBox = (wxCheckListBox*)FindWindow(CheckListBoxDictionaries);
      if (pCheckListBox == NULL)
      {
        ::wxMessageBox("Unable to find available dictionary checklistbox");
        return;
      }
      wxArrayString DictionaryArray;
      pDownloader->RetrieveDictionaryList(DictionaryArray);
      for (unsigned int i=0; i<DictionaryArray.GetCount(); i++)
      {
        wxString DictionaryListEntry = DictionaryArray[i];
        if (DictionaryListEntry != "")
          pCheckListBox->Append(DictionaryListEntry);
      }
    }
  }
}

/*!
 * WizardPage2 type definition
 */

IMPLEMENT_DYNAMIC_CLASS( WizardPage2, wxWizardPageSimple )

/*!
 * WizardPage2 event table definition
 */

BEGIN_EVENT_TABLE( WizardPage2, wxWizardPageSimple )

////@begin WizardPage2 event table entries
////@end WizardPage2 event table entries
  EVT_WIZARD_PAGE_CHANGED(-1, WizardPage2::OnPageChanged )
  EVT_BUTTON( ButtonDownload, WizardPage2::OnButtonDownloadClick )
END_EVENT_TABLE()

/*!
 * WizardPage2 constructors
 */

WizardPage2::WizardPage2( )
{
}

WizardPage2::WizardPage2( wxWizard* parent )
{
    Create( parent );
}

/*!
 * WizardPage2 creator
 */

bool WizardPage2::Create( wxWizard* parent )
{
////@begin WizardPage2 member initialisation
////@end WizardPage2 member initialisation

////@begin WizardPage2 creation
    wxBitmap wizardBitmap;
    wxWizardPageSimple::Create( parent, NULL, NULL, wizardBitmap );

    CreateControls();
    GetSizer()->Fit(this);
////@end WizardPage2 creation
    return TRUE;
}

/*!
 * Control creation for WizardPage2
 */

void WizardPage2::CreateControls()
{    
////@begin WizardPage2 content construction

    WizardPage2* item15 = this;

    wxBoxSizer* item16 = new wxBoxSizer(wxVERTICAL);
    item15->SetSizer(item16);
    item15->SetAutoLayout(TRUE);

    wxStaticText* item17 = new wxStaticText( item15, wxID_STATIC, _("The following dictionaries will be downloaded and installed."), wxDefaultPosition, wxDefaultSize, 0 );
    item16->Add(item17, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* item18 = new wxTextCtrl( item15, TextCtrlSummary, _(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
    item16->Add(item18, 1, wxGROW|wxALL, 5);

    item16->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxBoxSizer* item20 = new wxBoxSizer(wxHORIZONTAL);
    item16->Add(item20, 0, wxGROW|wxRIGHT|wxTOP|wxBOTTOM, 5);

    wxButton* item21 = new wxButton( item15, ButtonDownload, _("Begin Download"), wxDefaultPosition, wxDefaultSize, 0 );
    item20->Add(item21, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    item20->Add(5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* item23 = new wxStaticText( item15, wxID_STATIC, _("Current dictionary progress:"), wxDefaultPosition, wxDefaultSize, 0 );
    item16->Add(item23, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    wxGauge* item24 = new wxGauge( item15, GaugeFileDownload, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
    item24->SetValue(1);
    item16->Add(item24, 0, wxGROW|wxALL, 5);

    wxStaticText* item25 = new wxStaticText( item15, wxID_STATIC, _("Overall progress:"), wxDefaultPosition, wxDefaultSize, 0 );
    item16->Add(item25, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    wxGauge* item26 = new wxGauge( item15, GaugeTotalDownload, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
    item26->SetValue(1);
    item16->Add(item26, 0, wxGROW|wxALL, 5);

////@end WizardPage2 content construction
}

/*!
 * Should we show tooltips?
 */

bool WizardPage2::ShowToolTips()
{
    return TRUE;
}

void WizardPage2::OnPageChanged(wxWizardEvent& event)
{
  wxTextCtrl* pSummary = (wxTextCtrl*)FindWindow(TextCtrlSummary);
  ((DictionaryWizard*)GetParent())->PopulateDictionariesToDownload();
  if (pSummary)
    pSummary->SetValue(GenerateDictionarySummary());
}

void WizardPage2::OnButtonDownloadClick( wxCommandEvent& event )
{
  // Try to give some feedback that we're doing something
  wxWindowDisabler Disabler;
  wxBusyCursor Busy;

  // Loop through all the dictionaries selected for download
  // Get a directory list for that dictionaries FTP directory
  // Put a list of the files in that directory into a wxArrayString
  // Ask the EngineDictionaryDownloader class which file to download
  // Download that file and have the EngineDictionaryDownloader class
  //  install that dictionary
  DictionaryWizard* pWizard = (DictionaryWizard*)GetParent();
  if (pWizard)
  {
    wxArrayString* pDictionaryArray = pWizard->GetDictionariesToDownload();
    EngineDictionaryDownloader* pDownloader = pWizard->GetEngineDownloader();
    if (pDictionaryArray)
    {
      for (unsigned int i=0; i<pDictionaryArray->GetCount(); i++)
      {
        wxString strCurrentDictionary = pDictionaryArray->Item(i);
        wxString strDownloadedFileName = pDownloader->DownloadDictionary(strCurrentDictionary);
        if (strDownloadedFileName != wxEmptyString)
        {
          pDownloader->InstallDictionary(strDownloadedFileName);
        }
      }
    }
  }
}

wxString WizardPage2::GenerateDictionarySummary()
{
  wxString strReturn = "";
  DictionaryWizard* pWizard = (DictionaryWizard*)GetParent();
  if (pWizard)
  {
    wxArrayString* pDictionaryArray = pWizard->GetDictionariesToDownload();
    if (pDictionaryArray)
    {
      for (unsigned int i=0; i<pDictionaryArray->GetCount(); i++)
        strReturn += pDictionaryArray->Item(i) + _("\n");
    }
  }
  return strReturn;
}
