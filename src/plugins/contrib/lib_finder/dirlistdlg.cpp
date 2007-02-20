#include "dirlistdlg.h"

#include <wx/dirdlg.h>
#include <wx/string.h>
#include <wx/tokenzr.h>

//(*InternalHeaders(DirListDlg)
//*)

//(*IdInit(DirListDlg)
//*)

BEGIN_EVENT_TABLE(DirListDlg,wxDialog)
	//(*EventTable(DirListDlg)
	EVT_BUTTON(ID_BUTTON1,DirListDlg::OnButton1Click)
	EVT_BUTTON(ID_BUTTON2,DirListDlg::OnButton2Click)
	EVT_BUTTON(ID_BUTTON3,DirListDlg::OnButton3Click)
	EVT_BUTTON(ID_BUTTON4,DirListDlg::OnButton4Click)
	//*)
END_EVENT_TABLE()

DirListDlg::DirListDlg(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(DirListDlg)
	wxStaticBoxSizer* StaticBoxSizer1;
	wxButton* Button1;
	wxButton* Button2;

	Create(parent,id,_("List of directories with libraries"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE);
	FlexGridSizer1 = new wxFlexGridSizer(0,1,0,0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Scanned directories:"));
	DirList = new wxTextCtrl(this,ID_TEXTCTRL1,_T(""),wxDefaultPosition,wxSize(292,194),wxTE_MULTILINE);
	if ( 0 ) DirList->SetMaxLength(0);
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	Button1 = new wxButton(this,ID_BUTTON1,_("Add dir"),wxDefaultPosition,wxDefaultSize,0);
	if (false) Button1->SetDefault();
	Button2 = new wxButton(this,ID_BUTTON2,_("Clear All"),wxDefaultPosition,wxDefaultSize,0);
	if (false) Button2->SetDefault();
	BoxSizer1->Add(Button1,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER,5);
	BoxSizer1->Add(Button2,0,wxLEFT|wxRIGHT|wxALIGN_CENTER,5);
	StaticBoxSizer1->Add(DirList,1,wxBOTTOM|wxALIGN_CENTER,5);
	StaticBoxSizer1->Add(BoxSizer1,0,wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP,0);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	Button3 = new wxButton(this,ID_BUTTON3,_("Cancel"),wxDefaultPosition,wxDefaultSize,0);
	if (false) Button3->SetDefault();
	Button4 = new wxButton(this,ID_BUTTON4,_("Next"),wxDefaultPosition,wxDefaultSize,0);
	if (true) Button4->SetDefault();
	BoxSizer2->Add(Button3,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER,5);
	BoxSizer2->Add(Button4,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER,5);
	FlexGridSizer1->Add(StaticBoxSizer1,1,wxALL|wxALIGN_CENTER,5);
	FlexGridSizer1->Add(BoxSizer2,1,wxALIGN_CENTER,0);
	this->SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);
	//*)
}

DirListDlg::~DirListDlg()
{
}

void DirListDlg::OnButton1Click(wxCommandEvent& event)
{
    wxString DirName = ::wxDirSelector();
    if ( DirName.empty() ) return;

    if ( !DirList->GetValue().empty() ) DirList->AppendText(_T("\n"));
    DirList->AppendText(DirName);
}

void DirListDlg::OnButton2Click(wxCommandEvent& event)
{
    DirList->Clear();
}

void DirListDlg::OnButton3Click(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

void DirListDlg::OnButton4Click(wxCommandEvent& event)
{
    wxStringTokenizer Tknz(DirList->GetValue(),_T("\n"));
    Dirs.Clear();
    while ( Tknz.HasMoreTokens() )
    {
        Dirs.Add(Tknz.NextToken());
    }
    EndModal(wxID_OK);
}
