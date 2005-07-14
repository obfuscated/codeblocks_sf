#include "wxswnddialog.h"

//(*EventTable(wxsWndDialog)
BEGIN_EVENT_TABLE(wxsWndDialog,wxDialog)
END_EVENT_TABLE()
//*)

wxsWndDialog::wxsWndDialog(wxWidnow* parent,wxWindowID id = -1)
{
    //(*Initialize(wxsWndDialog)
    wxButton* Button2;
    wxCheckBox* CheckBox3;
    wxButton* Button3;
    wxCheckBox* CheckBox4;
    wxComboBox* ComboBox1;
    wxCheckBox* CheckBox5;
    wxListBox* ListBox1;

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
    CheckBox3->SetValue(true);
    Button3 = new wxButton(this,ID_COMMON,wxT("Button"),wxDefaultPosition,wxDefaultSize,0);
    if (false) Button3->SetDefault();
    CheckBox4 = new wxCheckBox(this,ID_COMMON,wxT(""),wxDefaultPosition,wxDefaultSize,0);
    CheckBox4->SetValue(false);
    GridSizer1 = new wxGridSizer(1);
    StaticText1 = new wxStaticText(this,ID_COMMON,wxT("Abc"),wxDefaultPosition,wxSize(100,30),wxALIGN_CENTRE);
    ComboBox1 = new wxComboBox(this,ID_COMMON,wxGetTranslation(""),wxDefaultPosition,wxDefaultSize,0,0,wxCB_READONLY);
    ComboBox1->Append(wxT("First"));
    ComboBox1->Append(wxT("Second"));
    ComboBox1->Append(wxT("Third"));
    ComboBox1->SetSelection(-1);
    CheckBox5 = new wxCheckBox(this,ID_COMMON,wxT(""),wxDefaultPosition,wxDefaultSize,0);
    CheckBox5->SetValue(false);
    ListBox1 = new wxListBox(this,ID_COMMON,wxDefaultPosition,wxSize(-1,50),0,0,0);
    ListBox1->Append(wxT("First"));
    ListBox1->Append(wxT("Second"));
    ListBox1->Append(wxT("Third"));
    ListBox1->Append(wxT("Fourth"));
    ListBox1->SetSelection(-1);
    this->SetSizer(MainSizer);
    //*)
}

wxsWndDialog::~wxsWndDialog()
{
}
