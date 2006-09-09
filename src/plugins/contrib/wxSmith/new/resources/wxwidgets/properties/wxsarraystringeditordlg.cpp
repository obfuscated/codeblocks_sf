#include "wxsarraystringeditordlg.h"

#include <wx/tokenzr.h>

BEGIN_EVENT_TABLE(wxsArrayStringEditorDlg,wxDialog)
	//(*EventTable(wxsArrayStringEditorDlg)
	EVT_BUTTON(wxID_OK,wxsArrayStringEditorDlg::OnOK)
	EVT_BUTTON(wxID_CANCEL,wxsArrayStringEditorDlg::OnCancel)
	//*)
END_EVENT_TABLE()

wxsArrayStringEditorDlg::wxsArrayStringEditorDlg(wxWindow* parent,wxArrayString& _Data,wxWindowID id):
    Data(_Data)
{
	//(*Initialize(wxsArrayStringEditorDlg)
	wxBoxSizer* BoxSizer1;
	wxStaticBoxSizer* StaticBoxSizer1;
	wxBoxSizer* BoxSizer2;
	wxButton* Button1;
	wxButton* Button2;
	
	Create(parent,id,_("Edit items"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE);
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Enter items (one item per line)"));
	Items = new wxTextCtrl(this,ID_TEXTCTRL1,_T(""),wxDefaultPosition,wxSize(350,200),wxTE_MULTILINE);
	if ( 0 ) Items->SetMaxLength(0);
	StaticBoxSizer1->Add(Items,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER,2);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	Button1 = new wxButton(this,wxID_OK,_("OK"),wxDefaultPosition,wxDefaultSize,0);
	if (true) Button1->SetDefault();
	Button2 = new wxButton(this,wxID_CANCEL,_("Cancel"),wxDefaultPosition,wxDefaultSize,0);
	if (false) Button2->SetDefault();
	BoxSizer2->Add(Button1,1,wxLEFT|wxTOP|wxBOTTOM|wxALIGN_CENTER,5);
	BoxSizer2->Add(Button2,1,wxALL|wxALIGN_CENTER,5);
	BoxSizer1->Add(StaticBoxSizer1,1,wxALL|wxALIGN_CENTER,5);
	BoxSizer1->Add(BoxSizer2,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL,5);
	this->SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)
	
	size_t Count = Data.Count();
	for ( size_t i = 0; i<Count; i++ )
	{
	    Items->AppendText(Data[i]);
	    Items->AppendText(_T('\n'));
	}
}

wxsArrayStringEditorDlg::~wxsArrayStringEditorDlg()
{
}


void wxsArrayStringEditorDlg::OnOK(wxCommandEvent& event)
{
    wxStringTokenizer Tknz(Items->GetValue(),_T("\n"),wxTOKEN_RET_EMPTY);
    Data.Clear();
    
    while ( Tknz.HasMoreTokens() )
    {
        wxString Line = Tknz.GetNextToken();
        Data.Add(Line);
    }
    
    EndModal(wxID_OK);
}
