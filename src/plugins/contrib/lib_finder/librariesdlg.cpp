#include "librariesdlg.h"

//(*InternalHeaders(LibrariesDlg)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/wxFlatNotebook/wxFlatNotebook.h>
#include <wx/checkbox.h>
#include <wx/listbox.h>
#include <wx/wxFlatNotebook/renderer.h>
#include <wx/panel.h>
#include <wx/intl.h>
#include <wx/button.h>
#include <wx/string.h>
//*)

#include <sdk.h>
#include <manager.h>
#include <configmanager.h>
#include <globals.h>
#include <wx/tokenzr.h>

#include "dirlistdlg.h"
#include "processingdlg.h"
#include "librarydetectionmanager.h"
#include "libselectdlg.h"
#include "defsdownloaddlg.h"

//(*IdInit(LibrariesDlg)
const long LibrariesDlg::ID_LISTBOX1 = wxNewId();
const long LibrariesDlg::ID_CHECKBOX1 = wxNewId();
const long LibrariesDlg::ID_CHECKBOX2 = wxNewId();
const long LibrariesDlg::ID_BUTTON1 = wxNewId();
const long LibrariesDlg::ID_BUTTON2 = wxNewId();
const long LibrariesDlg::ID_BUTTON11 = wxNewId();
const long LibrariesDlg::ID_BUTTON8 = wxNewId();
const long LibrariesDlg::ID_LISTBOX2 = wxNewId();
const long LibrariesDlg::ID_BUTTON9 = wxNewId();
const long LibrariesDlg::ID_BUTTON10 = wxNewId();
const long LibrariesDlg::ID_BUTTON3 = wxNewId();
const long LibrariesDlg::ID_BUTTON4 = wxNewId();
const long LibrariesDlg::ID_STATICTEXT10 = wxNewId();
const long LibrariesDlg::ID_STATICTEXT1 = wxNewId();
const long LibrariesDlg::ID_STATICTEXT9 = wxNewId();
const long LibrariesDlg::ID_STATICTEXT2 = wxNewId();
const long LibrariesDlg::ID_TEXTCTRL1 = wxNewId();
const long LibrariesDlg::ID_STATICTEXT5 = wxNewId();
const long LibrariesDlg::ID_TEXTCTRL4 = wxNewId();
const long LibrariesDlg::ID_STATICTEXT4 = wxNewId();
const long LibrariesDlg::ID_TEXTCTRL3 = wxNewId();
const long LibrariesDlg::ID_STATICTEXT3 = wxNewId();
const long LibrariesDlg::ID_TEXTCTRL2 = wxNewId();
const long LibrariesDlg::ID_PANEL1 = wxNewId();
const long LibrariesDlg::ID_TEXTCTRL13 = wxNewId();
const long LibrariesDlg::ID_PANEL8 = wxNewId();
const long LibrariesDlg::ID_TEXTCTRL5 = wxNewId();
const long LibrariesDlg::ID_PANEL6 = wxNewId();
const long LibrariesDlg::ID_TEXTCTRL8 = wxNewId();
const long LibrariesDlg::ID_PANEL3 = wxNewId();
const long LibrariesDlg::ID_TEXTCTRL12 = wxNewId();
const long LibrariesDlg::ID_PANEL5 = wxNewId();
const long LibrariesDlg::ID_STATICTEXT6 = wxNewId();
const long LibrariesDlg::ID_TEXTCTRL9 = wxNewId();
const long LibrariesDlg::ID_BUTTON5 = wxNewId();
const long LibrariesDlg::ID_STATICTEXT7 = wxNewId();
const long LibrariesDlg::ID_TEXTCTRL10 = wxNewId();
const long LibrariesDlg::ID_BUTTON6 = wxNewId();
const long LibrariesDlg::ID_STATICTEXT8 = wxNewId();
const long LibrariesDlg::ID_TEXTCTRL11 = wxNewId();
const long LibrariesDlg::ID_BUTTON7 = wxNewId();
const long LibrariesDlg::ID_PANEL4 = wxNewId();
const long LibrariesDlg::ID_TEXTCTRL6 = wxNewId();
const long LibrariesDlg::ID_PANEL7 = wxNewId();
const long LibrariesDlg::ID_TEXTCTRL7 = wxNewId();
const long LibrariesDlg::ID_PANEL2 = wxNewId();
const long LibrariesDlg::ID_STATICTEXT11 = wxNewId();
const long LibrariesDlg::ID_TEXTCTRL14 = wxNewId();
const long LibrariesDlg::ID_PANEL9 = wxNewId();
const long LibrariesDlg::ID_STATICTEXT12 = wxNewId();
const long LibrariesDlg::ID_TEXTCTRL15 = wxNewId();
const long LibrariesDlg::ID_PANEL10 = wxNewId();
const long LibrariesDlg::ID_FLATNOTEBOOK1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(LibrariesDlg,wxScrollingDialog)
    //(*EventTable(LibrariesDlg)
    //*)
    EVT_BUTTON(wxID_OK,LibrariesDlg::OnWrite)
END_EVENT_TABLE()

