#include "../wxsheaders.h"
#include "wxscheckstringseditor.h"

BEGIN_EVENT_TABLE(wxsCheckStringsEditor,wxDialog)
	//(*EventTable(wxsCheckStringsEditor)
	EVT_BUTTON(ID_BUTTON1,wxsCheckStringsEditor::OnButton1Click)
	EVT_BUTTON(ID_BUTTON2,wxsCheckStringsEditor::OnButton2Click)
	EVT_BUTTON(ID_BUTTON4,wxsCheckStringsEditor::OnButton4Click)
	EVT_BUTTON(ID_BUTTON3,wxsCheckStringsEditor::OnButton3Click)
	EVT_BUTTON(ID_BUTTON5,wxsCheckStringsEditor::OnButton5Click)
	//*)
	EVT_BUTTON(wxID_OK,wxsCheckStringsEditor::OnButton6Click)
	EVT_BUTTON(wxID_CANCEL,wxsCheckStringsEditor::OnButton7Click)
END_EVENT_TABLE()

wxsCheckStringsEditor::wxsCheckStringsEditor(
    wxWindow* parent,
    const wxArrayString& _Strings,
    const wxsArrayBool& _Bools,
    bool _Sorted,
    wxWindowID id):
        Strings(_Strings),
        Bools(_Bools),
        Sorted(_Sorted)
{
	//(*Initialize(wxsCheckStringsEditor)
	Create(parent,id,_("Choices:"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE);
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	EditArea = new wxTextCtrl(this,ID_TEXTCTRL1,_T(""),wxDefaultPosition,wxDefaultSize,0);
	if ( 0 ) EditArea->SetMaxLength(0);
	Button1 = new wxButton(this,ID_BUTTON1,_("Add"),wxDefaultPosition,wxDefaultSize,0);
	if (false) Button1->SetDefault();
	BoxSizer2->Add(EditArea,1,wxLEFT|wxTOP|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer2->Add(Button1,0,wxALL|wxALIGN_CENTER,5);
	StaticLine1 = new wxStaticLine(this,ID_STATICLINE1,wxDefaultPosition,wxSize(10,-1),0);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	StringList = new wxCheckListBox(this,ID_CHECKLISTBOX1,wxDefaultPosition,wxSize(180,255),0,NULL,0);
	BoxSizer4 = new wxBoxSizer(wxVERTICAL);
	Button2 = new wxButton(this,ID_BUTTON2,_("Update"),wxDefaultPosition,wxDefaultSize,0);
	if (false) Button2->SetDefault();
	Button4 = new wxButton(this,ID_BUTTON4,_("Remove"),wxDefaultPosition,wxDefaultSize,0);
	if (false) Button4->SetDefault();
	Button3 = new wxButton(this,ID_BUTTON3,_("Up"),wxDefaultPosition,wxDefaultSize,0);
	if (false) Button3->SetDefault();
	Button5 = new wxButton(this,ID_BUTTON5,_("Down"),wxDefaultPosition,wxDefaultSize,0);
	if (false) Button5->SetDefault();
	BoxSizer4->Add(Button2,0,wxALL|wxALIGN_CENTER,5);
	BoxSizer4->Add(Button4,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER,5);
	BoxSizer4->Add(Button3,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER,5);
	BoxSizer4->Add(Button5,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER,5);
	BoxSizer3->Add(StringList,1,wxLEFT|wxTOP|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer3->Add(BoxSizer4,0,wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP,0);
	StaticLine2 = new wxStaticLine(this,ID_STATICLINE2,wxDefaultPosition,wxSize(10,-1),0);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this,wxID_OK,_T("")));
	StdDialogButtonSizer1->AddButton(new wxButton(this,wxID_CANCEL,_T("")));
	StdDialogButtonSizer1->Realize();
	BoxSizer1->Add(BoxSizer2,0,wxALIGN_CENTER|wxEXPAND,0);
	BoxSizer1->Add(StaticLine1,0,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer1->Add(BoxSizer3,1,wxALIGN_CENTER|wxEXPAND,0);
	BoxSizer1->Add(StaticLine2,0,wxLEFT|wxRIGHT|wxALIGN_CENTER|wxEXPAND,5);
	BoxSizer1->Add(StdDialogButtonSizer1,0,wxALL|wxALIGN_CENTER|wxEXPAND,4);
	this->SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	Center();
	//*)

	if ( Sorted ) InitialRemapBools();

	for ( size_t i = 0; i<Strings.Count(); i++ )
	{
	    StringList->Append(Strings[i]);
	    StringList->Check(i,Bools[i]);
    }
}

wxsCheckStringsEditor::~wxsCheckStringsEditor()
{
}

void wxsCheckStringsEditor::OnButton7Click(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

void wxsCheckStringsEditor::OnButton6Click(wxCommandEvent& event)
{
    size_t Count = StringList->GetCount();
    Strings.Clear();
    Bools.Clear();
    for ( size_t i=0; i<Count; i++ )
    {
        Strings.Add(StringList->GetString(i));
        Bools.Add(StringList->IsChecked(i));
    }
    if ( Sorted ) FinalRemapBools();
    EndModal(wxID_OK);
}

void wxsCheckStringsEditor::OnButton1Click(wxCommandEvent& event)
{
    StringList->Append(EditArea->GetValue());
}

void wxsCheckStringsEditor::OnButton2Click(wxCommandEvent& event)
{
    int Sel = StringList->GetSelection();
    if ( Sel == wxNOT_FOUND ) return;
    StringList->SetString(Sel,EditArea->GetValue());
}

void wxsCheckStringsEditor::OnButton4Click(wxCommandEvent& event)
{
    int Sel = StringList->GetSelection();
    if ( Sel == wxNOT_FOUND ) return;
    StringList->Delete(Sel);
}

void wxsCheckStringsEditor::OnButton3Click(wxCommandEvent& event)
{
    int Sel = StringList->GetSelection();
    if ( Sel == wxNOT_FOUND ) return;
    if ( Sel < 1 ) return;
    bool Checked = StringList->IsChecked(Sel);
    wxString Str = StringList->GetString(Sel);
    StringList->Delete(Sel);
    StringList->Insert(Str,--Sel);
    StringList->Check(Sel,Checked);
    StringList->SetSelection(Sel);
}

void wxsCheckStringsEditor::OnButton5Click(wxCommandEvent& event)
{
    int Sel = StringList->GetSelection();
    if ( Sel == wxNOT_FOUND ) return;
    if ( Sel >= StringList->GetCount()-1 ) return;
    bool Checked = StringList->IsChecked(Sel);
    wxString Str = StringList->GetString(Sel);
    StringList->Delete(Sel);
    StringList->Insert(Str,++Sel);
    StringList->Check(Sel,Checked);
    StringList->SetSelection(Sel);
}

void wxsCheckStringsEditor::OnListClick(wxCommandEvent& event)
{
    EditArea->SetValue(StringList->GetStringSelection());
}

void wxsCheckStringsEditor::InitialRemapBools()
{
    // Need to fixup boolean order because sort flag is on
    // checked attribs are relative to sorted strings, not
    // the order in xrc file (surely XRC bug but we will keep
    // standards)

    SortArray Sort;
    for ( size_t i = 0; i<Strings.Count(); ++i )
    {
        Sort.Add(new SortItem(Strings[i],i) );
    }
    Sort.Sort(SortCmpFunc);

    wxsArrayBool NewBools;
    NewBools.Add(false,Bools.Count());

    for ( size_t i = 0; i<Sort.Count(); ++i )
    {
        NewBools[Sort[i]->InitialIndex] = Bools[i];
    }
    Bools = NewBools;

    for ( size_t i = 0; i<Sort.Count(); ++i )
    {
        delete Sort[i];
    }
}

void wxsCheckStringsEditor::FinalRemapBools()
{
    SortArray Sort;
    for ( size_t i = 0; i<Strings.Count(); ++i )
    {
        Sort.Add(new SortItem(Strings[i],i) );
    }
    Sort.Sort(SortCmpFunc);

    wxsArrayBool NewBools;
    NewBools.Add(false,Bools.Count());

    for ( size_t i = 0; i<Sort.Count(); ++i )
    {
        NewBools[i] = Bools[Sort[i]->InitialIndex];
    }
    Bools = NewBools;

    for ( size_t i = 0; i<Sort.Count(); ++i )
    {
        delete Sort[i];
    }
}
