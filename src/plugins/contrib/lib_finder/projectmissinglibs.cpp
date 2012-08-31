#include "projectmissinglibs.h"

//(*InternalHeaders(ProjectMissingLibs)
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/string.h>
#include <wx/intl.h>
#include <wx/statline.h>
#include <wx/stattext.h>
//*)
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <manager.h>
#include <configmanager.h>
#include "webresourcesmanager.h"
#include "dirlistdlg.h"
#include "processingdlg.h"

//(*IdInit(ProjectMissingLibs)
const long ProjectMissingLibs::ID_STATICTEXT1 = wxNewId();
const long ProjectMissingLibs::ID_STATICLINE2 = wxNewId();
const long ProjectMissingLibs::ID_STATICTEXT2 = wxNewId();
const long ProjectMissingLibs::ID_STATICLINE3 = wxNewId();
const long ProjectMissingLibs::ID_STATICTEXT3 = wxNewId();
const long ProjectMissingLibs::ID_STATICLINE10 = wxNewId();
const long ProjectMissingLibs::ID_STATICLINE11 = wxNewId();
const long ProjectMissingLibs::ID_STATICLINE12 = wxNewId();
const long ProjectMissingLibs::ID_STATICLINE13 = wxNewId();
const long ProjectMissingLibs::ID_STATICLINE14 = wxNewId();
const long ProjectMissingLibs::ID_PANEL1 = wxNewId();
const long ProjectMissingLibs::ID_BUTTON1 = wxNewId();
const long ProjectMissingLibs::ID_BUTTON2 = wxNewId();
const long ProjectMissingLibs::ID_STATICTEXT4 = wxNewId();
//*)

BEGIN_EVENT_TABLE(ProjectMissingLibs,wxScrollingDialog)
	//(*EventTable(ProjectMissingLibs)
	//*)
END_EVENT_TABLE()