LibrariesDlg::LibrariesDlg(wxWindow* parent, TypedResults& knownLibraries)
    : m_KnownLibraries(knownLibraries)
    , m_WorkingCopy(knownLibraries)
    , m_SelectedConfig(0)
    , m_WhileUpdating(false)
{
    //(*Initialize(LibrariesDlg)
    wxBoxSizer* BoxSizer4;
    wxStaticBoxSizer* StaticBoxSizer2;
    wxBoxSizer* BoxSizer6;
    wxBoxSizer* BoxSizer19;
    wxBoxSizer* BoxSizer15;
    wxBoxSizer* BoxSizer20;
    wxBoxSizer* BoxSizer5;
    wxBoxSizer* BoxSizer10;
    wxBoxSizer* BoxSizer7;
    wxBoxSizer* BoxSizer8;
    wxBoxSizer* BoxSizer13;
    wxStaticBoxSizer* StaticBoxSizer4;
    wxFlexGridSizer* FlexGridSizer3;
    wxFlexGridSizer* FlexGridSizer2;
    wxBoxSizer* BoxSizer2;
    wxBoxSizer* BoxSizer11;
    wxBoxSizer* BoxSizer16;
    wxBoxSizer* BoxSizer12;
    wxBoxSizer* BoxSizer18;
    wxBoxSizer* BoxSizer14;
    wxStaticBoxSizer* StaticBoxSizer3;
    wxBoxSizer* BoxSizer17;
    wxBoxSizer* BoxSizer1;
    wxBoxSizer* BoxSizer9;
    wxStaticBoxSizer* StaticBoxSizer1;
    wxFlexGridSizer* FlexGridSizer1;
    wxBoxSizer* BoxSizer3;
    wxStdDialogButtonSizer* StdDialogButtonSizer1;

    Create(parent, wxID_ANY, _("Registered libraries"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    BoxSizer16 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer14 = new wxBoxSizer(wxVERTICAL);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Registered libraries"));
    m_Libraries = new wxListBox(this, ID_LISTBOX1, wxDefaultPosition, wxSize(180,250), 0, 0, 0, wxDefaultValidator, _T("ID_LISTBOX1"));
    StaticBoxSizer1->Add(m_Libraries, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_ShowPredefined = new wxCheckBox(this, ID_CHECKBOX1, _("Show Predefined libraries"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
    m_ShowPredefined->SetValue(true);
    StaticBoxSizer1->Add(m_ShowPredefined, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_ShowPkgConfig = new wxCheckBox(this, ID_CHECKBOX2, _("Show Pkg-Config entries"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
    m_ShowPkgConfig->SetValue(false);
    StaticBoxSizer1->Add(m_ShowPkgConfig, 0, wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    Button1 = new wxButton(this, ID_BUTTON1, _("Add"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    BoxSizer2->Add(Button1, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button2 = new wxButton(this, ID_BUTTON2, _("Delete"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    BoxSizer2->Add(Button2, 1, wxLEFT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer1->Add(BoxSizer2, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button3 = new wxButton(this, ID_BUTTON11, _("Download definitions"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON11"));
    StaticBoxSizer1->Add(Button3, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer14->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer4 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Autodetection of libraries"));
    Button8 = new wxButton(this, ID_BUTTON8, _("Scan"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON8"));
    StaticBoxSizer4->Add(Button8, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer14->Add(StaticBoxSizer4, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer16->Add(BoxSizer14, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3 = new wxBoxSizer(wxVERTICAL);
    StaticBoxSizer2 = new wxStaticBoxSizer(wxVERTICAL, this, _("Available configurations"));
    BoxSizer18 = new wxBoxSizer(wxHORIZONTAL);
    m_Configurations = new wxListBox(this, ID_LISTBOX2, wxDefaultPosition, wxSize(201,44), 0, 0, 0, wxDefaultValidator, _T("ID_LISTBOX2"));
    BoxSizer18->Add(m_Configurations, 1, wxTOP|wxBOTTOM|wxLEFT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer17 = new wxBoxSizer(wxVERTICAL);
    m_ConfigUp = new wxButton(this, ID_BUTTON9, _("^"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON9"));
    BoxSizer17->Add(m_ConfigUp, 1, wxTOP|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_ConfigDown = new wxButton(this, ID_BUTTON10, _("v"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON10"));
    BoxSizer17->Add(m_ConfigDown, 1, wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer18->Add(BoxSizer17, 0, wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer4 = new wxBoxSizer(wxVERTICAL);
    m_ConfDuplicate = new wxButton(this, ID_BUTTON3, _("Duplicate"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
    BoxSizer4->Add(m_ConfDuplicate, 1, wxTOP|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_ConfDelete = new wxButton(this, ID_BUTTON4, _("Delete"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON4"));
    BoxSizer4->Add(m_ConfDelete, 1, wxBOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer18->Add(BoxSizer4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer2->Add(BoxSizer18, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText9 = new wxStaticText(this, ID_STATICTEXT10, _("Note: Order on this list set priorities of configurations."), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE, _T("ID_STATICTEXT10"));
    StaticBoxSizer2->Add(StaticText9, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3->Add(StaticBoxSizer2, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer3 = new wxStaticBoxSizer(wxVERTICAL, this, _("Configuration options"));
    FlexGridSizer1 = new wxFlexGridSizer(0, 2, 5, 5);
    FlexGridSizer1->AddGrowableCol(1);
    StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Type:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    FlexGridSizer1->Add(StaticText1, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    m_Type = new wxStaticText(this, ID_STATICTEXT9, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT9"));
    FlexGridSizer1->Add(m_Type, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer3->Add(FlexGridSizer1, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlatNotebook1 = new wxFlatNotebook(this, ID_FLATNOTEBOOK1, wxDefaultPosition, wxDefaultSize, wxFNB_DEFAULT_STYLE|wxFNB_NO_X_BUTTON|wxFNB_NODRAG|wxFNB_FF2|wxCLIP_CHILDREN);
    Panel1 = new wxPanel(FlatNotebook1, ID_PANEL1, wxPoint(53,10), wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    FlexGridSizer2 = new wxFlexGridSizer(0, 2, 5, 0);
    FlexGridSizer2->AddGrowableCol(0);
    FlexGridSizer2->AddGrowableRow(0);
    FlexGridSizer3 = new wxFlexGridSizer(0, 2, 5, 5);
    FlexGridSizer3->AddGrowableCol(1);
    StaticText2 = new wxStaticText(Panel1, ID_STATICTEXT2, _("Name:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    FlexGridSizer3->Add(StaticText2, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    m_Name = new wxTextCtrl(Panel1, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    m_Name->SetToolTip(_("Full name of the library"));
    FlexGridSizer3->Add(m_Name, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText5 = new wxStaticText(Panel1, ID_STATICTEXT5, _("Base path:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
    FlexGridSizer3->Add(StaticText5, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    m_BasePath = new wxTextCtrl(Panel1, ID_TEXTCTRL4, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL4"));
    FlexGridSizer3->Add(m_BasePath, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText4 = new wxStaticText(Panel1, ID_STATICTEXT4, _("Pkg-Config name:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
    FlexGridSizer3->Add(StaticText4, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    m_PkgConfigName = new wxTextCtrl(Panel1, ID_TEXTCTRL3, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL3"));
    FlexGridSizer3->Add(m_PkgConfigName, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText3 = new wxStaticText(Panel1, ID_STATICTEXT3, _("Description:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    FlexGridSizer3->Add(StaticText3, 1, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    m_Description = new wxTextCtrl(Panel1, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxSize(103,116), wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL2"));
    FlexGridSizer3->Add(m_Description, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer2->Add(FlexGridSizer3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel1->SetSizer(FlexGridSizer2);
    FlexGridSizer2->Fit(Panel1);
    FlexGridSizer2->SetSizeHints(Panel1);
    Panel8 = new wxPanel(FlatNotebook1, ID_PANEL8, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL8"));
    BoxSizer15 = new wxBoxSizer(wxHORIZONTAL);
    m_Categories = new wxTextCtrl(Panel8, ID_TEXTCTRL13, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL13"));
    BoxSizer15->Add(m_Categories, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel8->SetSizer(BoxSizer15);
    BoxSizer15->Fit(Panel8);
    BoxSizer15->SetSizeHints(Panel8);
    Panel2 = new wxPanel(FlatNotebook1, ID_PANEL6, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL6"));
    BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    m_Compilers = new wxTextCtrl(Panel2, ID_TEXTCTRL5, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL5"));
    BoxSizer5->Add(m_Compilers, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel2->SetSizer(BoxSizer5);
    BoxSizer5->Fit(Panel2);
    BoxSizer5->SetSizeHints(Panel2);
    Panel4 = new wxPanel(FlatNotebook1, ID_PANEL3, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL3"));
    BoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    m_Defines = new wxTextCtrl(Panel4, ID_TEXTCTRL8, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL8"));
    BoxSizer8->Add(m_Defines, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel4->SetSizer(BoxSizer8);
    BoxSizer8->Fit(Panel4);
    BoxSizer8->SetSizeHints(Panel4);
    Panel6 = new wxPanel(FlatNotebook1, ID_PANEL5, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL5"));
    BoxSizer13 = new wxBoxSizer(wxHORIZONTAL);
    m_Libs = new wxTextCtrl(Panel6, ID_TEXTCTRL12, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL12"));
    BoxSizer13->Add(m_Libs, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel6->SetSizer(BoxSizer13);
    BoxSizer13->Fit(Panel6);
    BoxSizer13->SetSizeHints(Panel6);
    Panel5 = new wxPanel(FlatNotebook1, ID_PANEL4, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL4"));
    BoxSizer9 = new wxBoxSizer(wxVERTICAL);
    StaticText6 = new wxStaticText(Panel5, ID_STATICTEXT6, _("Compiler:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
    BoxSizer9->Add(StaticText6, 0, wxTOP|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer10 = new wxBoxSizer(wxHORIZONTAL);
    m_CompilerDirs = new wxTextCtrl(Panel5, ID_TEXTCTRL9, wxEmptyString, wxDefaultPosition, wxSize(25,12), wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL9"));
    BoxSizer10->Add(m_CompilerDirs, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button5 = new wxButton(Panel5, ID_BUTTON5, _("+"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON5"));
    BoxSizer10->Add(Button5, 0, wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    BoxSizer9->Add(BoxSizer10, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText7 = new wxStaticText(Panel5, ID_STATICTEXT7, _("Linker:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
    BoxSizer9->Add(StaticText7, 0, wxTOP|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer11 = new wxBoxSizer(wxHORIZONTAL);
    m_LinkerDir = new wxTextCtrl(Panel5, ID_TEXTCTRL10, wxEmptyString, wxDefaultPosition, wxSize(48,8), wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL10"));
    BoxSizer11->Add(m_LinkerDir, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button6 = new wxButton(Panel5, ID_BUTTON6, _("+"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON6"));
    BoxSizer11->Add(Button6, 0, wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    BoxSizer9->Add(BoxSizer11, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText8 = new wxStaticText(Panel5, ID_STATICTEXT8, _("Extra objects (like resources):"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
    BoxSizer9->Add(StaticText8, 0, wxTOP|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer12 = new wxBoxSizer(wxHORIZONTAL);
    m_ObjectsDir = new wxTextCtrl(Panel5, ID_TEXTCTRL11, wxEmptyString, wxDefaultPosition, wxSize(78,12), wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL11"));
    BoxSizer12->Add(m_ObjectsDir, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button7 = new wxButton(Panel5, ID_BUTTON7, _("+"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON7"));
    BoxSizer12->Add(Button7, 0, wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 5);
    BoxSizer9->Add(BoxSizer12, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel5->SetSizer(BoxSizer9);
    BoxSizer9->Fit(Panel5);
    BoxSizer9->SetSizeHints(Panel5);
    Panel7 = new wxPanel(FlatNotebook1, ID_PANEL7, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL7"));
    BoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    m_CFlags = new wxTextCtrl(Panel7, ID_TEXTCTRL6, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL6"));
    BoxSizer6->Add(m_CFlags, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel7->SetSizer(BoxSizer6);
    BoxSizer6->Fit(Panel7);
    BoxSizer6->SetSizeHints(Panel7);
    Panel3 = new wxPanel(FlatNotebook1, ID_PANEL2, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL2"));
    BoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
    m_LFlags = new wxTextCtrl(Panel3, ID_TEXTCTRL7, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL7"));
    BoxSizer7->Add(m_LFlags, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel3->SetSizer(BoxSizer7);
    BoxSizer7->Fit(Panel3);
    BoxSizer7->SetSizeHints(Panel3);
    Panel9 = new wxPanel(FlatNotebook1, ID_PANEL9, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL9"));
    BoxSizer19 = new wxBoxSizer(wxVERTICAL);
    StaticText10 = new wxStaticText(Panel9, ID_STATICTEXT11, _("Header files used by this library:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT11"));
    BoxSizer19->Add(StaticText10, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_Headers = new wxTextCtrl(Panel9, ID_TEXTCTRL14, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL14"));
    BoxSizer19->Add(m_Headers, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel9->SetSizer(BoxSizer19);
    BoxSizer19->Fit(Panel9);
    BoxSizer19->SetSizeHints(Panel9);
    Panel10 = new wxPanel(FlatNotebook1, ID_PANEL10, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL10"));
    BoxSizer20 = new wxBoxSizer(wxVERTICAL);
    StaticText11 = new wxStaticText(Panel10, ID_STATICTEXT12, _("List of required libraries:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT12"));
    BoxSizer20->Add(StaticText11, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_Required = new wxTextCtrl(Panel10, ID_TEXTCTRL15, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL15"));
    BoxSizer20->Add(m_Required, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel10->SetSizer(BoxSizer20);
    BoxSizer20->Fit(Panel10);
    BoxSizer20->SetSizeHints(Panel10);
    FlatNotebook1->AddPage(Panel1, _("Base options"), false);
    FlatNotebook1->AddPage(Panel8, _("Categories"), false);
    FlatNotebook1->AddPage(Panel2, _("Compilers"), false);
    FlatNotebook1->AddPage(Panel4, _("Defines"), false);
    FlatNotebook1->AddPage(Panel6, _("Libs"), false);
    FlatNotebook1->AddPage(Panel5, _("Directories"), false);
    FlatNotebook1->AddPage(Panel7, _("CFlags"), false);
    FlatNotebook1->AddPage(Panel3, _("LFlags"), false);
    FlatNotebook1->AddPage(Panel9, _("Headers"), false);
    FlatNotebook1->AddPage(Panel10, _("Requirements"), false);
    StaticBoxSizer3->Add(FlatNotebook1, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer3->Add(StaticBoxSizer3, 0, wxTOP|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer16->Add(BoxSizer3, 2, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(BoxSizer16, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
    StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
    StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
    StdDialogButtonSizer1->Realize();
    BoxSizer1->Add(StdDialogButtonSizer1, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(BoxSizer1);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);

    Connect(ID_LISTBOX1,wxEVT_COMMAND_LISTBOX_SELECTED,(wxObjectEventFunction)&LibrariesDlg::Onm_LibrariesSelect);
    Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&LibrariesDlg::Onm_ShowPredefinedClick);
    Connect(ID_CHECKBOX2,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&LibrariesDlg::Onm_ShowPkgConfigClick);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&LibrariesDlg::OnButton1Click);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&LibrariesDlg::OnButton2Click);
    Connect(ID_BUTTON11,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&LibrariesDlg::OnButton3Click);
    Connect(ID_BUTTON8,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&LibrariesDlg::OnButton8Click);
    Connect(ID_LISTBOX2,wxEVT_COMMAND_LISTBOX_SELECTED,(wxObjectEventFunction)&LibrariesDlg::Onm_ConfigurationsSelect);
    Connect(ID_BUTTON9,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&LibrariesDlg::Onm_ConfigPosChangeUp);
    Connect(ID_BUTTON10,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&LibrariesDlg::Onm_ConfigPosChangeDown);
    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&LibrariesDlg::Onm_ConfDuplicateClick);
    Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&LibrariesDlg::Onm_ConfDeleteClick);
    Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&LibrariesDlg::Onm_NameText);
    Connect(wxID_ANY,wxEVT_INIT_DIALOG,(wxObjectEventFunction)&LibrariesDlg::OnInit);
    //*)

    RecreateLibrariesList(_T(""));
}

LibrariesDlg::~LibrariesDlg()
{
    //(*Destroy(LibrariesDlg)
    //*)
}


void LibrariesDlg::OnInit(wxInitDialogEvent& event)
{
    event.Skip();
}

void LibrariesDlg::OnButton8Click(wxCommandEvent& /*event*/)
{
    LibraryDetectionManager m_Manager(m_WorkingCopy);

    // Loading library search filters
    if ( !m_Manager.LoadSearchFilters() )
    {
        cbMessageBox(
            _("Didn't found any search filters used to detect libraries.\n"
              "Please check if lib_finder plugin is installed properly."));
        return;
    }

    // Getting list of directories to process
    DirListDlg Dlg(this);
    if ( Dlg.ShowModal() == wxID_CANCEL ) return;

    // Do the processing
    FileNamesMap FNMap;
    ProcessingDlg PDlg(Manager::Get()->GetAppWindow(),m_Manager,m_WorkingCopy);
    PDlg.ShowModal();

    bool apply = PDlg.ReadDirs(Dlg.Dirs) && PDlg.ProcessLibs();

    PDlg.Hide();

    if ( apply )
    {
        PDlg.ApplyResults( false );
    }

    RecreateLibrariesListForceRefresh();
}


void LibrariesDlg::RecreateLibrariesList(const wxString& Selection)
{
    m_Libraries->Clear();

    wxArrayString AllResults;
    bool ShowPredefined = m_ShowPredefined->GetValue();
    bool ShowPkgConfig  = m_ShowPkgConfig->GetValue();

    for ( int i=0; i<rtCount; i++ )
    {
        bool Include = true;
        switch ( i )
        {
            case rtPredefined: Include = ShowPredefined; break;
            case rtPkgConfig:  Include = ShowPkgConfig; break;
            default: break;
        }

        if ( Include )
        {
            m_WorkingCopy[i].GetShortCodes(AllResults);
        }
    }

    AllResults.Sort();

    wxString Prev = wxEmptyString;
    int Index = wxNOT_FOUND;
    for ( size_t i=0; i<AllResults.Count(); i++ )
    {
        if ( Prev != AllResults[i] )
        {
            Prev = AllResults[i];
            int ThisIndex = m_Libraries->Append( Prev );
            if ( Prev == Selection )
            {
                Index = ThisIndex;
            }
        }
    }

    if ( Index == wxNOT_FOUND )
    {
        if ( !m_Libraries->IsEmpty() )
        {
            Index = 0;
        }
    }

    m_Libraries->SetSelection(Index);
    if ( Index != wxNOT_FOUND )
    {
        SelectLibrary( m_Libraries->GetString(Index) );
    }
    else
    {
        SelectLibrary( wxEmptyString );
    }
}

void LibrariesDlg::RecreateLibrariesListForceRefresh()
{
    // We clear current selection before to force reloading of all
    // configurations of library
    wxString Sel = m_SelectedShortcut;
    m_SelectedShortcut.Clear();

    // Rebuilding the list
    RecreateLibrariesList(Sel);
}

void LibrariesDlg::Onm_ShowPredefinedClick(wxCommandEvent& /*event*/)
{
    RecreateLibrariesListForceRefresh();
}

void LibrariesDlg::Onm_ShowPkgConfigClick(wxCommandEvent& /*event*/)
{
    RecreateLibrariesListForceRefresh();
}

void LibrariesDlg::Onm_LibrariesSelect(wxCommandEvent& /*event*/)
{
    SelectLibrary( m_Libraries->GetStringSelection() );
}

void LibrariesDlg::SelectLibrary(const wxString& Shortcut)
{
    if ( Shortcut == m_SelectedShortcut )
    {
        return;
    }

    StoreConfiguration();
    m_SelectedShortcut = Shortcut;
    m_Configurations->Clear();

    int Index = wxNOT_FOUND;
    for ( int i=0; i<rtCount; i++ )
    {
        ResultArray& arr = m_WorkingCopy[i].GetShortCode( Shortcut );
        for ( size_t j=0; j<arr.Count(); j++ )
        {
            LibraryResult* result = arr[j];
            int ThisIndex = m_Configurations->Append( GetDesc(result), (void*)result );
            if ( result == m_SelectedConfig )
            {
                Index = ThisIndex;
            }
        }
    }

    if ( Index == wxNOT_FOUND )
    {
        if ( !m_Configurations->IsEmpty() )
        {
            Index = 0;
        }
    }

    m_Configurations->SetSelection(Index);

    SelectConfiguration( (Index==wxNOT_FOUND) ? 0 : (LibraryResult*)m_Configurations->GetClientData(Index) );
}

void LibrariesDlg::SelectConfiguration(LibraryResult* Configuration)
{
    if ( Configuration == m_SelectedConfig )
    {
        return;
    }
    m_SelectedConfig = Configuration;

    m_WhileUpdating = true;
    if ( !Configuration )
    {
        m_Type->SetLabel( wxEmptyString );
        m_Name->Disable();
        m_Name->Clear();
        m_BasePath->Disable();
        m_BasePath->Clear();
        m_PkgConfigName->Disable();
        m_PkgConfigName->Clear();
        m_Description->Disable();
        m_Description->Clear();
        m_Categories->Disable();
        m_Categories->Clear();
        m_Compilers->Clear();
        m_Compilers->Disable();
        m_Defines->Clear();
        m_Defines->Disable();
        m_Libs->Clear();
        m_Libs->Disable();
        m_CompilerDirs->Clear();
        m_CompilerDirs->Disable();
        m_LinkerDir->Clear();
        m_LinkerDir->Disable();
        m_ObjectsDir->Clear();
        m_ObjectsDir->Disable();
        m_CFlags->Clear();
        m_CFlags->Disable();
        m_LFlags->Clear();
        m_LFlags->Disable();
        m_ConfDelete->Disable();
        m_ConfDuplicate->Disable();
        m_ConfigDown->Disable();
        m_ConfigUp->Disable();
        m_Headers->Disable();
        m_Required->Clear();
        m_Required->Disable();

        m_WhileUpdating = false;
        return;
    }

    bool DisableAll = true;
    switch ( Configuration->Type )
    {
        case rtDetected:
            m_Type->SetLabel(_("Custom"));
            DisableAll = false;
            break;

        case rtPredefined:
            m_Type->SetLabel(_("Predefined"));
            break;

        case rtPkgConfig:
            m_Type->SetLabel(_("Pkg-Config"));
            break;

        case rtCount:
        case rtUnknown:
        default:
            break;
    }

    m_Name->SetValue(Configuration->LibraryName);
    m_Name->Enable( !DisableAll );
    m_BasePath->SetValue(Configuration->BasePath);
    m_BasePath->Enable( !DisableAll );
    m_PkgConfigName->SetValue(Configuration->PkgConfigVar);
    m_PkgConfigName->Enable( !DisableAll );
    m_Description->SetValue(Configuration->Description);
    m_Description->Enable( !DisableAll );
    m_Categories->SetValue(GetStringFromArray(Configuration->Categories,_T("\n")));
    m_Categories->Enable( !DisableAll );
    m_Compilers->SetValue(GetStringFromArray(Configuration->Compilers,_T("\n")));
    m_Compilers->Enable( !DisableAll );
    m_Defines->SetValue(GetStringFromArray(Configuration->Defines,_T("\n")));
    m_Defines->Enable( !DisableAll );
    m_Libs->SetValue(GetStringFromArray(Configuration->Libs,_T("\n")));
    m_Libs->Enable( !DisableAll );
    m_CompilerDirs->SetValue(GetStringFromArray(Configuration->IncludePath,_T("\n")));
    m_CompilerDirs->Enable( !DisableAll );
    m_LinkerDir->SetValue(GetStringFromArray(Configuration->LibPath,_T("\n")));
    m_LinkerDir->Enable( !DisableAll );
    m_ObjectsDir->SetValue(GetStringFromArray(Configuration->ObjPath,_T("\n")));
    m_ObjectsDir->Enable( !DisableAll );
    m_CFlags->SetValue(GetStringFromArray(Configuration->CFlags,_T("\n")));
    m_CFlags->Enable( !DisableAll );
    m_LFlags->SetValue(GetStringFromArray(Configuration->LFlags,_T("\n")));
    m_LFlags->Enable( !DisableAll );
    m_Headers->SetValue(GetStringFromArray(Configuration->Headers,_T("\n")));
    m_Headers->Enable( !DisableAll );
    m_ConfDelete->Enable( !DisableAll && ( m_Configurations->GetCount() > 1 ) );
    m_ConfDuplicate->Enable( true );
    m_Required->Enable( !DisableAll );
    m_Required->SetValue( GetStringFromArray(Configuration->Require,_T("\n")));

    m_ConfigUp->Disable();
    m_ConfigDown->Disable();

    if ( !DisableAll )
    {
        int Selection = m_Configurations->GetSelection();
        if ( Selection>0 )
        {
            m_ConfigUp->Enable();
        }


        if ( Selection < (int)m_Configurations->GetCount()-1 )
        {
            LibraryResult* NextResult = (LibraryResult*)m_Configurations->GetClientData( Selection+1 );
            if ( NextResult && NextResult->Type == rtDetected )
            {
                m_ConfigDown->Enable();
            }
        }
    }

    m_WhileUpdating = false;
}

wxString LibrariesDlg::GetDesc(LibraryResult* Configuration)
{
    wxString ret;
    switch ( Configuration->Type )
    {
        case rtPkgConfig:  ret += _("Pkg-Config: "); break;
        case rtPredefined: ret += _("Predefined: "); break;
        case rtDetected:
        case rtCount:
        case rtUnknown:
        default: break;
    }

    if ( Configuration->LibraryName.IsEmpty() )
    {
        ret += Configuration->ShortCode;
    }
    else
    {
        ret += Configuration->LibraryName;
    }

    if ( !Configuration->Compilers.IsEmpty() )
    {
        ret += _T(" (");
        ret += _("Compilers");

        for ( size_t i=0; i<Configuration->Compilers.Count(); i++ )
        {
            ret += (i==0) ? _T(": ") : _T(", ");
            ret += Configuration->Compilers[i];
        }

        ret += _T(")");
    }

    return ret;
}

void LibrariesDlg::OnWrite(wxCommandEvent& event)
{
    StoreConfiguration();
    //m_WorkingCopy[rtDetected].DebugDump( _T("LibrariesDlg::OnWrite - original") );
    m_KnownLibraries[rtDetected] = m_WorkingCopy[rtDetected];
    //m_KnownLibraries[rtDetected].DebugDump( _T("LibrariesDlg::OnWrite - after copy") );
    m_KnownLibraries[rtDetected].WriteDetectedResults();
    event.Skip();
}

void LibrariesDlg::Onm_ConfDeleteClick(wxCommandEvent& /*event*/)
{
    if ( m_SelectedShortcut.IsEmpty() || !m_SelectedConfig || m_SelectedConfig->Type!=rtDetected )
    {
        return;
    }

    if ( cbMessageBox( _("Do you really want to delete this entry?"), _("Deleting library settings"), wxYES_NO, this ) != wxID_YES )
    {
        return;
    }

    m_WhileUpdating = true;
    m_Configurations->Delete( m_Configurations->GetSelection() );
    m_WhileUpdating = false;
    ResultArray& arr = m_WorkingCopy[rtDetected].GetShortCode( m_SelectedShortcut );
    for ( size_t i=0; i<arr.Count(); i++ )
    {
        if ( arr[i] == m_SelectedConfig )
        {
            arr.RemoveAt(i);
            delete m_SelectedConfig;
            m_SelectedConfig = 0;
            if ( i >= arr.Count() )
            {
                if ( i>0 )
                {
                    i--;
                }
                else
                {
                    m_Configurations->SetSelection( wxNOT_FOUND );
                    SelectConfiguration( 0 );
                    return;
                }
            }

            m_Configurations->SetSelection( i );
            SelectConfiguration( (LibraryResult*) m_Configurations->GetClientData(i) );
        }
    }
}

void LibrariesDlg::Onm_ConfigurationsSelect(wxCommandEvent& /*event*/)
{
    if ( m_WhileUpdating ) return;
    StoreConfiguration();
    int Index = m_Configurations->GetSelection();
    if ( Index == wxNOT_FOUND )
    {
        SelectConfiguration( 0 );
        return;
    }
    SelectConfiguration( (LibraryResult*) m_Configurations->GetClientData(Index) );
}

void LibrariesDlg::Onm_ConfDuplicateClick(wxCommandEvent& /*event*/)
{
    if ( m_SelectedShortcut.IsEmpty() || !m_SelectedConfig )
    {
        return;
    }

    StoreConfiguration();
    ResultArray& arr = m_WorkingCopy[rtDetected].GetShortCode( m_SelectedShortcut );

    // Duplicating entry
    LibraryResult* newResult = new LibraryResult( *m_SelectedConfig );
    newResult->Type = rtDetected;
    arr.Add( newResult );

    // Searching for good place to put new result into the list
    int Index;
    for ( Index = m_Configurations->GetCount(); Index-->0; )
    {
        LibraryResult* conf = (LibraryResult*)m_Configurations->GetClientData( Index );
        if ( !conf ) continue;
        if ( conf->Type == rtDetected )
        {
            break;
        }
    }

    m_Configurations->Insert( GetDesc( newResult ), ++Index, (void*)newResult );
    m_Configurations->SetSelection(Index);
    SelectConfiguration( newResult );
}

void LibrariesDlg::StoreConfiguration()
{
    if ( !m_SelectedConfig ) return;
    if ( m_SelectedConfig->Type != rtDetected ) return;

    m_SelectedConfig->LibraryName  = m_Name->GetValue();
    m_SelectedConfig->BasePath     = m_BasePath->GetValue();
    m_SelectedConfig->PkgConfigVar = m_PkgConfigName->GetValue();
    m_SelectedConfig->Description  = m_Description->GetValue();
    m_SelectedConfig->Categories   = wxStringTokenize( m_Categories->GetValue(),   _T("\n\r"), wxTOKEN_STRTOK );
    m_SelectedConfig->Compilers    = wxStringTokenize( m_Compilers->GetValue(),    _T("\n\r"), wxTOKEN_STRTOK );
    m_SelectedConfig->Defines      = wxStringTokenize( m_Defines->GetValue(),      _T("\n\r"), wxTOKEN_STRTOK );
    m_SelectedConfig->Libs         = wxStringTokenize( m_Libs->GetValue(),         _T("\n\r"), wxTOKEN_STRTOK );
    m_SelectedConfig->IncludePath  = wxStringTokenize( m_CompilerDirs->GetValue(), _T("\n\r"), wxTOKEN_STRTOK );
    m_SelectedConfig->LibPath      = wxStringTokenize( m_LinkerDir->GetValue(),    _T("\n\r"), wxTOKEN_STRTOK );
    m_SelectedConfig->ObjPath      = wxStringTokenize( m_ObjectsDir->GetValue(),   _T("\n\r"), wxTOKEN_STRTOK );
    m_SelectedConfig->CFlags       = wxStringTokenize( m_CFlags->GetValue(),       _T("\n\r"), wxTOKEN_STRTOK );
    m_SelectedConfig->LFlags       = wxStringTokenize( m_LFlags->GetValue(),       _T("\n\r"), wxTOKEN_STRTOK );
    m_SelectedConfig->Headers      = wxStringTokenize( m_Headers->GetValue(),      _T("\n\r"), wxTOKEN_STRTOK );
    m_SelectedConfig->Require      = wxStringTokenize( m_Required->GetValue(),     _T("\n\r"), wxTOKEN_STRTOK );
}

void LibrariesDlg::Onm_NameText(wxCommandEvent& /*event*/)
{
    if ( m_WhileUpdating ) return;
    RefreshConfigurationName();
}

void LibrariesDlg::RefreshConfigurationName()
{
    if ( !m_SelectedConfig ) return;
    StoreConfiguration();
    m_Configurations->SetString( m_Configurations->GetSelection(), GetDesc( m_SelectedConfig ) );
}

void LibrariesDlg::Onm_CompilersText(wxCommandEvent& /*event*/)
{
    if ( m_WhileUpdating ) return;
    RefreshConfigurationName();
}

void LibrariesDlg::OnButton1Click(wxCommandEvent& /*event*/)
{
    StoreConfiguration();

    wxString ShortCode = wxGetTextFromUser( _("Enter Shortcode for new library"), _("New library"), wxEmptyString, this );
    if ( ShortCode.IsEmpty() ) return;

    // Detect whether such shortcode is used
    for ( int i=0; i<rtCount; i++ )
    {
        if ( m_WorkingCopy[i].IsShortCode( ShortCode ) )
        {
            cbMessageBox(
                _("Library with such shortcode already exists.\n"
                  "If you don't see it, make sure that all known\n"
                  "libraries (including those from pkg-config\n"
                  "and predefined ones) are shown."), _("Error"), wxOK | wxICON_ERROR );
            return;
        }
    }

    // Adding new result
    ResultArray& arr = m_WorkingCopy[rtDetected].GetShortCode(ShortCode);
    LibraryResult* res = new LibraryResult;
    res->Type = rtDetected;
    res->LibraryName = ShortCode;
    res->ShortCode = ShortCode;
    arr.Add(res);

    m_SelectedShortcut = ShortCode;
    RecreateLibrariesListForceRefresh();

    //m_WorkingCopy[rtDetected].DebugDump(_T("After add"));
}

void LibrariesDlg::OnButton2Click(wxCommandEvent& /*event*/)
{
    if ( m_SelectedShortcut.IsEmpty() ) return;

    if ( cbMessageBox( _("Do you really want to clear settings of this library?"), _("Removing library settings"), wxYES_NO, this ) != wxID_YES )
    {
        return;
    }

    for ( int i=0; i<rtCount; i++ )
    {
        if ( i == rtDetected )
        {
            break;
        }

        if ( m_WorkingCopy[i].IsShortCode( m_SelectedShortcut ) )
        {
            wxString Msg;
            wxString Cpt;
            switch ( i )
            {
                case rtPkgConfig:
                    Msg = _( "This library has configuration in pkg-config database\n"
                             "which can not be cleared.\n"
                             "Do you want to delete custom resuls only ?" );
                    Cpt = _("Pkg-config settings available");
                    break;

                case rtPredefined:
                    Msg = _( "This library has predefined configuration\n"
                             "which can not be cleared.\n"
                             "Do you want to delete custom resuls only ?" );
                    Cpt = _("Predefined settings available");
                    break;

                default:
                    Msg = _( "This library has static configuration\n"
                             "which can not be cleared.\n"
                             "Do you want to delete custom resuls only ?" );
                    Cpt = _("Static settings available");
            }

            if ( cbMessageBox( Msg, Cpt, wxYES_NO, this ) != wxID_YES )
            {
                return;
            }
            break;
        }
    }

    m_SelectedConfig = 0;
    ResultArray& arr = m_WorkingCopy[rtDetected].GetShortCode( m_SelectedShortcut );
    for ( size_t i=0; i<arr.Count(); i++ )
    {
        delete arr[i];
    }
    arr.Clear();
    RecreateLibrariesListForceRefresh();
}

void LibrariesDlg::Onm_ConfigPosChangeDown(wxCommandEvent& /*event*/)
{
    if ( m_WhileUpdating ) return;
    m_WhileUpdating = true;

    StoreConfiguration();

    int Index = m_Configurations->GetSelection();
    if ( Index!=wxNOT_FOUND )
    {
        m_Configurations->Insert(
            m_Configurations->GetStringSelection(),
            Index + 2,
            m_Configurations->GetClientData( Index ) );
        m_Configurations->Delete( Index );
        m_Configurations->SetSelection( Index+1 );

        LibraryResult* tmp = m_SelectedConfig;
        m_SelectedConfig = 0;
        SelectConfiguration( tmp );
    }

    m_WhileUpdating = false;
}

void LibrariesDlg::Onm_ConfigPosChangeUp(wxCommandEvent& /*event*/)
{
    if ( m_WhileUpdating ) return;
    m_WhileUpdating = true;

    StoreConfiguration();

    int Index = m_Configurations->GetSelection();
    if ( Index!=wxNOT_FOUND )
    {
        m_Configurations->Insert(
            m_Configurations->GetStringSelection(),
            Index - 1,
            m_Configurations->GetClientData( Index ) );
        m_Configurations->Delete( Index+1 );
        m_Configurations->SetSelection( Index-1 );

        LibraryResult* tmp = m_SelectedConfig;
        m_SelectedConfig = 0;
        SelectConfiguration( tmp );
    }
    m_WhileUpdating = false;
}

void LibrariesDlg::OnButton3Click(wxCommandEvent& /*event*/)
{
    DefsDownloadDlg(this).ShowModal();
}
