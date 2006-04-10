#include "wxsfontfaceeditordlg.h"
#include <wx/fontdlg.h>

BEGIN_EVENT_TABLE(wxsFontFaceEditorDlg,wxDialog)
	//(*EventTable(wxsFontFaceEditorDlg)
	EVT_BUTTON(ID_BUTTON1,wxsFontFaceEditorDlg::OnButton1Click)
	EVT_BUTTON(wxID_OK,wxsFontFaceEditorDlg::OnButton2Click)
	//*)
END_EVENT_TABLE()

wxsFontFaceEditorDlg::wxsFontFaceEditorDlg(wxWindow* parent,wxString& _Face,wxWindowID id):
    Face(_Face)
{
	//(*Initialize(wxsFontFaceEditorDlg)
	Create(parent,id,_("Selecting font face"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE);
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Face name"));
	FaceName = new wxTextCtrl(this,ID_TEXTCTRL1,_T(""),wxDefaultPosition,wxDefaultSize,0);
	if ( 0 ) FaceName->SetMaxLength(0);
	Button1 = new wxButton(this,ID_BUTTON1,_("Pick"),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
	if (false) Button1->SetDefault();
	StaticBoxSizer1->Add(FaceName,1,wxLEFT|wxBOTTOM|wxALIGN_CENTER,3);
	StaticBoxSizer1->Add(Button1,0,wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,3);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	Button2 = new wxButton(this,wxID_OK,_("OK"),wxDefaultPosition,wxDefaultSize,0);
	if (true) Button2->SetDefault();
	Button3 = new wxButton(this,wxID_CANCEL,_("Cancel"),wxDefaultPosition,wxDefaultSize,0);
	if (false) Button3->SetDefault();
	BoxSizer2->Add(60,1,1);
	BoxSizer2->Add(Button2,0,wxALL|wxALIGN_CENTER,5);
	BoxSizer2->Add(Button3,0,wxRIGHT|wxTOP|wxBOTTOM|wxALIGN_CENTER,5);
	BoxSizer2->Add(-1,-1,1);
	BoxSizer1->Add(StaticBoxSizer1,1,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer1->Add(BoxSizer2,0,wxLEFT|wxRIGHT|wxALIGN_CENTER,5);
	this->SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	Center();
	//*)
	FaceName->SetValue(Face);
}

wxsFontFaceEditorDlg::~wxsFontFaceEditorDlg()
{
}

void wxsFontFaceEditorDlg::OnButton2Click(wxCommandEvent& event)
{
    Face = FaceName->GetValue();
    EndModal(wxID_OK);
}

void wxsFontFaceEditorDlg::OnButton1Click(wxCommandEvent& event)
{
    wxFont Font = ::wxGetFontFromUser();
    if ( Font.Ok() )
    {
        FaceName->SetValue(Font.GetFaceName());
    }
}
