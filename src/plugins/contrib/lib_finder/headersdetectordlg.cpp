
//(*InternalHeaders(HeadersDetectorDlg)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/intl.h>
#include <wx/button.h>
#include <wx/string.h>
#include <wx/gauge.h>
//*)

#include <encodingdetector.h>
#include "headersdetectordlg.h"

//(*IdInit(HeadersDetectorDlg)
const long HeadersDetectorDlg::ID_STATICTEXT1 = wxNewId();
const long HeadersDetectorDlg::ID_STATICTEXT3 = wxNewId();
const long HeadersDetectorDlg::ID_STATICTEXT2 = wxNewId();
const long HeadersDetectorDlg::ID_STATICTEXT4 = wxNewId();
const long HeadersDetectorDlg::ID_GAUGE1 = wxNewId();
const long HeadersDetectorDlg::ID_TIMER1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(HeadersDetectorDlg,wxDialog)
	//(*EventTable(HeadersDetectorDlg)
	//*)
	EVT_BUTTON(wxID_CANCEL,HeadersDetectorDlg::Cancel)
END_EVENT_TABLE()

HeadersDetectorDlg::HeadersDetectorDlg(wxWindow* parent,cbProject* project,wxArrayString& headers)
    : m_Project(project)
    , m_Headers(headers)
    , m_Progress(0)
    , m_Finished(false)
    , m_Cancel(false)
{
	//(*Initialize(HeadersDetectorDlg)
	wxBoxSizer* BoxSizer1;
	wxStaticBoxSizer* StaticBoxSizer1;
	wxFlexGridSizer* FlexGridSizer1;
	wxStdDialogButtonSizer* StdDialogButtonSizer1;

	Create(parent, wxID_ANY, _("Detecting missing libraries..."), wxDefaultPosition, wxDefaultSize, wxCAPTION, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Scanning:"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 2, 5, 5);
	FlexGridSizer1->AddGrowableCol(1);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Project:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	m_ProjectName = new wxStaticText(this, ID_STATICTEXT3, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer1->Add(m_ProjectName, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("File:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer1->Add(StaticText2, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	m_FileNameTxt = new wxStaticText(this, ID_STATICTEXT4, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	FlexGridSizer1->Add(m_FileNameTxt, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(FlexGridSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_ProgressBar = new wxGauge(this, ID_GAUGE1, 100, wxDefaultPosition, wxSize(341,15), 0, wxDefaultValidator, _T("ID_GAUGE1"));
	StaticBoxSizer1->Add(m_ProgressBar, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(StaticBoxSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	StdDialogButtonSizer1->Realize();
	BoxSizer1->Add(StdDialogButtonSizer1, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	Timer1.SetOwner(this, ID_TIMER1);
	Timer1.Start(50, false);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	Center();

	Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&HeadersDetectorDlg::OnTimer1Trigger);
	//*)

    m_Thread.m_Dlg = this;
    m_Thread.Create();
    m_Thread.Run();

    m_ProgressBar->SetRange( m_Project->GetFilesCount() );
    m_ProjectName->SetLabel( m_Project->GetTitle() );
}

HeadersDetectorDlg::~HeadersDetectorDlg()
{
    m_Thread.Wait();

	//(*Destroy(HeadersDetectorDlg)
	//*)
}


void HeadersDetectorDlg::OnTimer1Trigger(wxTimerEvent& event)
{
    wxCriticalSectionLocker lock(m_Section);
    Freeze();
    m_FileNameTxt->SetLabel( m_FileName );
    m_ProgressBar->SetValue( m_Progress );
    if ( m_Finished )
    {
        EndModal( m_Cancel ? wxID_CANCEL : wxID_OK );
    }
    Thaw();
}

void HeadersDetectorDlg::Cancel(wxCommandEvent& event)
{
    m_Cancel = true;
}

void HeadersDetectorDlg::ThreadProc()
{
    for ( int i=0; i<m_Project->GetFilesCount(); i++ )
    {
        if ( m_Thread.TestDestroy() || m_Cancel )
        {
            break;
        }

        ProjectFile* file = m_Project->GetFile(i);

        {
            wxCriticalSectionLocker lock(m_Section);
            m_Progress = i;
            m_FileName = file ? file->relativeFilename : _T("");
        }

        ProcessFile( file, m_Headers );
    }
    m_Finished = true;
}

void HeadersDetectorDlg::ProcessFile( ProjectFile* file, wxArrayString& includes )
{
    // We do not care about proper encoding right now.
    // Libraries should never use any native characters in names
    // of their includes and in case of any multibyte encoding
    // multibyte charcters shouldn't hurt us
    // Encoding detector tends to work really slow in some cases

    wxString Ext = file->file.GetExt();
    Ext.MakeLower();

    static const wxChar* Exts[] =
    {
        _T("h"), _T("hxx"), _T("hpp"),
        _T("c"), _T("cpp"), _T("cxx"),
        0
    };

    bool validExt = false;
    for ( const wxChar** ptr = Exts; *ptr; ptr++ )
    {
        if ( Ext == *ptr )
        {
            validExt = true;
            break;
        }
    }
    if ( !validExt )
    {
        return;
    }


    wxFile fl( file->file.GetFullPath() );
    if ( !fl.IsOpened() ) return;
    wxFileOffset contentLength = fl.Length();
    if ( contentLength <= 0 )
    {
        return;
    }
    char* content = new char[contentLength+1];
    char* line = new char[contentLength+1];
    if ( fl.Read(content,contentLength) != contentLength )
    {
        delete[] line;
        delete[] content;
        return;
    }
    content[contentLength] = 0;

    bool blockComment = false;
    for ( size_t pos = 0; pos < static_cast<size_t>(contentLength); )
    {
        // Fetching next line
        char last = 0;
        bool lineEnd = false;
        int lineLength = 0;
        bool lineComment = false;
        bool inStr = false;
        bool inChar = false;
        bool lastCharAdded = false;
        do
        {
            char ch = content[pos++];
            bool thisCharAdded = false;
            switch ( ch )
            {
                case '\n':
                    if ( content[pos] == '\r' )
                    {
                        pos++;
                    }
                    // Continue to \r
                case '\r':
                    if ( last != '\\' )
                    {
                        lineEnd = true;
                        break;
                    }
                    else if ( lastCharAdded )
                    {
                        // Removing last char since it was '\'
                        // which is removed in the
                        // preprocessor level
                        lineLength--;
                    }
                    break;

                case '*':
                    if ( blockComment )
                    {
                        if ( content[pos] == '/' )
                        {
                            pos++;
                            blockComment = false;
                            break;
                        }
                    }
                    else if ( !lineComment )
                    {
                        thisCharAdded = true;
                        line[lineLength++] = ch;
                    }
                    break;

                case '"':
                    if ( !blockComment && !lineComment )
                    {
                        if ( !inChar )
                        {
                            if ( !inStr )
                            {
                                inStr = true;
                            }
                            else if ( last != '\\' )
                            {
                                inStr = false;
                            }
                        }
                        thisCharAdded = true;
                        line[lineLength++] = ch;
                    }
                    break;

                case '\'':
                    if ( !blockComment && !lineComment )
                    {
                        if ( !inStr )
                        {
                            if ( !inChar )
                            {
                                inChar = true;
                            }
                            else if ( last != '\\' )
                            {
                                inChar = false;
                            }
                        }
                        thisCharAdded = true;
                        line[lineLength++] = ch;
                    }
                    break;

                case '/':
                    if ( !blockComment && !lineComment && !inStr && !inChar )
                    {
                        if ( content[pos] == '/' )
                        {
                            pos++;
                            lineComment = true;
                            break;
                        }

                        if ( content[pos] == '*' )
                        {
                            pos++;
                            blockComment = true;
                            break;
                        }
                    }

                    // Contnue to default case
                default:
                    if ( !blockComment && !lineComment )
                    {
                        thisCharAdded = true;
                        line[lineLength++] = ch;
                    }
            }
            last = ch;
            lastCharAdded = thisCharAdded;
        }
        while ( !lineEnd && pos < static_cast<size_t>(contentLength) );
        line[lineLength] = 0;

        // Searching for include

        int i=0;
        while ( line[i]==' ' || line[i]=='\t' ) i++;
        if ( line[i++] == '#' )
        {
            while ( line[i]==' ' || line[i]=='\t' ) i++;
            if ( !strncmp( line+i, "include", 7 ) )
            {
                i += 7;
                while ( line[i]==' ' || line[i]=='\t' ) i++;

                wxString include;

                char readTill =
                    ( line[i] == '<' ) ? '>' :
                    ( line[i] == '"' ) ? '"' :
                      0;

                if ( readTill )
                {
                    i++;
                    while ( line[i] && line[i]!=readTill )
                    {
                        include += (wxChar)line[i++];
                    }
                    if ( line[i] == readTill )
                    {
                        includes.Add( include );
                    }
                }
            }
        }
    }

    delete[] line;
    delete[] content;
}
