/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>

#ifndef CB_PRECOMP
    #include <wx/object.h>
    #include <wx/string.h>
    #include <wx/choicdlg.h> // wxGetSingleChoiceIndex
    #include <wx/file.h>
    #include <wx/filedlg.h>
    #include <wx/utils.h>    // wxWindowDisabler

    //(*InternalHeaders(CCDebugInfo)
    #include <wx/intl.h>
    #include <wx/string.h>
    //*)

    #include <logmanager.h>
#endif

#include <wx/busyinfo.h>

#include "ccdebuginfo.h"
#include "parser/parser.h"


#define CC_DEBUGINFO_DEBUG_OUTPUT 0

#if CC_GLOBAL_DEBUG_OUTPUT == 1
    #undef CC_DEBUGINFO_DEBUG_OUTPUT
    #define CC_DEBUGINFO_DEBUG_OUTPUT 1
#elif CC_GLOBAL_DEBUG_OUTPUT == 2
    #undef CC_DEBUGINFO_DEBUG_OUTPUT
    #define CC_DEBUGINFO_DEBUG_OUTPUT 2
#endif

#if CC_DEBUGINFO_DEBUG_OUTPUT == 1
    #define TRACE(format, args...) \
        CCLogger::Get()->DebugLog(F(format, ##args))
    #define TRACE2(format, args...)
#elif CC_DEBUGINFO_DEBUG_OUTPUT == 2
    #define TRACE(format, args...)                                              \
        do                                                                      \
        {                                                                       \
            if (g_EnableDebugTrace)                                             \
                CCLogger::Get()->DebugLog(F(format, ##args));                   \
        }                                                                       \
        while (false)
    #define TRACE2(format, args...) \
        CCLogger::Get()->DebugLog(F(format, ##args))
#else
    #define TRACE(format, args...)
    #define TRACE2(format, args...)
#endif



//(*IdInit(CCDebugInfo)
const long CCDebugInfo::ID_STATICTEXT29 = wxNewId();
const long CCDebugInfo::ID_TEXTCTRL1 = wxNewId();
const long CCDebugInfo::ID_BUTTON1 = wxNewId();
const long CCDebugInfo::ID_STATICLINE1 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT17 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT18 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT1 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT2 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT9 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT10 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT12 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT3 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT4 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT5 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT6 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT7 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT8 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT36 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT37 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT40 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT41 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT13 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT14 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT15 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT16 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT32 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT33 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT38 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT39 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT19 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT20 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT22 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT24 = wxNewId();
const long CCDebugInfo::ID_BUTTON4 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT30 = wxNewId();
const long CCDebugInfo::ID_COMBOBOX3 = wxNewId();
const long CCDebugInfo::ID_BUTTON5 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT21 = wxNewId();
const long CCDebugInfo::ID_COMBOBOX2 = wxNewId();
const long CCDebugInfo::ID_BUTTON3 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT23 = wxNewId();
const long CCDebugInfo::ID_COMBOBOX1 = wxNewId();
const long CCDebugInfo::ID_BUTTON2 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT25 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT26 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT27 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT28 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT34 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT35 = wxNewId();
const long CCDebugInfo::ID_PANEL1 = wxNewId();
const long CCDebugInfo::ID_LISTBOX1 = wxNewId();
const long CCDebugInfo::ID_PANEL2 = wxNewId();
const long CCDebugInfo::ID_LISTBOX2 = wxNewId();
const long CCDebugInfo::ID_PANEL3 = wxNewId();
const long CCDebugInfo::ID_NOTEBOOK1 = wxNewId();
const long CCDebugInfo::ID_STATICTEXT31 = wxNewId();
const long CCDebugInfo::ID_BUTTON6 = wxNewId();
const long CCDebugInfo::ID_STATICLINE2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(CCDebugInfo,wxScrollingDialog)
    //(*EventTable(CCDebugInfo)
    //*)
END_EVENT_TABLE()

CCDebugInfo::CCDebugInfo(wxWindow* parent, ParserBase* parser, Token* token) :
    m_Parser(parser),
    m_Token(token)
{
    //(*Initialize(CCDebugInfo)
    wxBoxSizer* BoxSizer10;
    wxBoxSizer* BoxSizer2;
    wxPanel* Panel3;
    wxBoxSizer* BoxSizer1;

    Create(parent, wxID_ANY, _("Code-completion debug tool"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxTAB_TRAVERSAL, _T("wxID_ANY"));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    Notebook1 = new wxNotebook(this, ID_NOTEBOOK1, wxDefaultPosition, wxDefaultSize, 0, _T("ID_NOTEBOOK1"));
    Panel1 = new wxPanel(Notebook1, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    BoxSizer2 = new wxBoxSizer(wxVERTICAL);
    BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    StaticText29 = new wxStaticText(Panel1, ID_STATICTEXT29, _("Find:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT29"));
    BoxSizer4->Add(StaticText29, 0, wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 6);
    txtFilter = new wxTextCtrl(Panel1, ID_TEXTCTRL1, _("*"), wxDefaultPosition, wxSize(401,21), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    BoxSizer4->Add(txtFilter, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
    btnFind = new wxButton(Panel1, ID_BUTTON1, _("Find"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    btnFind->SetDefault();
    BoxSizer4->Add(btnFind, 0, wxLEFT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(BoxSizer4, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
    StaticLine1 = new wxStaticLine(Panel1, ID_STATICLINE1, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("ID_STATICLINE1"));
    BoxSizer2->Add(StaticLine1, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
    FlexGridSizer1 = new wxFlexGridSizer(0, 2, 5, 5);
    FlexGridSizer1->AddGrowableCol(1);
    lblID = new wxStaticText(Panel1, ID_STATICTEXT17, _("ID:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT17"));
    FlexGridSizer1->Add(lblID, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    txtID = new wxStaticText(Panel1, ID_STATICTEXT18, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT18"));
    FlexGridSizer1->Add(txtID, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    lblName = new wxStaticText(Panel1, ID_STATICTEXT1, _("Name:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    FlexGridSizer1->Add(lblName, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    txtName = new wxStaticText(Panel1, ID_STATICTEXT2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    FlexGridSizer1->Add(txtName, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    lblKind = new wxStaticText(Panel1, ID_STATICTEXT9, _("Kind:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT9"));
    FlexGridSizer1->Add(lblKind, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    txtKind = new wxStaticText(Panel1, ID_STATICTEXT10, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT10"));
    FlexGridSizer1->Add(txtKind, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    lblScope = new wxStaticText(Panel1, wxID_ANY, _("Scope:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    FlexGridSizer1->Add(lblScope, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    txtScope = new wxStaticText(Panel1, ID_STATICTEXT12, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT12"));
    FlexGridSizer1->Add(txtScope, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    lblType = new wxStaticText(Panel1, ID_STATICTEXT3, _("Type:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    FlexGridSizer1->Add(lblType, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    txtType = new wxStaticText(Panel1, ID_STATICTEXT4, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
    FlexGridSizer1->Add(txtType, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    lblActualType = new wxStaticText(Panel1, ID_STATICTEXT5, _("Actual type:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
    FlexGridSizer1->Add(lblActualType, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    txtActualType = new wxStaticText(Panel1, ID_STATICTEXT6, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
    FlexGridSizer1->Add(txtActualType, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    lblArgs = new wxStaticText(Panel1, ID_STATICTEXT7, _("Arguments:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
    FlexGridSizer1->Add(lblArgs, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    txtArgs = new wxStaticText(Panel1, ID_STATICTEXT8, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
    FlexGridSizer1->Add(txtArgs, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    lblArgsStripped = new wxStaticText(Panel1, ID_STATICTEXT36, _("Arguments (str.):"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT36"));
    FlexGridSizer1->Add(lblArgsStripped, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    txtArgsStripped = new wxStaticText(Panel1, ID_STATICTEXT37, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT37"));
    FlexGridSizer1->Add(txtArgsStripped, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    lblTemplateArg = new wxStaticText(Panel1, ID_STATICTEXT40, _("Templ. args:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT40"));
    FlexGridSizer1->Add(lblTemplateArg, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    txtTemplateArg = new wxStaticText(Panel1, ID_STATICTEXT41, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT41"));
    FlexGridSizer1->Add(txtTemplateArg, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    lblIsOp = new wxStaticText(Panel1, ID_STATICTEXT13, _("Is operator\?"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT13"));
    FlexGridSizer1->Add(lblIsOp, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    txtIsOp = new wxStaticText(Panel1, ID_STATICTEXT14, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT14"));
    FlexGridSizer1->Add(txtIsOp, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    lblIsLocal = new wxStaticText(Panel1, ID_STATICTEXT15, _("Is local\?"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT15"));
    FlexGridSizer1->Add(lblIsLocal, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    txtIsLocal = new wxStaticText(Panel1, ID_STATICTEXT16, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT16"));
    FlexGridSizer1->Add(txtIsLocal, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    lblIsTemp = new wxStaticText(Panel1, ID_STATICTEXT32, _("Is temp\?"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT32"));
    FlexGridSizer1->Add(lblIsTemp, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    txtIsTemp = new wxStaticText(Panel1, ID_STATICTEXT33, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT33"));
    FlexGridSizer1->Add(txtIsTemp, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    lblIsConst = new wxStaticText(Panel1, ID_STATICTEXT38, _("Is const\?"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT38"));
    FlexGridSizer1->Add(lblIsConst, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    txtIsConst = new wxStaticText(Panel1, ID_STATICTEXT39, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT39"));
    FlexGridSizer1->Add(txtIsConst, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    lblNameSpace = new wxStaticText(Panel1, ID_STATICTEXT19, _("Namespace:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT19"));
    FlexGridSizer1->Add(lblNameSpace, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    txtNamespace = new wxStaticText(Panel1, ID_STATICTEXT20, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT20"));
    FlexGridSizer1->Add(txtNamespace, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    lblParent = new wxStaticText(Panel1, ID_STATICTEXT22, _("Parent:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT22"));
    FlexGridSizer1->Add(lblParent, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
    txtParent = new wxStaticText(Panel1, ID_STATICTEXT24, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT24"));
    BoxSizer7->Add(txtParent, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
    btnGoParent = new wxButton(Panel1, ID_BUTTON4, _("Go"), wxDefaultPosition, wxSize(36,23), 0, wxDefaultValidator, _T("ID_BUTTON4"));
    BoxSizer7->Add(btnGoParent, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
    FlexGridSizer1->Add(BoxSizer7, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
    lblChildren = new wxStaticText(Panel1, ID_STATICTEXT30, _("Children:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT30"));
    FlexGridSizer1->Add(lblChildren, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    cmbChildren = new wxComboBox(Panel1, ID_COMBOBOX3, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_READONLY, wxDefaultValidator, _T("ID_COMBOBOX3"));
    BoxSizer8->Add(cmbChildren, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
    btnGoChildren = new wxButton(Panel1, ID_BUTTON5, _("Go"), wxDefaultPosition, wxSize(36,23), 0, wxDefaultValidator, _T("ID_BUTTON5"));
    BoxSizer8->Add(btnGoChildren, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
    FlexGridSizer1->Add(BoxSizer8, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
    lblAncestors = new wxStaticText(Panel1, ID_STATICTEXT21, _("Ancestors:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT21"));
    FlexGridSizer1->Add(lblAncestors, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    cmbAncestors = new wxComboBox(Panel1, ID_COMBOBOX2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_READONLY, wxDefaultValidator, _T("ID_COMBOBOX2"));
    BoxSizer6->Add(cmbAncestors, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
    btnGoAsc = new wxButton(Panel1, ID_BUTTON3, _("Go"), wxDefaultPosition, wxSize(36,23), 0, wxDefaultValidator, _T("ID_BUTTON3"));
    BoxSizer6->Add(btnGoAsc, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
    FlexGridSizer1->Add(BoxSizer6, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
    lblDescendants = new wxStaticText(Panel1, ID_STATICTEXT23, _("Descendants:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT23"));
    FlexGridSizer1->Add(lblDescendants, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    cmbDescendants = new wxComboBox(Panel1, ID_COMBOBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_READONLY, wxDefaultValidator, _T("ID_COMBOBOX1"));
    BoxSizer5->Add(cmbDescendants, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
    btnGoDesc = new wxButton(Panel1, ID_BUTTON2, _("Go"), wxDefaultPosition, wxSize(36,23), 0, wxDefaultValidator, _T("ID_BUTTON2"));
    BoxSizer5->Add(btnGoDesc, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
    FlexGridSizer1->Add(BoxSizer5, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
    lblDeclFile = new wxStaticText(Panel1, ID_STATICTEXT25, _("Decl. filename:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT25"));
    FlexGridSizer1->Add(lblDeclFile, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    txtDeclFile = new wxStaticText(Panel1, ID_STATICTEXT26, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT26"));
    FlexGridSizer1->Add(txtDeclFile, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    lblImplfile = new wxStaticText(Panel1, ID_STATICTEXT27, _("Impl. filename:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT27"));
    FlexGridSizer1->Add(lblImplfile, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    txtImplFile = new wxStaticText(Panel1, ID_STATICTEXT28, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT28"));
    FlexGridSizer1->Add(txtImplFile, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    lblUserData = new wxStaticText(Panel1, ID_STATICTEXT34, _("User data:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT34"));
    FlexGridSizer1->Add(lblUserData, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    txtUserData = new wxStaticText(Panel1, ID_STATICTEXT35, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT35"));
    FlexGridSizer1->Add(txtUserData, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
    BoxSizer2->Add(FlexGridSizer1, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
    Panel1->SetSizer(BoxSizer2);
    BoxSizer2->Fit(Panel1);
    BoxSizer2->SetSizeHints(Panel1);
    Panel2 = new wxPanel(Notebook1, ID_PANEL2, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL2"));
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    lstFiles = new wxListBox(Panel2, ID_LISTBOX1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_LISTBOX1"));
    BoxSizer3->Add(lstFiles, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
    Panel2->SetSizer(BoxSizer3);
    BoxSizer3->Fit(Panel2);
    BoxSizer3->SetSizeHints(Panel2);
    Panel3 = new wxPanel(Notebook1, ID_PANEL3, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL3"));
    BoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    lstDirs = new wxListBox(Panel3, ID_LISTBOX2, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_LISTBOX2"));
    BoxSizer9->Add(lstDirs, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel3->SetSizer(BoxSizer9);
    BoxSizer9->Fit(Panel3);
    BoxSizer9->SetSizeHints(Panel3);
    Notebook1->AddPage(Panel1, _("Tokens"), false);
    Notebook1->AddPage(Panel2, _("Files list"), false);
    Notebook1->AddPage(Panel3, _("Search dirs"), false);
    BoxSizer1->Add(Notebook1, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
    BoxSizer10 = new wxBoxSizer(wxHORIZONTAL);
    lblInfo = new wxStaticText(this, ID_STATICTEXT31, _("The parser contains 0 tokens found in 0 files"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT31"));
    BoxSizer10->Add(lblInfo, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    btnSave = new wxButton(this, ID_BUTTON6, _("Save"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON6"));
    BoxSizer10->Add(btnSave, 0, wxALL|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    BoxSizer1->Add(BoxSizer10, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_BOTTOM, 5);
    StaticLine2 = new wxStaticLine(this, ID_STATICLINE2, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("ID_STATICLINE2"));
    BoxSizer1->Add(StaticLine2, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
    StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, _("Close")));
    StdDialogButtonSizer1->Realize();
    BoxSizer1->Add(StdDialogButtonSizer1, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(BoxSizer1);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);
    Center();

    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CCDebugInfo::OnFindClick);
    Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CCDebugInfo::OnGoParentClick);
    Connect(ID_BUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CCDebugInfo::OnGoChildrenClick);
    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CCDebugInfo::OnGoAscClick);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CCDebugInfo::OnGoDescClick);
    Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CCDebugInfo::OnSave);
    Connect(wxID_ANY,wxEVT_INIT_DIALOG,(wxObjectEventFunction)&CCDebugInfo::OnInit);
    //*)
}

CCDebugInfo::~CCDebugInfo()
{
    //(*Destroy(CCDebugInfo)
    //*)
}


void CCDebugInfo::FillFiles()
{
    TRACK_THREAD_LOCKER(s_TokensTreeCritical);
    wxCriticalSectionLocker locker(s_TokensTreeCritical);
    THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

    lstFiles->Freeze();
    lstFiles->Clear();

    TokensTree* tokens = m_Parser->GetTokensTree();
    for (size_t i = 0; i < tokens->m_FilenamesMap.size(); ++i)
    {
        wxString file = tokens->m_FilenamesMap.GetString(i);
        if (!file.IsEmpty())
            lstFiles->Append(file);
    }

    lstFiles->Thaw();
}

void CCDebugInfo::FillDirs()
{
    lstDirs->Freeze();
    lstDirs->Clear();

    const wxArrayString& dirs = m_Parser->GetIncludeDirs();
    for (size_t i = 0; i < dirs.GetCount(); ++i)
    {
        const wxString& file = dirs[i];
        if (!file.IsEmpty())
            lstDirs->Append(file);
    }

    lstDirs->Thaw();
}

void CCDebugInfo::DisplayTokenInfo()
{
    if (!m_Token)
    {
        txtID->SetLabel(wxEmptyString);
        txtName->SetLabel(wxEmptyString);
        txtKind->SetLabel(wxEmptyString);
        txtScope->SetLabel(wxEmptyString);
        txtType->SetLabel(wxEmptyString);
        txtActualType->SetLabel(wxEmptyString);
        txtArgs->SetLabel(wxEmptyString);
        txtArgsStripped->SetLabel(wxEmptyString);
        txtTemplateArg->SetLabel(wxEmptyString);
        txtIsOp->SetLabel(wxEmptyString);
        txtIsLocal->SetLabel(wxEmptyString);
        txtNamespace->SetLabel(wxEmptyString);
        txtParent->SetLabel(wxEmptyString);
        cmbChildren->Clear();
        cmbAncestors->Clear();
        cmbDescendants->Clear();
        txtDeclFile->SetLabel(wxEmptyString);
        txtImplFile->SetLabel(wxEmptyString);
        return;
    }

    Token* parent = nullptr;
    {
        TRACK_THREAD_LOCKER(s_TokensTreeCritical);
        wxCriticalSectionLocker locker(s_TokensTreeCritical);
        THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

        TokensTree* tokens = m_Parser->GetTokensTree();
        parent = tokens->at(m_Token->m_ParentIndex);
        tokens->RecalcInheritanceChain(m_Token);
    }

    wxString args = m_Token->GetFormattedArgs();
    wxString argsStr = m_Token->m_BaseArgs;
    wxString tmplArg = m_Token->m_TemplateArgument;
    wxString ttype = m_Token->m_Type;

    // so they can be displayed in wxStaticText
    args.Replace(_T("&"), _T("&&"), true);
    argsStr.Replace(_T("&"), _T("&&"), true);
    tmplArg.Replace(_T("&"), _T("&&"), true);
    ttype.Replace(_T("&"), _T("&&"), true);

    txtID->SetLabel(wxString::Format(_T("%d"), m_Token->GetSelf()));
    txtName->SetLabel(m_Token->m_Name);
    txtKind->SetLabel(m_Token->GetTokenKindString());
    txtScope->SetLabel(m_Token->GetTokenScopeString());
    txtType->SetLabel(ttype);
    txtActualType->SetLabel(m_Token->m_ActualType);
    txtArgs->SetLabel(args);
    txtArgsStripped->SetLabel(argsStr);
    txtTemplateArg->SetLabel(tmplArg);
    txtIsOp->SetLabel(m_Token->m_IsOperator ? _("Yes") : _("No"));
    txtIsLocal->SetLabel(m_Token->m_IsLocal ? _("Yes") : _("No"));
    txtIsTemp->SetLabel(m_Token->m_IsTemp ? _("Yes") : _("No"));
    txtIsConst->SetLabel(m_Token->m_IsConst ? _("Yes") : _("No"));
    txtNamespace->SetLabel(m_Token->GetNamespace());
    txtParent->SetLabel(wxString::Format(_T("%s (%d)"), parent ? parent->m_Name.c_str() : (const wxChar*)_("<Global namespace>"), m_Token->m_ParentIndex));

    FillChildren();
    FillAncestors();
    FillDescendants();

    if (!m_Token->GetFilename().IsEmpty())
        txtDeclFile->SetLabel(wxString::Format(_T("%s : %d"), m_Token->GetFilename().c_str(), m_Token->m_Line));
    else
        txtDeclFile->SetLabel(wxEmptyString);
    if (!m_Token->GetImplFilename().IsEmpty())
        txtImplFile->SetLabel(wxString::Format(_("%s : %d (code lines: %d to %d)"), m_Token->GetImplFilename().c_str(), m_Token->m_ImplLine, m_Token->m_ImplLineStart, m_Token->m_ImplLineEnd));
    else
        txtImplFile->SetLabel(wxEmptyString);
    txtUserData->SetLabel(wxString::Format(_T("0x%p"), m_Token->m_UserData));
}

void CCDebugInfo::FillChildren()
{
    TRACK_THREAD_LOCKER(s_TokensTreeCritical);
    wxCriticalSectionLocker locker(s_TokensTreeCritical);
    THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

    TokensTree* tokens = m_Parser->GetTokensTree();
    cmbChildren->Clear();
    for (TokenIdxSet::iterator it = m_Token->m_Children.begin(); it != m_Token->m_Children.end(); ++it)
    {
        Token* child = tokens->at(*it);
        const wxString msgInvalidToken = _("<invalid token>");
        cmbChildren->Append(wxString::Format(_T("%s (%d)"), child ? child->m_Name.wx_str() : msgInvalidToken.wx_str(), *it));
    }
    cmbChildren->SetSelection(0);
}

void CCDebugInfo::FillAncestors()
{
    TRACK_THREAD_LOCKER(s_TokensTreeCritical);
    wxCriticalSectionLocker locker(s_TokensTreeCritical);
    THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

    TokensTree* tokens = m_Parser->GetTokensTree();
    cmbAncestors->Clear();
    for (TokenIdxSet::iterator it = m_Token->m_Ancestors.begin(); it != m_Token->m_Ancestors.end(); ++it)
    {
        Token* ancestor = tokens->at(*it);
        const wxString msgInvalidToken = _("<invalid token>");
        cmbAncestors->Append(wxString::Format(_T("%s (%d)"), ancestor ? ancestor->m_Name.wx_str() : msgInvalidToken.wx_str(), *it));
    }
    cmbAncestors->SetSelection(0);
}

void CCDebugInfo::FillDescendants()
{
    TRACK_THREAD_LOCKER(s_TokensTreeCritical);
    wxCriticalSectionLocker locker(s_TokensTreeCritical);
    THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

    TokensTree* tokens = m_Parser->GetTokensTree();
    cmbDescendants->Clear();
    for (TokenIdxSet::iterator it = m_Token->m_Descendants.begin(); it != m_Token->m_Descendants.end(); ++it)
    {
        Token* descendant = tokens->at(*it);
        const wxString msgInvalidToken = _("<invalid token>");
        cmbDescendants->Append(wxString::Format(_T("%s (%d)"), descendant ? descendant->m_Name.wx_str() : msgInvalidToken.wx_str(), *it));
    }
    cmbDescendants->SetSelection(0);
}

void CCDebugInfo::OnInit(wxInitDialogEvent& /*event*/)
{
    if (!m_Parser)
        return;

    {
        TRACK_THREAD_LOCKER(s_TokensTreeCritical);
        wxCriticalSectionLocker locker(s_TokensTreeCritical);
        THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

        lblInfo->SetLabel(wxString::Format(_("The parser contains %d tokens, found in %d files"),
                                           m_Parser->GetTokensTree()->size(), m_Parser->GetFilesCount()));
    }

    DisplayTokenInfo();
    FillFiles();
    FillDirs();

    txtFilter->SetFocus();
}

void CCDebugInfo::OnFindClick(wxCommandEvent& /*event*/)
{
    {
        TRACK_THREAD_LOCKER(s_TokensTreeCritical);
        wxCriticalSectionLocker locker(s_TokensTreeCritical);
        THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

        TokensTree* tokens = m_Parser->GetTokensTree();
        wxString search = txtFilter->GetValue();

        m_Token = 0;

        // first determine if the user entered an ID or a search mask
        long unsigned id;
        if (search.ToULong(&id, 10))
        {
            // easy; ID
            m_Token = tokens->at(id);
        }
        else
        {
            // find all matching tokens
            TokenIdxSet result;
            for (size_t i = 0; i < tokens->size(); ++i)
            {
                Token* token = tokens->at(i);
                if (token && token->m_Name.Matches(search))
                    result.insert(i);
            }

            // a single result?
            if (result.size() == 1)
            {
                m_Token = tokens->at(*(result.begin()));
            }
            else
            {
                // fill a list and ask the user which token to display
                wxArrayString arr;
                wxArrayInt intarr;
                for (TokenIdxSet::iterator it = result.begin(); it != result.end(); ++it)
                {
                    Token* token = tokens->at(*it);
                    arr.Add(token->DisplayName());
                    intarr.Add(*it);
                }
                int sel = wxGetSingleChoiceIndex(_("Please make a selection:"), _("Multiple matches"), arr, this);
                if (sel == -1)
                    return;

                m_Token = tokens->at(intarr[sel]);
            }
        }
    }

    DisplayTokenInfo();
}

void CCDebugInfo::OnGoAscClick(wxCommandEvent& /*event*/)
{
    int idx = cmbAncestors->GetSelection();
    if (!m_Token || idx == -1)
        return;

    int count = 0;
    for (TokenIdxSet::iterator it = m_Token->m_Ancestors.begin(); it != m_Token->m_Ancestors.end(); ++it)
    {
        if (count == idx)
        {
            {
                TRACK_THREAD_LOCKER(s_TokensTreeCritical);
                wxCriticalSectionLocker locker(s_TokensTreeCritical);
                THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

                m_Token = m_Parser->GetTokensTree()->at(*it);
            }
            DisplayTokenInfo();
            break;
        }
        ++count;
    }
}

void CCDebugInfo::OnGoDescClick(wxCommandEvent& /*event*/)
{
    int idx = cmbDescendants->GetSelection();
    if (!m_Token || idx == -1)
        return;

    int count = 0;
    for (TokenIdxSet::iterator it = m_Token->m_Descendants.begin(); it != m_Token->m_Descendants.end(); ++it)
    {
        if (count == idx)
        {
            {
                TRACK_THREAD_LOCKER(s_TokensTreeCritical);
                wxCriticalSectionLocker locker(s_TokensTreeCritical);
                THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

                m_Token = m_Parser->GetTokensTree()->at(*it);
            }
            DisplayTokenInfo();
            break;
        }
        ++count;
    }
}

void CCDebugInfo::OnGoParentClick(wxCommandEvent& /*event*/)
{
    if (!m_Token || m_Token->m_ParentIndex == -1)
        return;

    {
        TRACK_THREAD_LOCKER(s_TokensTreeCritical);
        wxCriticalSectionLocker locker(s_TokensTreeCritical);
        THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

        m_Token = m_Parser->GetTokensTree()->at(m_Token->m_ParentIndex);
    }

    DisplayTokenInfo();
}

void CCDebugInfo::OnGoChildrenClick(wxCommandEvent& /*event*/)
{
    int idx = cmbChildren->GetSelection();
    if (!m_Token || idx == -1)
        return;

    int count = 0;
    for (TokenIdxSet::iterator it = m_Token->m_Children.begin(); it != m_Token->m_Children.end(); ++it)
    {
        if (count == idx)
        {
            {
                TRACK_THREAD_LOCKER(s_TokensTreeCritical);
                wxCriticalSectionLocker locker(s_TokensTreeCritical);
                THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

                m_Token = m_Parser->GetTokensTree()->at(*it);
            }
            DisplayTokenInfo();
            break;
        }
        ++count;
    }
}

void SaveCCDebugInfo(const wxString& fileDesc, const wxString& content)
{
    wxString fname;
    wxFileDialog dlg (Manager::Get()->GetAppWindow(),
                    fileDesc,
                    _T(""),
                    _T(""),
                    _T("Text files (*.txt)|*.txt|Any file (*)|*"),
                    wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() != wxID_OK)
        return;

    // Opening the file migth have failed, verify:
    wxFile f(dlg.GetPath(), wxFile::write);
    if (f.IsOpened())
    {
        f.Write(content); // write buffer to file
        f.Close();        // release file handle
    }
    else
        cbMessageBox(_("Cannot create file ") + fname, _("CC Debug Info"));
}

void CCDebugInfo::OnSave(wxCommandEvent& /*event*/)
{
    TokensTree* tokens = m_Parser->GetTokensTree();
    if (!tokens)
        return;

    TRACK_THREAD_LOCKER(s_TokensTreeCritical);
    wxCriticalSectionLocker locker(s_TokensTreeCritical);
    THREAD_LOCKER_SUCCESS(s_TokensTreeCritical);

    wxArrayString saveWhat;
    saveWhat.Add(_("Dump the tokens tree"));
    saveWhat.Add(_("Dump the file list"));
    saveWhat.Add(_("Dump the list of include directories"));
    saveWhat.Add(_("Dump the token list of files"));

    int sel = wxGetSingleChoiceIndex(_("What do you want to save?"),
                                     _("CC Debug Info"), saveWhat, this);
    switch (sel)
    {
        case -1:
            // cancelled
            return;

        case 0:
            {
                wxString tt;
                { // life time of wxWindowDisabler/wxBusyInfo
                    wxWindowDisabler disableAll;
                    wxBusyInfo running(_("Obtaining tokens tree... please wait (this may take several seconds)..."),
                                       Manager::Get()->GetAppWindow());
                    tt = tokens->m_Tree.dump();
                }
                SaveCCDebugInfo(_("Save tokens tree"), tt);
            }
            break;
        case 1:
            {
                wxString files;
                for (size_t i = 0; i < tokens->m_FilenamesMap.size(); ++i)
                {
                    wxString file = tokens->m_FilenamesMap.GetString(i);
                    if (!file.IsEmpty())
                        files += file + _T("\r\n");
                }
                SaveCCDebugInfo(_("Save file list"), files);
            }
            break;
        case 2:
            {
                wxString dirs;
                const wxArrayString& dirsArray = m_Parser->GetIncludeDirs();
                for (size_t i = 0; i < dirsArray.GetCount(); ++i)
                {
                    const wxString& dir = dirsArray[i];
                    if (!dir.IsEmpty())
                        dirs += dir + _T("\r\n");
                }
                SaveCCDebugInfo(_("Save list of include directories"), dirs);
            }
            break;
        case 3:
            {
                wxString fileTokens;
                {
                    wxWindowDisabler disableAll;
                    wxBusyInfo running(_("Obtaining tokens tree... please wait (this may take several seconds)..."),
                                       Manager::Get()->GetAppWindow());
                    for (size_t i = 0; i < tokens->m_FilenamesMap.size(); ++i)
                    {
                        const wxString file = tokens->m_FilenamesMap.GetString(i);
                        if (!file.IsEmpty())
                        {
                            fileTokens += file + _T("\r\n");

                            TokenIdxSet result;
                            tokens->FindTokensInFile(file, result, tkUndefined);
                            for (TokenIdxSet::iterator it = result.begin(); it != result.end(); ++it)
                            {
                                Token* token = tokens->at(*it);
                                fileTokens << token->GetTokenKindString() << _T(" ");
                                if (token->m_TokenKind == tkFunction)
                                    fileTokens << token->m_Name << token->GetFormattedArgs() << _T("\t");
                                else
                                    fileTokens << token->DisplayName() << _T("\t");
                                fileTokens << _T("[") << token->m_Line << _T(",") << token->m_ImplLine << _T("]");
                                fileTokens << _T("\r\n");
                            }
                        }
                        fileTokens += _T("\r\n");
                    }
                }

                SaveCCDebugInfo(_("Save token list of files"), fileTokens);
            }
            break;
        default:
            cbMessageBox(_("Invalid selection."), _("CC Debug Info"));
    }
}
