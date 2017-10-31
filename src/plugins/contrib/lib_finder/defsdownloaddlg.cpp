#include <memory>
#include "defsdownloaddlg.h"

//(*InternalHeaders(DefsDownloadDlg)
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/string.h>
#include <wx/intl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/treectrl.h>
#include <wx/listbox.h>
//*)
#include <wx/mstream.h>
#include <wx/url.h>
#include <manager.h>
#include <configmanager.h>
#include <logmanager.h>

//(*IdInit(DefsDownloadDlg)
const long DefsDownloadDlg::ID_LISTBOX1 = wxNewId();
const long DefsDownloadDlg::ID_BUTTON1 = wxNewId();
const long DefsDownloadDlg::ID_BUTTON2 = wxNewId();
const long DefsDownloadDlg::ID_TREECTRL1 = wxNewId();
const long DefsDownloadDlg::ID_STATICTEXT1 = wxNewId();
const long DefsDownloadDlg::ID_TEXTCTRL2 = wxNewId();
const long DefsDownloadDlg::ID_CHECKBOX1 = wxNewId();
const long DefsDownloadDlg::ID_BUTTON3 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DefsDownloadDlg,wxScrollingDialog)
	//(*EventTable(DefsDownloadDlg)
	//*)
END_EVENT_TABLE()

DefsDownloadDlg::DefsDownloadDlg(wxWindow* parent)
{
	//(*Initialize(DefsDownloadDlg)
	wxStaticBoxSizer* StaticBoxSizer2;
	wxBoxSizer* BoxSizer3;
	wxBoxSizer* BoxSizer7;
	wxBoxSizer* BoxSizer2;
	wxStaticBoxSizer* StaticBoxSizer3;
	wxBoxSizer* BoxSizer4;
	wxBoxSizer* BoxSizer1;
	wxBoxSizer* BoxSizer6;
	wxBoxSizer* BoxSizer5;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, wxID_ANY, _("Download libraries definitions"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	BoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
	BoxSizer5 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Download list"));
	m_UsedLibraries = new wxListBox(this, ID_LISTBOX1, wxDefaultPosition, wxSize(156,290), 0, 0, 0, wxDefaultValidator, _T("ID_LISTBOX1"));
	StaticBoxSizer1->Add(m_UsedLibraries, 1, wxALL|wxEXPAND, 5);
	BoxSizer5->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND, 5);
	BoxSizer6->Add(BoxSizer5, 3, wxEXPAND, 5);
	BoxSizer2 = new wxBoxSizer(wxVERTICAL);
	m_Add = new wxButton(this, ID_BUTTON1, _("<"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON1"));
	m_Add->Disable();
	m_Add->SetToolTip(_("Add selected library to project"));
	BoxSizer2->Add(m_Add, 1, wxEXPAND, 5);
	m_Remove = new wxButton(this, ID_BUTTON2, _(">"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON2"));
	m_Remove->Disable();
	m_Remove->SetToolTip(_("Remove selected library from project"));
	BoxSizer2->Add(m_Remove, 1, wxEXPAND, 5);
	BoxSizer6->Add(BoxSizer2, 0, wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer3 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxVERTICAL, this, _("Known libraries"));
	m_KnownLibrariesTree = new wxTreeCtrl(this, ID_TREECTRL1, wxDefaultPosition, wxDefaultSize, wxTR_HIDE_ROOT|wxTR_SINGLE|wxTR_DEFAULT_STYLE|wxSUNKEN_BORDER, wxDefaultValidator, _T("ID_TREECTRL1"));
	StaticBoxSizer2->Add(m_KnownLibrariesTree, 1, wxALL|wxEXPAND, 5);
	BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Filter:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer4->Add(StaticText1, 0, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
	m_Filter = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	BoxSizer4->Add(m_Filter, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2->Add(BoxSizer4, 0, wxEXPAND, 5);
	BoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
	m_Tree = new wxCheckBox(this, ID_CHECKBOX1, _("Show as tree"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	m_Tree->SetValue(true);
	BoxSizer7->Add(m_Tree, 1, wxLEFT|wxRIGHT|wxEXPAND, 5);
	StaticBoxSizer2->Add(BoxSizer7, 0, wxBOTTOM|wxEXPAND, 5);
	BoxSizer3->Add(StaticBoxSizer2, 1, wxBOTTOM|wxEXPAND, 5);
	StaticBoxSizer3 = new wxStaticBoxSizer(wxVERTICAL, this, _("Configuration"));
	Button1 = new wxButton(this, ID_BUTTON3, _("Servers"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
	StaticBoxSizer3->Add(Button1, 0, wxALL|wxEXPAND, 5);
	BoxSizer3->Add(StaticBoxSizer3, 0, wxEXPAND, 5);
	BoxSizer6->Add(BoxSizer3, 4, wxALL|wxEXPAND, 5);
	BoxSizer1->Add(BoxSizer6, 1, wxALIGN_CENTER_HORIZONTAL, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)

	FetchList();
}

DefsDownloadDlg::~DefsDownloadDlg()
{
	//(*Destroy(DefsDownloadDlg)
	//*)
}

void DefsDownloadDlg::FetchList()
{
    wxArrayString Urls = Manager::Get()->GetConfigManager(_T("lib_finder"))->ReadArrayString(_T("download_urls"));
    Urls.Add(_T("http://localhost/lib_finder_test"));

    for ( size_t i=0; i<Urls.Count(); i++ )
    {
        wxString Url = Urls[i];
        if ( Url.IsEmpty() ) continue;

        if ( Url.Last() != _T('/') ) Url.Append(_T('/'));
        Url << _T("list") << _T(".xml");

        wxURL UrlData(Url);
        if ( !UrlData.IsOk() )
        {
            LogManager::Get()->LogWarning(F(_T("lib_finder: Invalid url '%s'"),Url.wx_str()));
            continue;
        }
        UrlData.SetProxy( ConfigManager::GetProxy() );

        std::unique_ptr< wxInputStream > is ( UrlData.GetInputStream() );
        if ( !is.get() || !is->IsOk() )
        {
            LogManager::Get()->LogWarning(F(_T("lib_finder: Couldn't open stream for '%s'"),Url.wx_str()));
            continue;
        }

        wxMemoryOutputStream memory;
        is->Read( memory );

        // Don't need input stream anymore
        is.reset();

        // Add padding zero to have valid ASCII-z string
        memory.PutC(0);

        // Parse XML data
        TiXmlDocument doc;
        if ( !doc.Parse( (const char*) memory.GetOutputStreamBuffer()->GetBufferStart() ) )
        {
            LogManager::Get()->LogWarning(F(_T("lib_finder: Invalid XML data in '%s'"),Url.wx_str()));
            continue;
        }


    }
}
