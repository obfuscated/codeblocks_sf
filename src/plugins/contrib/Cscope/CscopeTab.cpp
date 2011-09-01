#include "CscopeConfig.h"
#include "CscopeTab.h"
#include <manager.h>
#include <editormanager.h>
#include <cbeditor.h>

#include <wx/clipbrd.h>
#include <wx/dataobj.h>

CscopeTab::CscopeTab( wxWindow* parent, CscopeConfig *cfg )
    : wxPanel( parent )
    , m_table(NULL)
    , m_cfg(cfg)
{

    wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );

    m_pListCtrl = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,  wxLC_REPORT | wxLC_VRULES |wxLC_SINGLE_SEL);
    mainSizer->Add( m_pListCtrl, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );

	m_statusMessage = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_statusMessage->Wrap( -1 );
	bSizer3->Add( m_statusMessage, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 5 );

	m_gauge = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxSize( -1,15 ), wxGA_HORIZONTAL|wxGA_SMOOTH );
	m_gauge->SetValue( 0 );
	bSizer3->Add( m_gauge, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 0 );

	mainSizer->Add( bSizer3, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );

	this->SetSizer( mainSizer );
	this->Layout();

	// Connect Events
	m_pListCtrl->Connect(wxEVT_COMMAND_LIST_ITEM_ACTIVATED,wxListEventHandler(CscopeTab::OnListItemActivated),NULL,this);

    Clear();

	SetMessage(_T("Ready"), 0);
}

CscopeTab::~CscopeTab()
{
    // Disconnect Events
	m_pListCtrl->Disconnect(wxEVT_COMMAND_LIST_ITEM_ACTIVATED,wxListEventHandler(CscopeTab::OnListItemActivated),NULL,this);
	if ( m_table )
        delete m_table;
    m_table = NULL;
}


void CscopeTab::Clear()
{
    if ( m_table )
        delete m_table;
    m_table = NULL;

	m_pListCtrl->ClearAll();
    m_pListCtrl->InsertColumn(0,_T("File"),wxLIST_FORMAT_LEFT, 100);
    m_pListCtrl->InsertColumn(1,_T("Line"),wxLIST_FORMAT_CENTER, 50);
    m_pListCtrl->InsertColumn(2,_T("Scope"),wxLIST_FORMAT_LEFT, 50);
    m_pListCtrl->InsertColumn(3,_T("Text"),wxLIST_FORMAT_LEFT, 500);
}

void CscopeTab::BuildTable(CscopeResultTable *table)
{
	if ( !table ) return;
	Clear();
	m_table = table;

	CscopeResultTable::iterator iter = table->begin();
    unsigned int i = 0;
	for (; iter != table->end(); iter++, i++ )
	{
	    long idx = m_pListCtrl->InsertItem(i, iter->GetFile() );
	    m_pListCtrl->SetItem(idx, 1, wxString::Format(_T("%d"),iter->GetLine()));
	    m_pListCtrl->SetItem(idx, 2, iter->GetScope() );
	    m_pListCtrl->SetItem(idx, 3, iter->GetPattern() );
	}

	//
	for ( i = 0 ; i < 4; ++i )
	    m_pListCtrl->SetColumnWidth(i, wxLIST_AUTOSIZE );
}

void CscopeTab::OnListItemActivated(wxListEvent &event)
{
    if ( !m_table ) return;

    long idx = event.GetIndex();

    CscopeEntryData data = m_table->at(idx);

    cbEditor *ed = Manager::Get()->GetEditorManager()->Open( data.GetFile() );
    if ( ed )
        ed->GotoLine(data.GetLine()-1);
    event.Skip();
}

void CscopeTab::CopyContentsToClipboard(bool selectionOnly)
{
    if ( !m_table ) return;
    CscopeEntryData data;
    int state = wxLIST_STATE_DONTCARE;

    if ( m_pListCtrl->GetSelectedItemCount() && selectionOnly)
        state = wxLIST_STATE_SELECTED;

    wxString str;
    long item = -1;
    for ( ;; )
    {
        item = m_pListCtrl->GetNextItem(item, wxLIST_NEXT_ALL, state);
        if ( item == -1 )
            break;
        data = m_table->at(item);
        str += data.GetFile() + _T('|') + wxString::Format(_T("%d|"), data.GetLine()) + data.GetScope() +
               _T('|') + data.GetPattern() + _T("|\n");
    }

    if (wxTheClipboard->Open())
    {
        wxTheClipboard->SetData(new wxTextDataObject(str));
        wxTheClipboard->Close();
    }
}

void CscopeTab::SetMessage(const wxString &msg, int percent)
{
	m_statusMessage->SetLabel(msg);
	m_gauge->SetValue(percent);
}
void CscopeTab::ClearResults(void)
{
    SetMessage(wxT("Ready"), 0);
	Clear();
}
