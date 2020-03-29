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

/*!
 * Includes
 */

////@begin includes
#include "wx/wizard.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class WizardPage;
class WizardPage1;
class WizardPage2;
////@end forward declarations
class EngineDictionaryDownloader;

/*!
 * Control identifiers
 */

////@begin control identifiers
#define DictionaryDownloadWizard 10000
#define DownloadListWizardPage 10002
#define ButtonDownloadList 10011
#define GaugeDownloadList 10003
#define CheckListBoxDictionaries 10004
#define DictionaryListWizardPage 10005
#define TextCtrlSummary 10006
#define ButtonDownload 10007
#define GaugeFileDownload 10008
#define GaugeTotalDownload 10009
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * DictionaryWizard class declaration
 */

class DictionaryWizard: public wxWizard
{    
    DECLARE_CLASS( DictionaryWizard )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    DictionaryWizard( );
    DictionaryWizard( wxWindow* parent, wxWindowID id = DictionaryDownloadWizard, const wxPoint& pos = wxDefaultPosition );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = DictionaryDownloadWizard, const wxPoint& pos = wxDefaultPosition );

    /// Creates the controls and sizers
    void CreateControls();

////@begin DictionaryWizard event handler declarations
////@end DictionaryWizard event handler declarations

////@begin DictionaryWizard member function declarations
    /// Runs the wizard.
    bool Run();

////@end DictionaryWizard member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();
    
    void SetEngineDownloader(EngineDictionaryDownloader* pDownloader) { m_pDownloader = pDownloader; }
    EngineDictionaryDownloader* GetEngineDownloader() { return m_pDownloader; }
    wxArrayString* GetDictionariesToDownload() { return &m_DictionariesToDownload; }
    void PopulateDictionariesToDownload();

////@begin DictionaryWizard member variables
////@end DictionaryWizard member variables

private:
    EngineDictionaryDownloader* m_pDownloader;
    wxArrayString m_DictionariesToDownload;
};

/*!
 * WizardPage1 class declaration
 */

class WizardPage1: public wxWizardPageSimple
{    
    DECLARE_DYNAMIC_CLASS( WizardPage1 )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    WizardPage1( );

    WizardPage1( wxWizard* parent );

    /// Creation
    bool Create( wxWizard* parent );

    /// Creates the controls and sizers
    void CreateControls();

////@begin WizardPage1 event handler declarations
////@end WizardPage1 event handler declarations
    void OnButtonDownloadListClick( wxCommandEvent& event );

////@begin WizardPage1 member function declarations
////@end WizardPage1 member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin WizardPage1 member variables
////@end WizardPage1 member variables
};

/*!
 * WizardPage2 class declaration
 */

class WizardPage2: public wxWizardPageSimple
{    
    DECLARE_DYNAMIC_CLASS( WizardPage2 )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    WizardPage2( );

    WizardPage2( wxWizard* parent );

    /// Creation
    bool Create( wxWizard* parent );

    /// Creates the controls and sizers
    void CreateControls();

////@begin WizardPage2 event handler declarations
////@end WizardPage2 event handler declarations
    void OnPageChanged(wxWizardEvent& event);
    void OnButtonDownloadClick( wxCommandEvent& event );
    
////@begin WizardPage2 member function declarations
////@end WizardPage2 member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();
    wxString GenerateDictionarySummary();
    
////@begin WizardPage2 member variables
////@end WizardPage2 member variables
};

