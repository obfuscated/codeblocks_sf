#include "wxsmenueditor.h"

//(*InternalHeaders(wxsMenuEditor)
#include <wx/bitmap.h>
#include <wx/font.h>
#include <wx/fontenum.h>
#include <wx/fontmap.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/settings.h>
//*)

//(*IdInit(wxsMenuEditor)
const long wxsMenuEditor::ID_TREECTRL1 = wxNewId();
const long wxsMenuEditor::ID_RADIOBUTTON1 = wxNewId();
const long wxsMenuEditor::ID_RADIOBUTTON4 = wxNewId();
const long wxsMenuEditor::ID_RADIOBUTTON5 = wxNewId();
const long wxsMenuEditor::ID_RADIOBUTTON2 = wxNewId();
const long wxsMenuEditor::ID_RADIOBUTTON3 = wxNewId();
const long wxsMenuEditor::ID_STATICLINE1 = wxNewId();
const long wxsMenuEditor::ID_STATICTEXT6 = wxNewId();
const long wxsMenuEditor::ID_TEXTCTRL4 = wxNewId();
const long wxsMenuEditor::ID_STATICTEXT1 = wxNewId();
const long wxsMenuEditor::ID_TEXTCTRL1 = wxNewId();
const long wxsMenuEditor::ID_STATICTEXT2 = wxNewId();
const long wxsMenuEditor::ID_TEXTCTRL2 = wxNewId();
const long wxsMenuEditor::ID_STATICTEXT3 = wxNewId();
const long wxsMenuEditor::ID_TEXTCTRL3 = wxNewId();
const long wxsMenuEditor::ID_STATICTEXT4 = wxNewId();
const long wxsMenuEditor::ID_CHECKBOX1 = wxNewId();
const long wxsMenuEditor::ID_STATICTEXT5 = wxNewId();
const long wxsMenuEditor::ID_CHECKBOX2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxsMenuEditor,wxPanel)
	//(*EventTable(wxsMenuEditor)
	//*)
END_EVENT_TABLE()

wxsMenuEditor::wxsMenuEditor(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(wxsMenuEditor)
	Create(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL,_T("id"));
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Content"));
	TreeCtrl1 = new wxTreeCtrl(this,ID_TREECTRL1,wxDefaultPosition,wxSize(160,290),wxTR_DEFAULT_STYLE,wxDefaultValidator,_T("ID_TREECTRL1"));
	StaticBoxSizer1->Add(TreeCtrl1,1,wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	BoxSizer1->Add(StaticBoxSizer1,1,wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxVERTICAL,this,_("Options"));
	GridSizer1 = new wxGridSizer(0,2,0,0);
	RadioButton1 = new wxRadioButton(this,ID_RADIOBUTTON1,_("Normal"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_RADIOBUTTON1"));
	GridSizer1->Add(RadioButton1,1,wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	RadioButton4 = new wxRadioButton(this,ID_RADIOBUTTON4,_("Separator"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_RADIOBUTTON4"));
	GridSizer1->Add(RadioButton4,1,wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	RadioButton5 = new wxRadioButton(this,ID_RADIOBUTTON5,_("Break"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_RADIOBUTTON5"));
	GridSizer1->Add(RadioButton5,1,wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	RadioButton2 = new wxRadioButton(this,ID_RADIOBUTTON2,_("Check"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_RADIOBUTTON2"));
	GridSizer1->Add(RadioButton2,1,wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	RadioButton3 = new wxRadioButton(this,ID_RADIOBUTTON3,_("Radio"),wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_RADIOBUTTON3"));
	GridSizer1->Add(RadioButton3,1,wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	StaticBoxSizer2->Add(GridSizer1,0,wxTOP|wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,10);
	StaticLine1 = new wxStaticLine(this,ID_STATICLINE1,wxDefaultPosition,wxSize(10,-1),wxLI_HORIZONTAL,_T("ID_STATICLINE1"));
	StaticBoxSizer2->Add(StaticLine1,0,wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	FlexGridSizer1 = new wxFlexGridSizer(0,2,0,0);
	StaticText6 = new wxStaticText(this,ID_STATICTEXT6,_("Id:"),wxDefaultPosition,wxDefaultSize,0,_T("ID_STATICTEXT6"));
	FlexGridSizer1->Add(StaticText6,1,wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,4);
	TextCtrl4 = new wxTextCtrl(this,ID_TEXTCTRL4,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_TEXTCTRL4"));
	FlexGridSizer1->Add(TextCtrl4,1,wxTOP|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	StaticText1 = new wxStaticText(this,ID_STATICTEXT1,_("Label:"),wxDefaultPosition,wxDefaultSize,0,_T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1,1,wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,4);
	TextCtrl1 = new wxTextCtrl(this,ID_TEXTCTRL1,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_TEXTCTRL1"));
	FlexGridSizer1->Add(TextCtrl1,1,wxTOP|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	StaticText2 = new wxStaticText(this,ID_STATICTEXT2,_("Accelerator:"),wxDefaultPosition,wxDefaultSize,0,_T("ID_STATICTEXT2"));
	FlexGridSizer1->Add(StaticText2,1,wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,4);
	TextCtrl2 = new wxTextCtrl(this,ID_TEXTCTRL2,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_TEXTCTRL2"));
	FlexGridSizer1->Add(TextCtrl2,1,wxTOP|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	StaticText3 = new wxStaticText(this,ID_STATICTEXT3,_("Help:"),wxDefaultPosition,wxDefaultSize,0,_T("ID_STATICTEXT3"));
	FlexGridSizer1->Add(StaticText3,1,wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,4);
	TextCtrl3 = new wxTextCtrl(this,ID_TEXTCTRL3,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_TEXTCTRL3"));
	FlexGridSizer1->Add(TextCtrl3,1,wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	StaticText4 = new wxStaticText(this,ID_STATICTEXT4,_("Checked:"),wxDefaultPosition,wxDefaultSize,0,_T("ID_STATICTEXT4"));
	FlexGridSizer1->Add(StaticText4,1,wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,4);
	CheckBox1 = new wxCheckBox(this,ID_CHECKBOX1,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_CHECKBOX1"));
	CheckBox1->SetValue(false);
	FlexGridSizer1->Add(CheckBox1,1,wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	StaticText5 = new wxStaticText(this,ID_STATICTEXT5,_("Enabled:"),wxDefaultPosition,wxDefaultSize,0,_T("ID_STATICTEXT5"));
	FlexGridSizer1->Add(StaticText5,1,wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,4);
	CheckBox2 = new wxCheckBox(this,ID_CHECKBOX2,wxEmptyString,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator,_T("ID_CHECKBOX2"));
	CheckBox2->SetValue(false);
	FlexGridSizer1->Add(CheckBox2,1,wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	StaticBoxSizer2->Add(FlexGridSizer1,0,wxTOP|wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,10);
	BoxSizer1->Add(StaticBoxSizer2,0,wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL,4);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)
}

wxsMenuEditor::~wxsMenuEditor()
{
}

