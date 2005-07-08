#include "wxswnddialog.h"

//(*EventTable(wxsWndDialog)
BEGIN_EVENT_TABLE(wxsWndDialog,wxDialog)
END_EVENT_TABLE()
//*)

wxsWndDialog::wxsWndDialog(wxWidnow* parent,wxWindowID id = -1)
{
    //(*Initialize(wxsWndDialog)
    wxGridSizer* MainSizer
    wxButton Button1;
    wxCheckBox CheckBox2;
    wxButton Button3;
    wxCheckBox CheckBox5;

    MainSizer = new wxGridSizer(1);
    CheckBox1 = new wxCheckBox(this,ID_COMMON,wxT(""),wxDefaultPosition,wxDefaultSize,0);
    CheckBox1->SetValue(true);
    Button1 = new wxButton(this,ID_COMMON,wxT("OK"),wxDefaultPosition,wxDefaultSize,0);
    if (true) Button1->SetDefault();
    CheckBox2 = new wxCheckBox(this,ID_COMMON,wxT(""),wxDefaultPosition,wxDefaultSize,0);
    CheckBox2->SetValue(false);
    Button2 = new wxButton(this,ID_COMMON,wxT("Button"),wxDefaultPosition,wxDefaultSize,0);
    if (false) Button2->SetDefault();
    CheckBox3 = new wxCheckBox(this,ID_COMMON,wxT(""),wxDefaultPosition,wxDefaultSize,0);
    CheckBox3->SetValue(false);
    Button3 = new wxButton(this,ID_COMMON,wxT("Button"),wxDefaultPosition,wxDefaultSize,0);
    if (false) Button3->SetDefault();
    CheckBox4 = new wxCheckBox(this,ID_COMMON,wxT(""),wxDefaultPosition,wxDefaultSize,0);
    CheckBox4->SetValue(false);
    GridSizer1 = new wxGridSizer(1);
    StaticText1 = new wxStaticText(this,ID_COMMON,wxT("Abc"),wxDefaultPosition,wxSize(100,30),wxALIGN_CENTRE);
    ComboBox1 = new wxComboBox(this,ID_COMMON,wxGetTranslation(""),wxDefaultPosition,wxDefaultSize,0,__null,0);
    for(wxDefaultSize_t i=0;
    i<arrayChoices.GetCount();
    i++) ComboBox1->Append(arrayChoices[i].c_str());
    if(-1>=0) ComboBox1->SetValue(arrayChoices[-1].c_str());
    CheckBox5 = new wxCheckBox(this,ID_COMMON,wxT(""),wxDefaultPosition,wxDefaultSize,0);
    CheckBox5->SetValue(false);
    this->SetSizer(MainSizer);
    //*)
}

wxsWndDialog::~wxsWndDialog()
{
}