ProjectMissingLibs::ProjectMissingLibs(wxWindow* parent,wxArrayString& missingList, TypedResults& currentResults)
    : m_CurrentUrlId( 0 )
    , m_Libs( missingList )
    , m_CurrentResults( currentResults )
    , m_DetectionManager( currentResults )
{
	//(*Initialize(ProjectMissingLibs)
	wxStaticBoxSizer* StaticBoxSizer2;
	wxStaticLine* StaticLine10;
	wxFlexGridSizer* FlexGridSizer1;
	wxFlexGridSizer* FlexGridSizer2;
	wxStaticLine* StaticLine2;
	wxStaticLine* StaticLine11;
	wxStaticLine* StaticLine14;
	wxStaticLine* StaticLine12;
	wxStaticLine* StaticLine13;
	wxStaticText* StaticText1;
	wxStaticText* StaticText3;
	wxStaticLine* StaticLine3;
	wxStaticText* StaticText2;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, wxID_ANY, _("Missing libraries definitions"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Unknown libraries"));
	m_LibsBack = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxSize(308,113), wxSUNKEN_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	m_LibsContainer = new wxFlexGridSizer(0, 5, 0, 0);
	m_LibsContainer->AddGrowableCol(0);
	m_LibsContainer->AddGrowableCol(2);
	m_LibsContainer->AddGrowableCol(4);
	StaticText1 = new wxStaticText(m_LibsBack, ID_STATICTEXT1, _("Name"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	m_LibsContainer->Add(StaticText1, 1, wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticLine2 = new wxStaticLine(m_LibsBack, ID_STATICLINE2, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL, _T("ID_STATICLINE2"));
	m_LibsContainer->Add(StaticLine2, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText2 = new wxStaticText(m_LibsBack, ID_STATICTEXT2, _("Scan"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	m_LibsContainer->Add(StaticText2, 1, wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticLine3 = new wxStaticLine(m_LibsBack, ID_STATICLINE3, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL, _T("ID_STATICLINE3"));
	m_LibsContainer->Add(StaticLine3, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText3 = new wxStaticText(m_LibsBack, ID_STATICTEXT3, _("web"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	m_LibsContainer->Add(StaticText3, 1, wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticLine10 = new wxStaticLine(m_LibsBack, ID_STATICLINE10, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL, _T("ID_STATICLINE10"));
	m_LibsContainer->Add(StaticLine10, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticLine11 = new wxStaticLine(m_LibsBack, ID_STATICLINE11, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL, _T("ID_STATICLINE11"));
	m_LibsContainer->Add(StaticLine11, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticLine12 = new wxStaticLine(m_LibsBack, ID_STATICLINE12, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL, _T("ID_STATICLINE12"));
	m_LibsContainer->Add(StaticLine12, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticLine13 = new wxStaticLine(m_LibsBack, ID_STATICLINE13, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL, _T("ID_STATICLINE13"));
	m_LibsContainer->Add(StaticLine13, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticLine14 = new wxStaticLine(m_LibsBack, ID_STATICLINE14, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL, _T("ID_STATICLINE14"));
	m_LibsContainer->Add(StaticLine14, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_LibsBack->SetSizer(m_LibsContainer);
	m_LibsContainer->SetSizeHints(m_LibsBack);
	StaticBoxSizer1->Add(m_LibsBack, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer2 = new wxFlexGridSizer(0, 2, 5, 5);
	m_MissingDefsBtn = new wxButton(this, ID_BUTTON1, _("Download missing defs."), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	FlexGridSizer2->Add(m_MissingDefsBtn, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button1 = new wxButton(this, ID_BUTTON2, _("Scan for selected"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	FlexGridSizer2->Add(Button1, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(FlexGridSizer2, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Status"));
	m_StatusText = new wxStaticText(this, ID_STATICTEXT4, _("Ready"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	StaticBoxSizer2->Add(m_StatusText, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(StaticBoxSizer2, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ProjectMissingLibs::OnButton1Click);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ProjectMissingLibs::OnButton1Click1);
	//*)

    m_DetectionManager.LoadSearchFilters();
    m_MissingDefsBtn->Enable( AreMissingSearchFilters() );

    RecreateLibsList();
	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
	SetSize(750,550);
	Center();
}

ProjectMissingLibs::~ProjectMissingLibs()
{
	//(*Destroy(ProjectMissingLibs)
	//*)
}

void ProjectMissingLibs::InsertLibEntry( const wxString& entry, bool hasSearchFilter, bool detected )
{
    // Add library name
    m_LibsContainer->Add( new wxStaticText(m_LibsBack,wxID_ANY,entry), 1, wxLEFT|wxRIGHT|wxALIGN_CENTER, 5 );
    m_LibsContainer->Add( new wxStaticLine(m_LibsBack,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxLI_VERTICAL), 1, wxEXPAND );

    // Setup scan option
    if ( !hasSearchFilter || detected )
    {
        // No search filter, can not load the library
        wxStaticText* text = new wxStaticText(m_LibsBack,wxID_ANY, detected ? _("detected") : _("missing definitions") );
        m_LibsContainer->Add( text, 1, wxLEFT|wxRIGHT|wxALIGN_CENTER, 5 );
        m_SearchFlags.Append( text );
    }
    else
    {
        wxCheckBox* box = new wxCheckBox(m_LibsBack,wxID_ANY,wxEmptyString);
        box->SetValue( true );
        m_LibsContainer->Add( box, 1, wxLEFT|wxRIGHT|wxALIGN_CENTER, 5 );
        m_SearchFlags.Append( box );
    }
    m_LibsContainer->Add( new wxStaticLine(m_LibsBack,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxLI_VERTICAL), 1, wxEXPAND );

    // Add web stuff
    m_LibsContainer->Add( new wxStaticText(m_LibsBack,wxID_ANY,_T("---")), 1, wxLEFT|wxRIGHT|wxALIGN_CENTER, 5 );
}

void ProjectMissingLibs::TryDownloadMissing()
{
    WebResourcesManager mgr;
    wxArrayString urls = Manager::Get()->GetConfigManager(_T("lib_finder"))->ReadArrayString(_T("/web/lists"));
    if ( urls.IsEmpty() )
    {
        urls.Add( _T("http://www.codeblocks.org/library_finder/list.xml") );
    }

    if ( !mgr.LoadDetectionConfigurations( urls, this ) )
    {
        cbMessageBox( _("Couldn't connect to servers"), _("Error"), wxICON_ERROR|wxOK, this );
        return;
    }

    for ( size_t i=0; i<m_Libs.Count(); i++ )
    {
        if ( !m_DetectionManager.GetLibrary( m_Libs[i] ) )
        {
            std::vector< char > Content;
            if ( mgr.LoadDetectionConfig( m_Libs[i], Content, this ) )
            {
                m_DetectionManager.StoreNewSettingsFile( m_Libs[i], Content );
            }
        }
    }
}

int ProjectMissingLibs::StartDownloading( const wxString& Url )
{
    m_CurrentUrl = Url;
    m_StatusText->SetLabel( wxString::Format(_("0%% - Downloading %s"),Url.c_str()) );
    return ++m_CurrentUrlId;
}

void ProjectMissingLibs::SetProgress( float progress, int id )
{
    if ( id == m_CurrentUrlId )
    {
        m_StatusText->SetLabel( wxString::Format(_("%.2f%% - Downloading %s"), progress, m_CurrentUrl.c_str()) );
    }
}

void ProjectMissingLibs::JobFinished( int /*id*/ )
{
    m_StatusText->SetLabel( _("Ready") );
}

void ProjectMissingLibs::Error( const wxString& info, int id )
{
    if ( id == m_CurrentUrlId )
    {
        m_StatusText->SetLabel( wxString::Format(_("Error downloading %s - %s"), m_CurrentUrl.c_str(), info.c_str()) );
    }
}

bool ProjectMissingLibs::AreMissingSearchFilters()
{
    for ( size_t i=0; i<m_Libs.Count(); i++ )
    {
        if ( !m_DetectionManager.GetLibrary( m_Libs[i] ) )
        {
            return true;
        }
    }
    return false;
}

void ProjectMissingLibs::OnButton1Click(wxCommandEvent& /*event*/)
{
    TryDownloadMissing();
    m_DetectionManager.LoadSearchFilters();

    if ( AreMissingSearchFilters() )
    {
        cbMessageBox(
            _("Couldn't download settings of some libraries.\n"
              "To make your project compile\n"
              "you will have to define them manually."),
            _("Download missing search settings"),
            wxOK | wxICON_ASTERISK,
            this );
    }

    //m_MissingDefsBtn->Disable();

    // Updating
    RecreateLibsList();
}

void ProjectMissingLibs::RecreateLibsList()
{
    m_LibsContainer->Clear(true);
    m_SearchFlags.Clear();

	m_LibsContainer->Add( new wxStaticText(m_LibsBack, wxID_ANY, _("Name")), 1, wxLEFT|wxRIGHT|wxALIGN_CENTER, 5);
    m_LibsContainer->Add( new wxStaticLine(m_LibsBack,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxLI_VERTICAL), 1, wxEXPAND );
	m_LibsContainer->Add( new wxStaticText(m_LibsBack, wxID_ANY, _("Scan")), 1, wxLEFT|wxRIGHT|wxALIGN_CENTER, 5);
    m_LibsContainer->Add( new wxStaticLine(m_LibsBack,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxLI_VERTICAL), 1, wxEXPAND );
	m_LibsContainer->Add( new wxStaticText(m_LibsBack, wxID_ANY, _("Web")), 1, wxLEFT|wxRIGHT|wxALIGN_CENTER, 5);

    for ( int i=0; i<5; i++ )
    {
        m_LibsContainer->Add( new wxStaticLine(m_LibsBack,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxLI_HORIZONTAL), 1, wxEXPAND );
    }

	for ( size_t i=0; i<m_Libs.Count(); i++ )
	{
	    bool IsFound = m_DetectionManager.GetLibrary( m_Libs[i] ) != 0;
	    bool IsDetected = false;
	    for ( int j=0; j<rtCount; j++ )
	    {
	        if ( m_CurrentResults[j].IsShortCode( m_Libs[i] ) )
	        {
	            IsDetected = true;
	            break;
	        }
	    }
	    InsertLibEntry( m_Libs[i], IsFound, IsDetected );
	}

    m_LibsContainer->Layout();
    m_LibsContainer->Fit(m_LibsBack);
    m_LibsContainer->SetSizeHints(m_LibsBack);

	Layout();

}

void ProjectMissingLibs::OnButton1Click1(wxCommandEvent& /*event*/)
{
    wxArrayString Libs;
    for ( size_t i=0; i<m_Libs.Count(); i++ )
    {
        if ( wxDynamicCast( m_SearchFlags[i], wxCheckBox ) &&
             wxDynamicCast( m_SearchFlags[i], wxCheckBox )->GetValue() )
        {
            Libs.Add( m_Libs[i] );
        }
    }
    if ( Libs.IsEmpty() )
    {
        cbMessageBox(_("No libraries selected for scan"));
        return;
    }

    // Getting list of directories to process
    DirListDlg Dlg(this);
    if ( Dlg.ShowModal() == wxID_CANCEL ) return;

    // Do the processing
    ProcessingDlg PDlg( this, m_DetectionManager, m_CurrentResults );
    PDlg.ShowModal();

    bool apply = PDlg.ReadDirs(Dlg.Dirs) && PDlg.ProcessLibs(Libs);

    PDlg.Hide();

    if ( apply )
    {
        PDlg.ApplyResults( false );
    }

    m_CurrentResults[rtDetected].WriteDetectedResults();

    RecreateLibsList();
}
