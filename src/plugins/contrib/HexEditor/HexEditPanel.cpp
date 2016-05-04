/*
* This file is part of HexEditor plugin for Code::Blocks Studio
* Copyright (C) 2008-2009 Bartlomiej Swiecki
*
* HexEditor plugin is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* HexEditor pluging is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with HexEditor. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/
#include <sdk.h>

#ifndef CB_PRECOMP
    #include <wx/dcclient.h>
    #include <wx/filedlg.h>
    #include <wx/filename.h>
    #include <wx/sizer.h>
    #include <wx/choicdlg.h>

    #include <manager.h>
    #include <editormanager.h>
    #include <configmanager.h>
    #include <logmanager.h>
    #include <globals.h>
    #include "prep.h"
#endif // CB_PRECOMP

#include <wx/dcbuffer.h>
#include <wx/numdlg.h>

#include "HexEditPanel.h"
#include "ExpressionTester.h"
#include "SelectStoredExpressionDlg.h"
#include "ExpressionExecutor.h"
#include "ExpressionParser.h"
#include "CharacterView.h"
#include "DigitView.h"
#include "HexEditLineBuffer.h"

#include "ExpressionTestCases.h"
#include "FileContentDisk.h"
#include "TestCasesDlg.h"
#include "SearchDialog.h"

//(*InternalHeaders(HexEditPanel)
#include <wx/string.h>
#include <wx/intl.h>
//*)


namespace
{
    inline int NWD( int a, int b )
    {
        while ( b )
        {
            int c = a % b;
            a = b;
            b = c;
        }
        return a;
    }
}

//(*IdInit(HexEditPanel)
const long HexEditPanel::ID_STATICTEXT1 = wxNewId();
const long HexEditPanel::ID_BUTTON10 = wxNewId();
const long HexEditPanel::ID_BUTTON9 = wxNewId();
const long HexEditPanel::ID_STATICLINE2 = wxNewId();
const long HexEditPanel::ID_BUTTON7 = wxNewId();
const long HexEditPanel::ID_BUTTON4 = wxNewId();
const long HexEditPanel::ID_BUTTON6 = wxNewId();
const long HexEditPanel::ID_BUTTON5 = wxNewId();
const long HexEditPanel::ID_STATICLINE1 = wxNewId();
const long HexEditPanel::ID_BUTTON1 = wxNewId();
const long HexEditPanel::ID_BUTTON8 = wxNewId();
const long HexEditPanel::ID_CHECKBOX1 = wxNewId();
const long HexEditPanel::ID_PANEL1 = wxNewId();
const long HexEditPanel::ID_SCROLLBAR1 = wxNewId();
const long HexEditPanel::ID_STATICTEXT2 = wxNewId();
const long HexEditPanel::ID_STATICTEXT3 = wxNewId();
const long HexEditPanel::ID_STATICTEXT4 = wxNewId();
const long HexEditPanel::ID_STATICTEXT5 = wxNewId();
const long HexEditPanel::ID_STATICTEXT6 = wxNewId();
const long HexEditPanel::ID_STATICTEXT7 = wxNewId();
const long HexEditPanel::ID_STATICTEXT8 = wxNewId();
const long HexEditPanel::ID_STATICTEXT9 = wxNewId();
const long HexEditPanel::ID_STATICTEXT10 = wxNewId();
const long HexEditPanel::ID_STATICTEXT11 = wxNewId();
const long HexEditPanel::ID_STATICTEXT12 = wxNewId();
const long HexEditPanel::ID_STATICTEXT13 = wxNewId();
const long HexEditPanel::ID_STATICTEXT14 = wxNewId();
const long HexEditPanel::ID_TEXTCTRL1 = wxNewId();
const long HexEditPanel::ID_BUTTON3 = wxNewId();
const long HexEditPanel::ID_BUTTON2 = wxNewId();
const long HexEditPanel::ID_STATICTEXT15 = wxNewId();
const long HexEditPanel::ID_TIMER1 = wxNewId();
const long HexEditPanel::ID_MENUITEM2 = wxNewId();
const long HexEditPanel::ID_MENUITEM1 = wxNewId();
const long HexEditPanel::ID_MENUITEM3 = wxNewId();
const long HexEditPanel::ID_MENUITEM4 = wxNewId();
const long HexEditPanel::ID_MENUITEM5 = wxNewId();
const long HexEditPanel::ID_MENUITEM6 = wxNewId();
const long HexEditPanel::ID_MENUITEM7 = wxNewId();
const long HexEditPanel::ID_MENUITEM8 = wxNewId();
const long HexEditPanel::ID_MENUITEM9 = wxNewId();
const long HexEditPanel::ID_MENUITEM11 = wxNewId();
const long HexEditPanel::ID_MENUITEM12 = wxNewId();
const long HexEditPanel::ID_MENUITEM13 = wxNewId();
const long HexEditPanel::ID_MENUITEM14 = wxNewId();
const long HexEditPanel::ID_MENUITEM15 = wxNewId();
const long HexEditPanel::ID_MENUITEM16 = wxNewId();
const long HexEditPanel::ID_MENUITEM17 = wxNewId();
const long HexEditPanel::ID_MENUITEM18 = wxNewId();
const long HexEditPanel::ID_MENUITEM32 = wxNewId();
const long HexEditPanel::ID_MENUITEM10 = wxNewId();
const long HexEditPanel::ID_MENUITEM20 = wxNewId();
const long HexEditPanel::ID_MENUITEM21 = wxNewId();
const long HexEditPanel::ID_MENUITEM22 = wxNewId();
const long HexEditPanel::ID_MENUITEM23 = wxNewId();
const long HexEditPanel::ID_MENUITEM24 = wxNewId();
const long HexEditPanel::ID_MENUITEM25 = wxNewId();
const long HexEditPanel::ID_MENUITEM26 = wxNewId();
const long HexEditPanel::ID_MENUITEM27 = wxNewId();
const long HexEditPanel::ID_MENUITEM19 = wxNewId();
const long HexEditPanel::ID_MENUITEM29 = wxNewId();
const long HexEditPanel::ID_MENUITEM30 = wxNewId();
const long HexEditPanel::ID_MENUITEM31 = wxNewId();
const long HexEditPanel::ID_MENUITEM33 = wxNewId();
const long HexEditPanel::ID_MENUITEM28 = wxNewId();
//*)

BEGIN_EVENT_TABLE(HexEditPanel,EditorBase)
	//(*EventTable(HexEditPanel)
	//*)
END_EVENT_TABLE()

HexEditPanel::EditorsSet HexEditPanel::m_AllEditors;

HexEditPanel::HexEditPanel( const wxString& fileName, const wxString& title )
    : EditorBase( (wxWindow*)Manager::Get()->GetEditorManager()->GetNotebook(), fileName )
    , m_FileName( fileName )
    , m_Content( 0 )
    , m_DrawFont( 0 )
    , m_Current( 0 )
    , m_CurrentBlockStart( 0 )
    , m_CurrentBlockEnd( 0 )
    , m_MouseDown( false )
    , m_ColsMode( CM_ANY )
{
    /*
    --- Begin of comment which prevents calling Create() --

    //(*Initialize(HexEditPanel)
    Create(parent, wxID_ANY, wxDefaultPosition, wxSize(79,51), wxTAB_TRAVERSAL, _T("wxID_ANY"));

    --- End of comment which prevents calling Create() --
    */

    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    m_Status = new wxStaticText(this, ID_STATICTEXT1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    BoxSizer3->Add(m_Status, 1, wxALL|wxEXPAND, 5);
    Button6 = new wxButton(this, ID_BUTTON10, _("Goto"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON10"));
    BoxSizer3->Add(Button6, 0, wxALIGN_CENTER_VERTICAL, 5);
    Button5 = new wxButton(this, ID_BUTTON9, _("Search"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON9"));
    BoxSizer3->Add(Button5, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    StaticLine2 = new wxStaticLine(this, ID_STATICLINE2, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL, _T("ID_STATICLINE2"));
    BoxSizer3->Add(StaticLine2, 0, wxALL|wxEXPAND, 5);
    m_ColsModeBtn = new wxButton(this, ID_BUTTON7, _("Cols"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON7"));
    BoxSizer3->Add(m_ColsModeBtn, 0, wxALIGN_CENTER_VERTICAL, 5);
    m_DigitBits = new wxButton(this, ID_BUTTON4, _("Hex"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON4"));
    BoxSizer3->Add(m_DigitBits, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    m_BlockSize = new wxButton(this, ID_BUTTON6, _("1B"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON6"));
    BoxSizer3->Add(m_BlockSize, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    m_Endianess = new wxButton(this, ID_BUTTON5, _("BE"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON5"));
    BoxSizer3->Add(m_Endianess, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    StaticLine1 = new wxStaticLine(this, ID_STATICLINE1, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL, _T("ID_STATICLINE1"));
    BoxSizer3->Add(StaticLine1, 0, wxALL|wxEXPAND, 5);
    Button1 = new wxButton(this, ID_BUTTON1, _("Calc"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON1"));
    BoxSizer3->Add(Button1, 0, wxALIGN_CENTER_VERTICAL, 5);
    Button4 = new wxButton(this, ID_BUTTON8, _("Test"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON8"));
    BoxSizer3->Add(Button4, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
    CheckBox1 = new wxCheckBox(this, ID_CHECKBOX1, _("Value preview"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
    CheckBox1->SetValue(true);
    BoxSizer3->Add(CheckBox1, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(BoxSizer3, 0, wxEXPAND, 5);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    m_DrawArea = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxFULL_REPAINT_ON_RESIZE, _T("ID_PANEL1"));
    BoxSizer2->Add(m_DrawArea, 1, wxEXPAND, 5);
    m_ContentScroll = new wxScrollBar(this, ID_SCROLLBAR1, wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL, wxDefaultValidator, _T("ID_SCROLLBAR1"));
    m_ContentScroll->SetScrollbar(0, 1, 1, 1);
    BoxSizer2->Add(m_ContentScroll, 0, wxEXPAND, 0);
    BoxSizer1->Add(BoxSizer2, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 5);
    PreviewSizer = new wxBoxSizer(wxVERTICAL);
    FlexGridSizer1 = new wxFlexGridSizer(0, 8, 5, 5);
    FlexGridSizer1->AddGrowableCol(1);
    FlexGridSizer1->AddGrowableCol(4);
    FlexGridSizer1->AddGrowableCol(7);
    StaticText1 = new wxStaticText(this, ID_STATICTEXT2, _("Byte:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    FlexGridSizer1->Add(StaticText1, 1, wxEXPAND, 5);
    m_ByteVal = new wxStaticText(this, ID_STATICTEXT3, _("-9999999999"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    FlexGridSizer1->Add(m_ByteVal, 1, wxEXPAND, 5);
    FlexGridSizer1->Add(5,5,1, wxALIGN_CENTER_VERTICAL, 5);
    StaticText3 = new wxStaticText(this, ID_STATICTEXT4, _("Word:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
    FlexGridSizer1->Add(StaticText3, 1, wxEXPAND, 5);
    m_WordVal = new wxStaticText(this, ID_STATICTEXT5, _("-9999999999"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
    FlexGridSizer1->Add(m_WordVal, 1, wxEXPAND, 5);
    FlexGridSizer1->Add(5,5,1, wxALIGN_CENTER_VERTICAL, 5);
    StaticText2 = new wxStaticText(this, ID_STATICTEXT6, _("Dword:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
    FlexGridSizer1->Add(StaticText2, 1, wxEXPAND, 5);
    m_DwordVal = new wxStaticText(this, ID_STATICTEXT7, _("-9999999999"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
    FlexGridSizer1->Add(m_DwordVal, 1, wxEXPAND, 5);
    StaticText4 = new wxStaticText(this, ID_STATICTEXT8, _("Float:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
    FlexGridSizer1->Add(StaticText4, 1, wxEXPAND, 5);
    m_FloatVal = new wxStaticText(this, ID_STATICTEXT9, _("-9999999999"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT9"));
    FlexGridSizer1->Add(m_FloatVal, 1, wxEXPAND, 5);
    FlexGridSizer1->Add(5,5,1, wxALIGN_CENTER_VERTICAL, 5);
    StaticText6 = new wxStaticText(this, ID_STATICTEXT10, _("Double:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT10"));
    FlexGridSizer1->Add(StaticText6, 1, wxEXPAND, 5);
    m_DoubleVal = new wxStaticText(this, ID_STATICTEXT11, _("-9999999999"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT11"));
    FlexGridSizer1->Add(m_DoubleVal, 1, wxEXPAND, 5);
    FlexGridSizer1->Add(5,5,1, wxALIGN_CENTER_VERTICAL, 5);
    StaticText8 = new wxStaticText(this, ID_STATICTEXT12, _("L-Double:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT12"));
    FlexGridSizer1->Add(StaticText8, 1, wxEXPAND, 5);
    m_LDoubleVal = new wxStaticText(this, ID_STATICTEXT13, _("-9999999999"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT13"));
    FlexGridSizer1->Add(m_LDoubleVal, 1, wxEXPAND, 5);
    PreviewSizer->Add(FlexGridSizer1, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 5);
    BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    StaticText5 = new wxStaticText(this, ID_STATICTEXT14, _("Expression:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT14"));
    BoxSizer4->Add(StaticText5, 0, wxALIGN_CENTER_VERTICAL, 5);
    m_Expression = new wxTextCtrl(this, ID_TEXTCTRL1, _("byte[ @ ]"), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    BoxSizer4->Add(m_Expression, 1, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    Button3 = new wxButton(this, ID_BUTTON3, _("v"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON3"));
    BoxSizer4->Add(Button3, 0, wxRIGHT|wxEXPAND, 5);
    Button2 = new wxButton(this, ID_BUTTON2, _("\?"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON2"));
    BoxSizer4->Add(Button2, 0, wxRIGHT|wxEXPAND, 5);
    m_ExpressionVal = new wxStaticText(this, ID_STATICTEXT15, _("-9999999999"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT15"));
    BoxSizer4->Add(m_ExpressionVal, 1, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    PreviewSizer->Add(BoxSizer4, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 5);
    BoxSizer1->Add(PreviewSizer, 0, wxEXPAND, 5);
    SetSizer(BoxSizer1);
    ReparseTimer.SetOwner(this, ID_TIMER1);
    MenuItem2 = new wxMenuItem((&m_BaseMenu), ID_MENUITEM2, _("Bin"), wxEmptyString, wxITEM_NORMAL);
    m_BaseMenu.Append(MenuItem2);
    MenuItem1 = new wxMenuItem((&m_BaseMenu), ID_MENUITEM1, _("Hex"), wxEmptyString, wxITEM_NORMAL);
    m_BaseMenu.Append(MenuItem1);
    MenuItem3 = new wxMenuItem((&m_EndianessMenu), ID_MENUITEM3, _("Big Endian"), wxEmptyString, wxITEM_NORMAL);
    m_EndianessMenu.Append(MenuItem3);
    MenuItem4 = new wxMenuItem((&m_EndianessMenu), ID_MENUITEM4, _("Little Endian"), wxEmptyString, wxITEM_NORMAL);
    m_EndianessMenu.Append(MenuItem4);
    MenuItem5 = new wxMenuItem((&m_BlockSizeMenu), ID_MENUITEM5, _("1 Byte"), wxEmptyString, wxITEM_NORMAL);
    m_BlockSizeMenu.Append(MenuItem5);
    MenuItem6 = new wxMenuItem((&m_BlockSizeMenu), ID_MENUITEM6, _("2 Bytes"), wxEmptyString, wxITEM_NORMAL);
    m_BlockSizeMenu.Append(MenuItem6);
    MenuItem7 = new wxMenuItem((&m_BlockSizeMenu), ID_MENUITEM7, _("4 Bytes"), wxEmptyString, wxITEM_NORMAL);
    m_BlockSizeMenu.Append(MenuItem7);
    MenuItem8 = new wxMenuItem((&m_BlockSizeMenu), ID_MENUITEM8, _("8 Bytes"), wxEmptyString, wxITEM_NORMAL);
    m_BlockSizeMenu.Append(MenuItem8);
    MenuItem9 = new wxMenuItem((&m_ColsModeMenu), ID_MENUITEM9, _("Any"), wxEmptyString, wxITEM_NORMAL);
    m_ColsModeMenu.Append(MenuItem9);
    MenuItem10 = new wxMenu();
    MenuItem11 = new wxMenuItem(MenuItem10, ID_MENUITEM11, _("1"), wxEmptyString, wxITEM_NORMAL);
    MenuItem10->Append(MenuItem11);
    MenuItem12 = new wxMenuItem(MenuItem10, ID_MENUITEM12, _("2"), wxEmptyString, wxITEM_NORMAL);
    MenuItem10->Append(MenuItem12);
    MenuItem13 = new wxMenuItem(MenuItem10, ID_MENUITEM13, _("3"), wxEmptyString, wxITEM_NORMAL);
    MenuItem10->Append(MenuItem13);
    MenuItem14 = new wxMenuItem(MenuItem10, ID_MENUITEM14, _("4"), wxEmptyString, wxITEM_NORMAL);
    MenuItem10->Append(MenuItem14);
    MenuItem15 = new wxMenuItem(MenuItem10, ID_MENUITEM15, _("5"), wxEmptyString, wxITEM_NORMAL);
    MenuItem10->Append(MenuItem15);
    MenuItem16 = new wxMenuItem(MenuItem10, ID_MENUITEM16, _("6"), wxEmptyString, wxITEM_NORMAL);
    MenuItem10->Append(MenuItem16);
    MenuItem17 = new wxMenuItem(MenuItem10, ID_MENUITEM17, _("7"), wxEmptyString, wxITEM_NORMAL);
    MenuItem10->Append(MenuItem17);
    MenuItem18 = new wxMenuItem(MenuItem10, ID_MENUITEM18, _("8"), wxEmptyString, wxITEM_NORMAL);
    MenuItem10->Append(MenuItem18);
    MenuItem10->AppendSeparator();
    MenuItem32 = new wxMenuItem(MenuItem10, ID_MENUITEM32, _("Other"), wxEmptyString, wxITEM_NORMAL);
    MenuItem10->Append(MenuItem32);
    m_ColsModeMenu.Append(ID_MENUITEM10, _("Exactly"), MenuItem10, wxEmptyString);
    MenuItem19 = new wxMenu();
    MenuItem20 = new wxMenuItem(MenuItem19, ID_MENUITEM20, _("2"), wxEmptyString, wxITEM_NORMAL);
    MenuItem19->Append(MenuItem20);
    MenuItem21 = new wxMenuItem(MenuItem19, ID_MENUITEM21, _("3"), wxEmptyString, wxITEM_NORMAL);
    MenuItem19->Append(MenuItem21);
    MenuItem22 = new wxMenuItem(MenuItem19, ID_MENUITEM22, _("4"), wxEmptyString, wxITEM_NORMAL);
    MenuItem19->Append(MenuItem22);
    MenuItem23 = new wxMenuItem(MenuItem19, ID_MENUITEM23, _("5"), wxEmptyString, wxITEM_NORMAL);
    MenuItem19->Append(MenuItem23);
    MenuItem24 = new wxMenuItem(MenuItem19, ID_MENUITEM24, _("6"), wxEmptyString, wxITEM_NORMAL);
    MenuItem19->Append(MenuItem24);
    MenuItem25 = new wxMenuItem(MenuItem19, ID_MENUITEM25, _("7"), wxEmptyString, wxITEM_NORMAL);
    MenuItem19->Append(MenuItem25);
    MenuItem26 = new wxMenuItem(MenuItem19, ID_MENUITEM26, _("8"), wxEmptyString, wxITEM_NORMAL);
    MenuItem19->Append(MenuItem26);
    MenuItem19->AppendSeparator();
    MenuItem27 = new wxMenuItem(MenuItem19, ID_MENUITEM27, _("Other"), wxEmptyString, wxITEM_NORMAL);
    MenuItem19->Append(MenuItem27);
    m_ColsModeMenu.Append(ID_MENUITEM19, _("Multiple of"), MenuItem19, wxEmptyString);
    MenuItem28 = new wxMenu();
    MenuItem29 = new wxMenuItem(MenuItem28, ID_MENUITEM29, _("2"), wxEmptyString, wxITEM_NORMAL);
    MenuItem28->Append(MenuItem29);
    MenuItem30 = new wxMenuItem(MenuItem28, ID_MENUITEM30, _("4"), wxEmptyString, wxITEM_NORMAL);
    MenuItem28->Append(MenuItem30);
    MenuItem31 = new wxMenuItem(MenuItem28, ID_MENUITEM31, _("8"), wxEmptyString, wxITEM_NORMAL);
    MenuItem28->Append(MenuItem31);
    MenuItem28->AppendSeparator();
    MenuItem33 = new wxMenuItem(MenuItem28, ID_MENUITEM33, _("Other"), wxEmptyString, wxITEM_NORMAL);
    MenuItem28->Append(MenuItem33);
    m_ColsModeMenu.Append(ID_MENUITEM28, _("Power of"), MenuItem28, wxEmptyString);
    SetSizer(BoxSizer1);
    Layout();

    Connect(ID_BUTTON10,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&HexEditPanel::OnButton6Click);
    Connect(ID_BUTTON9,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&HexEditPanel::OnButton5Click);
    Connect(ID_BUTTON7,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&HexEditPanel::Onm_ColsModeClick);
    Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&HexEditPanel::OnButton4Click);
    Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&HexEditPanel::Onm_BlockSizeClick);
    Connect(ID_BUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&HexEditPanel::Onm_EndianessClick);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&HexEditPanel::OnButton1Click);
    Connect(ID_BUTTON8,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&HexEditPanel::OnButton4Click1);
    Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&HexEditPanel::OnCheckBox1Click);
    m_DrawArea->Connect(wxEVT_PAINT,(wxObjectEventFunction)&HexEditPanel::OnContentPaint,0,this);
    m_DrawArea->Connect(wxEVT_ERASE_BACKGROUND,(wxObjectEventFunction)&HexEditPanel::OnDrawAreaEraseBackground,0,this);
    m_DrawArea->Connect(wxEVT_KEY_DOWN,(wxObjectEventFunction)&HexEditPanel::OnSpecialKeyDown,0,this);
    m_DrawArea->Connect(wxEVT_CHAR,(wxObjectEventFunction)&HexEditPanel::OnDrawAreaKeyDown,0,this);
    m_DrawArea->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&HexEditPanel::OnDrawAreaLeftDown,0,this);
    m_DrawArea->Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&HexEditPanel::OnDrawAreaLeftUp,0,this);
    m_DrawArea->Connect(wxEVT_MOTION,(wxObjectEventFunction)&HexEditPanel::OnDrawAreaMouseMove,0,this);
    m_DrawArea->Connect(wxEVT_LEAVE_WINDOW,(wxObjectEventFunction)&HexEditPanel::OnDrawAreaLeftUp,0,this);
    m_DrawArea->Connect(wxEVT_MOUSEWHEEL,(wxObjectEventFunction)&HexEditPanel::OnContentMouseWheel,0,this);
    Connect(ID_SCROLLBAR1,wxEVT_SCROLL_TOP,(wxObjectEventFunction)&HexEditPanel::OnContentScroll);
    Connect(ID_SCROLLBAR1,wxEVT_SCROLL_BOTTOM,(wxObjectEventFunction)&HexEditPanel::OnContentScroll);
    Connect(ID_SCROLLBAR1,wxEVT_SCROLL_LINEUP,(wxObjectEventFunction)&HexEditPanel::OnContentScroll);
    Connect(ID_SCROLLBAR1,wxEVT_SCROLL_LINEDOWN,(wxObjectEventFunction)&HexEditPanel::OnContentScroll);
    Connect(ID_SCROLLBAR1,wxEVT_SCROLL_PAGEUP,(wxObjectEventFunction)&HexEditPanel::OnContentScroll);
    Connect(ID_SCROLLBAR1,wxEVT_SCROLL_PAGEDOWN,(wxObjectEventFunction)&HexEditPanel::OnContentScroll);
    Connect(ID_SCROLLBAR1,wxEVT_SCROLL_THUMBTRACK,(wxObjectEventFunction)&HexEditPanel::OnContentScroll);
    Connect(ID_SCROLLBAR1,wxEVT_SCROLL_THUMBRELEASE,(wxObjectEventFunction)&HexEditPanel::OnContentScroll);
    Connect(ID_SCROLLBAR1,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&HexEditPanel::OnContentScroll);
    Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&HexEditPanel::Onm_ExpressionText);
    Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&HexEditPanel::OnExpressionTextEnter);
    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&HexEditPanel::OnButton3Click1);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&HexEditPanel::OnButton2Click);
    Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&HexEditPanel::OnReparseTimerTrigger);
    Connect(ID_MENUITEM2,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetBaseBin);
    Connect(ID_MENUITEM1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetBaseHex);
    Connect(ID_MENUITEM3,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetEndianessBig);
    Connect(ID_MENUITEM4,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetEndianessLittle);
    Connect(ID_MENUITEM5,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetBlockSize1);
    Connect(ID_MENUITEM6,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetBlockSize2);
    Connect(ID_MENUITEM7,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetBlockSize4);
    Connect(ID_MENUITEM8,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetBlockSize8);
    Connect(ID_MENUITEM9,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetColsMul1);
    Connect(ID_MENUITEM11,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetColsValue1);
    Connect(ID_MENUITEM12,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetColsValue2);
    Connect(ID_MENUITEM13,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetColsValue3);
    Connect(ID_MENUITEM14,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetColsValue4);
    Connect(ID_MENUITEM15,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetColsValue5);
    Connect(ID_MENUITEM16,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetColsValue6);
    Connect(ID_MENUITEM17,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetColsValue7);
    Connect(ID_MENUITEM18,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetColsValue8);
    Connect(ID_MENUITEM32,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetColsValueOther);
    Connect(ID_MENUITEM20,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetColsMul2);
    Connect(ID_MENUITEM21,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetColsMul3);
    Connect(ID_MENUITEM22,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetColsMul4);
    Connect(ID_MENUITEM23,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetColsMul5);
    Connect(ID_MENUITEM24,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetColsMul6);
    Connect(ID_MENUITEM25,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetColsMul7);
    Connect(ID_MENUITEM26,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetColsMul8);
    Connect(ID_MENUITEM27,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetColsMulOther);
    Connect(ID_MENUITEM29,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetColsPower2);
    Connect(ID_MENUITEM30,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetColsPower4);
    Connect(ID_MENUITEM31,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetColsPower8);
    Connect(ID_MENUITEM33,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HexEditPanel::OnSetColsPowerOther);
    //*)

    // We connect these events manually
    m_DrawArea->Connect( wxEVT_SIZE, (wxObjectEventFunction)&HexEditPanel::OnContentSize,     0, this );
    m_DrawArea->SetBackgroundStyle( wxBG_STYLE_CUSTOM );
    Connect( wxEVT_SET_FOCUS, (wxObjectEventFunction)&HexEditPanel::OnForwardFocus );

    m_ActiveView = 0;
    m_LastScrollPos = 0;
    m_LastScrollUnits = 0;
    m_LinesPerScrollUnit = 1;
    CreateViews();
    m_NeedRecalc = true;

    ReparseExpression();
    SetFontSize( 8 );
    ReadContent();

    if ( m_Content && m_Content->GetSize() > 0x40000000ULL )
    {
        // Because of the filesize we have to map scroll units
        // to some number of lines
        m_LinesPerScrollUnit = m_Content->GetSize() / 0x20000000ULL;
    }

    RefreshStatus();

    m_Current = 0;
    PropagateOffsetChange();

    m_DrawArea->SetFocus();

    ((wxWindow*)Manager::Get()->GetEditorManager()->GetNotebook())->Layout();

    m_Shortname = title;
    SetTitle( m_Shortname );

    m_AllEditors.insert( this );

}

void HexEditPanel::CreateViews()
{
    for ( int i=0; i<MAX_VIEWS; ++i )
    {
        m_Views[ i ] = 0;
    }

    m_Views[ VIEW_DIGIT ] = m_DigitView = new DigitView( this );
    m_Views[ VIEW_CHARS ] = new CharacterView( this );
    ActivateView( m_Views[ 0 ] );
}

void HexEditPanel::ActivateView( HexEditViewBase* view )
{
    if ( view == m_ActiveView ) return;

    if ( m_ActiveView )
    {
        m_ActiveView->SetActive( false );
    }

    m_ActiveView = view;
    view->SetActive( true );
}


HexEditPanel::~HexEditPanel()
{
    for ( int i=0; i<MAX_VIEWS; ++i )
    {
        delete m_Views[ i ];
        m_Views[ i ] = 0;
    }

    m_AllEditors.erase( this );

    delete m_DrawFont;
    m_DrawFont = 0;

    delete m_Content;
    m_Content = 0;

	//(*Destroy(HexEditPanel)
	//*)
}

void HexEditPanel::SetFontSize( int size )
{
    delete m_DrawFont;
    m_DrawFont = wxFont::New( size, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString, wxFONTENCODING_DEFAULT );
}

void HexEditPanel::ReadContent()
{
    if ( m_Content ) delete m_Content;

    m_Content = FileContentBase::BuildInstance( m_FileName );
    if ( !m_Content )
    {
        m_ErrorString = _("Could not open the file (note that HexEditor may not be able to open big files)");
        return;
    }

    if ( !m_Content->ReadFile( m_FileName ) )
    {
        delete m_Content;
        m_Content = 0;
        m_ErrorString = _("Could not read the file");
        return;
    }
}

void HexEditPanel::RecalculateCoefs(wxDC &dc)
{
    if (!m_NeedRecalc)
        return;
    m_NeedRecalc = false;
    dc.GetTextExtent( _T("0123456789ABCDEF"), &m_FontX, &m_FontY, NULL, NULL, m_DrawFont );
    m_FontX /= 16;

    // Calculate number of rows and columns in characters
    wxSize size = m_DrawArea->GetClientSize();
    m_Cols      = size.GetWidth() / m_FontX;
    m_Lines     = size.GetHeight() / m_FontY;

    // Calculate number of bytes in one line
    double avgByteCharacters = 0.0;
    int cumulativeBlockSize = 1;
    for ( int i=0; i<MAX_VIEWS; ++i )
    {
        int blockLength;
        int blockBytes;
        int spacing;
        m_Views[ i ]->GetBlockSizes( blockLength, blockBytes, spacing );

        avgByteCharacters  += ( (double)( blockLength + spacing ) ) / (double)blockBytes;
        cumulativeBlockSize = cumulativeBlockSize * blockBytes / NWD( cumulativeBlockSize, blockBytes );
    }

    int colsLeftForViews =
        m_Cols
          - 8               // Offset
          - 1               // ':' after offset
          - 2 * MAX_VIEWS   // spacing between views
          - 2;              // right margin

    int maxByteInLine = (int)( colsLeftForViews / avgByteCharacters );

    // Now we need to find such number of bytes to be multiple of cumulativeBlockSize
    // and try not to cross maxByteInLine

    // Additionally we try to match required columns count,
    // this is a little bit naive approach but will work in generic way
    int maxColumns = std::max( (int)(maxByteInLine / cumulativeBlockSize), 1 );
    for ( int i=maxColumns;; i-- )
    {
        if ( i<1 )
        {
            for ( i=maxColumns+1; i<0x1000; ++i )
            {
                if ( MatchColumnsCount( i ) )
                {
                    maxColumns = i;
                    break;
                }
            }
            break;
        }

        if ( MatchColumnsCount( i ) )
        {
            maxColumns = i;
            break;
        }
    }

    m_ColsCount = maxColumns;
    m_LineBytes = maxColumns * cumulativeBlockSize;

    // Calculate column positions
    for ( int i=0; i<MAX_VIEWS; ++i )
    {
        int blockLength;
        int blockBytes;
        int spacing;
        m_Views[ i ]->GetBlockSizes( blockLength, blockBytes, spacing );
        m_ViewsCols[ i ] = ( ( m_LineBytes + blockBytes - 1 ) / blockBytes ) * ( blockLength + spacing );
    }

    // Adjust scroll bar
    OffsetT contentSize = m_Content ? m_Content->GetSize() : 0;

    int totalLines = ( contentSize + m_LineBytes          - 1 ) / m_LineBytes;
    int totalUnits = ( totalLines  + m_LinesPerScrollUnit - 1 ) / m_LinesPerScrollUnit;
    int thumbLines = ( m_Lines     + m_LinesPerScrollUnit - 1 ) / m_LinesPerScrollUnit;

    m_ContentScroll->SetScrollbar(
        m_ContentScroll->GetThumbPosition(),
        thumbLines,
        totalUnits,
        thumbLines );
}

void HexEditPanel::OnContentPaint( wxPaintEvent& /*event*/ )
{
    wxAutoBufferedPaintDC dc( m_DrawArea );
    RecalculateCoefs( dc );
    dc.SetBrush( GetBackgroundColour() );
    dc.SetPen  ( GetBackgroundColour() );
    dc.DrawRectangle( GetClientRect() );

    if ( !m_Content ) return;

    dc.SetFont( *m_DrawFont );

    OffsetT startOffs = DetectStartOffset();

    HexEditLineBuffer buff( m_Cols );
    char* content = new char[ m_Cols ];

    wxColour backgrounds[ stCount ] =
    {
        GetBackgroundColour(),
        wxColour( 0x70, 0x70, 0x70 ),
        wxColour( 0xA0, 0xA0, 0xFF ),
        wxColour( 0x80, 0x80, 0xFF ),
    };

    wxColour foregrounds[ stCount ] =
    {
        *wxBLACK,
        *wxWHITE,
        *wxWHITE,
        *wxBLACK,
    };


    for ( OffsetT j = 0; j < m_Lines; ++j )
    {
        buff.Reset();

        // Calculate offsets
        OffsetT offs    = startOffs + j * m_LineBytes;
        OffsetT offsMax = offs + m_LineBytes;

        // Add offset to view buffer
        for ( size_t i=8; i-->0; )
        {
            buff.PutChar( "0123456789ABCDEF"[ ( offs >> ( i << 2 ) ) & 0xF ] );
        }
        buff.PutChar(':');

        // Clamp offsets to content size
        offs    = wxMin( offs,    m_Content->GetSize() );
        offsMax = wxMin( offsMax, m_Content->GetSize() );

        if ( offs == offsMax ) continue;

        // Invoking views
        m_Content->Read( content, offs, offsMax - offs );
        for ( int i=0; i<MAX_VIEWS; ++i )
        {
            buff.PutString("  ");
            m_Views[ i ]->PutLine( offs, buff, content, offsMax - offs );
        }

        buff.Draw( dc, 0, j * m_FontY, m_FontX, m_FontY, foregrounds, backgrounds );
    }

    delete[] content;
}

void HexEditPanel::OnContentScroll( wxScrollEvent& /*event*/ )
{
    if ( !m_Content || !m_Content->GetSize() )
    {
        return;
    }

    if ( m_ContentScroll->GetThumbPosition() == 0 )
    {
        DetectStartOffset();
        m_LastScrollPos   = 0;
    }
    else if ( m_ContentScroll->GetThumbPosition() >= m_ContentScroll->GetRange() - m_ContentScroll->GetThumbSize() )
    {
        DetectStartOffset();
        int totalLines = m_Content->GetSize() / m_LineBytes;
        m_LastScrollPos = totalLines - m_Lines + 1;
    }

    ClampCursorToVisibleArea();
    m_DrawArea->Refresh();
    RefreshStatus();
    m_DrawArea->SetFocus();
}

void HexEditPanel::OnContentScrollTop(wxScrollEvent& event)
{
    if ( !m_Content || !m_Content->GetSize() )
    {
        return;
    }

    m_LastScrollPos   = 0;

    LogManager::Get()->DebugLog( _T("Top") );

    OnContentScroll( event );
}

void HexEditPanel::OnContentScrollBottom(wxScrollEvent& event)
{
    if ( !m_Content || !m_Content->GetSize() )
    {
        return;
    }

    int totalLines = m_Content->GetSize() / m_LineBytes;
    m_LastScrollPos = totalLines - m_Lines + 1;

    LogManager::Get()->DebugLog( _T("Top") );

    OnContentScroll( event );
}

void HexEditPanel::OnContentSize( wxSizeEvent& event )
{
    m_NeedRecalc = true;
    EnsureCarretVisible();
    RefreshStatus();
    event.Skip();
}

FileContentBase::OffsetT HexEditPanel::DetectStartOffset()
{
    if ( !m_Content ) return 0;

    int currentUnits = m_ContentScroll->GetThumbPosition();

    if ( currentUnits < m_LastScrollUnits )
    {
        FileContentBase::OffsetT diff = ( m_LastScrollUnits - currentUnits ) * m_LinesPerScrollUnit;
        if ( m_LastScrollPos < diff )
        {
            m_LastScrollPos = 0;
        }
        else
        {
            m_LastScrollPos -= diff;
        }
    }
    else if ( currentUnits > m_LastScrollUnits )
    {
        m_LastScrollPos += ( currentUnits - m_LastScrollUnits ) * m_LinesPerScrollUnit;
        FileContentBase::OffsetT maxScrollPos = ( m_Content->GetSize() + m_LineBytes - 1 ) / m_LineBytes;
        if ( m_LastScrollPos >= maxScrollPos )
        {
            m_LastScrollPos = maxScrollPos-1;
        }
    }

    m_LastScrollUnits = currentUnits;

    return m_LastScrollPos * m_LineBytes;
}

void HexEditPanel::OnContentMouseWheel(wxMouseEvent& event)
{
    m_ContentScroll->SetThumbPosition(
        m_ContentScroll->GetThumbPosition() -
        2 * event.GetWheelRotation() / event.GetWheelDelta() );
    ClampCursorToVisibleArea();
    m_DrawArea->Refresh();
    RefreshStatus();
}

void HexEditPanel::RefreshStatus()
{
    if ( !m_Content )
    {
        m_Status->SetLabel( m_ErrorString );
        return;
    }

    FileContentBase::OffsetT size = m_Content->GetSize();

    if ( !size )
    {
        m_Status->SetLabel( _("File is empty") );
    }
    else
    {
        unsigned long long current_pos = m_Current + 1; // OffsetT = unsigned long long
        m_Status->SetLabel( F( _("Position: %llX / %llX ( %lld%% )"), current_pos, size, ( current_pos * 100 / (size) ) ) );
    }

    FileContentBase::OffsetT left = size - m_Current;

    if ( left >= sizeof( char ) )
    {
        char val;
        m_Content->Read( &val, m_Current, sizeof(val) );
        m_ByteVal->SetLabel( wxString::Format(_T("%d"),(int)val) );
    }
    else
    {
        m_ByteVal->SetLabel( _T("-") );
    }

    if ( left >= sizeof( short ) )
    {
        short val;
        m_Content->Read( &val, m_Current, sizeof(val) );
        m_WordVal->SetLabel( wxString::Format(_T("%d"),(int)val) );
    }
    else
    {
        m_WordVal->SetLabel( _T("-") );
    }

    if ( left >= sizeof( int ) )
    {
        int val;
        m_Content->Read( &val, m_Current, sizeof(val) );
        m_DwordVal->SetLabel( wxString::Format(_T("%d"),(int)val) );
    }
    else
    {
        m_DwordVal->SetLabel( _T("-") );
    }

    if ( left >= sizeof( float ) )
    {
        float val;
        m_Content->Read( &val, m_Current, sizeof(val) );
        m_FloatVal->SetLabel( wxString::Format(_T("%g"), val) );
    }
    else
    {
        m_FloatVal->SetLabel( _T("-") );
    }

    if ( left >= sizeof( double ) )
    {
        double val;
        m_Content->Read( &val, m_Current, sizeof(val) );
        m_DoubleVal->SetLabel( wxString::Format(_T("%g"), val) );
    }
    else
    {
        m_DoubleVal->SetLabel( _T("-") );
    }

    if ( left >= sizeof( long double ) )
    {
        long double val;
        m_Content->Read( &val, m_Current, sizeof(val) );
        if ( platform::windows )
        {
            // MinGW has broken support for long double
            m_LDoubleVal->SetLabel( wxString::Format(_T("%g"), (double)val) );
        }
        else
        {
            m_LDoubleVal->SetLabel( wxString::Format(_T("%Lg"), val) );
        }
    }
    else
    {
        m_LDoubleVal->SetLabel( _T("-") );
    }

    if ( m_ExpressionError.IsEmpty() )
    {

        Expression::Executor executor;

        wxStopWatch sw;
        if ( !executor.Execute( m_ExpressionCode, m_Content, m_Current ) )
        {
            m_ExpressionVal->SetLabel( executor.ErrorDesc() );
        }
        else
        {
//            LogManager::Get()->DebugLog( F( _T("HEExpr Calculate: %d"), (int)sw.Time() ) );
            unsigned long long uintLoc;
            long long          sint;
            long double        flt;

            if ( executor.GetResult( uintLoc ) )
            {
                m_ExpressionVal->SetLabel( wxString::Format( _T("%llu"), uintLoc) );
            }
            else if ( executor.GetResult( sint ) )
            {
                m_ExpressionVal->SetLabel( wxString::Format( _T("%lld"), sint ) );
            }
            else if ( executor.GetResult( flt ) )
            {
                m_ExpressionVal->SetLabel( wxString::Format( _T("%g"), (double)flt ) );
            }
            else
            {
                m_ExpressionVal->SetLabel( _T("Error") );
            }
        }
    }
    else
    {
        m_ExpressionVal->SetLabel( m_ExpressionError );
    }


    if ( m_DigitView )
    {
        switch ( m_DigitView->GetDigitBits() )
        {
            case 1: m_DigitBits->SetLabel( _("Bin") ); break;
            case 4: m_DigitBits->SetLabel( _("Hex") ); break;
            default: m_DigitBits->SetLabel( wxString::Format( _("%d bits") , m_DigitView->GetDigitBits() ) );
        }

        if ( m_DigitView->GetLittleEndian() )
            m_Endianess->SetLabel( _("LE") );
        else
            m_Endianess->SetLabel( _("BE") );

        m_BlockSize->SetLabel( wxString::Format( _("%dB"), m_DigitView->GetBlockBytes() ) );

        switch ( m_ColsMode )
        {
            case CM_SPECIFIED: m_ColsModeBtn->SetLabel( wxString::Format( _("Cols: %d"), m_ColsValue ) ); break;
            case CM_MULT:      m_ColsModeBtn->SetLabel( wxString::Format( _("Cols: %d ( n * %d )"), m_ColsCount, m_ColsValue ) ); break;
            case CM_POWER:     m_ColsModeBtn->SetLabel( wxString::Format( _("Cols: %d ( n ^ %d )"), m_ColsCount, m_ColsValue ) ); break;
            default:           m_ColsModeBtn->SetLabel( wxString::Format( _("Cols: %d ( Any )"), m_ColsCount ) );
        }
    }
}

void HexEditPanel::OnDrawAreaKeyDown(wxKeyEvent& event)
{
    if ( !m_Content || !m_Content->GetSize() ) return;

    m_ViewNotifyContentChange = false;
    m_ViewNotifyOffsetChange  = false;

    switch ( event.GetKeyCode() )
    {
        case WXK_LEFT:
        {
            m_ActiveView->MoveLeft();
            break;
        }

        case WXK_RIGHT:
        {
            m_ActiveView->MoveRight();
            break;
        }

        case WXK_UP:
        {
            m_ActiveView->MoveUp();
            break;
        }

        case WXK_DOWN:
        {
            m_ActiveView->MoveDown();
            break;
        }

        case WXK_PAGEDOWN:
        {
            for ( unsigned int i=0; i<m_Lines/2; i++ )
            {
                m_ActiveView->MoveDown();
            }
            break;
        }

        case WXK_PAGEUP:
        {
            for ( unsigned int i=0; i<m_Lines/2; i++ )
            {
                m_ActiveView->MoveUp();
            }
            break;
        }

        case WXK_TAB:
        {
            m_ActiveView->SetActive( false );
            int newViewId = -1;
            for ( int i=0; i<MAX_VIEWS; ++i )
            {
                if ( m_ActiveView == m_Views[ i ] )
                {
                    newViewId = ( i + 1 ) % MAX_VIEWS;
                    break;
                }
            }
            if ( newViewId < 0 ) newViewId = 0;
            m_ActiveView = m_Views[ newViewId ];
            m_ActiveView->SetActive( true );
            m_ViewNotifyContentChange = true;
            break;
        }

        case WXK_HOME:
        {
            m_Current = 0;
            m_ViewNotifyOffsetChange = true;
            break;
        }

        case WXK_END:
        {

            m_Current = m_Content->GetSize()-1;
            m_ViewNotifyOffsetChange = true;
            break;
        }

        case WXK_INSERT:
        {
            // Insert empty byte at current position
            FileContentBase::ExtraUndoData data(
                m_ActiveView,
                m_Current, m_ActiveView->GetCurrentPositionFlags(),
                m_Current, m_ActiveView->GetCurrentPositionFlags() );
            m_Content->Add( data, m_Current, 1, 0 );
            m_ViewNotifyContentChange = true;
            break;
        }

        case WXK_DELETE:
        {
            FileContentBase::ExtraUndoData data(
                m_ActiveView,
                m_Current, m_ActiveView->GetCurrentPositionFlags(),
                m_Current, m_ActiveView->GetCurrentPositionFlags() );
            m_Content->Remove( data, m_Current, 1 );
            m_ViewNotifyContentChange = true;
            break;
        }

        default:
        {
            m_ActiveView->PutChar( event.GetUnicodeKey() );
            break;
        }
    }

    if ( m_ViewNotifyOffsetChange )
    {
        EnsureCarretVisible();
        PropagateOffsetChange();
        RefreshStatus();
        m_ViewNotifyContentChange = true;
    }

    if ( m_ViewNotifyContentChange )
    {
        m_DrawArea->Refresh();
        UpdateModified();
    }
}

void HexEditPanel::EnsureCarretVisible()
{
    FileContentBase::OffsetT line      = m_Current / m_LineBytes;
    FileContentBase::OffsetT startLine = DetectStartOffset() / m_LineBytes;
    FileContentBase::OffsetT endLine   = startLine + m_Lines;

    if ( line < startLine )
    {
        m_LastScrollPos   = line;
        m_LastScrollUnits = line / m_LinesPerScrollUnit;

        m_ContentScroll->SetThumbPosition( m_LastScrollUnits );
        m_DrawArea->Refresh();
    }
    else if ( line >= endLine )
    {
        line = line - m_Lines + 1;

        m_LastScrollPos   = line;
        m_LastScrollUnits = line / m_LinesPerScrollUnit;

        m_ContentScroll->SetThumbPosition( m_LastScrollUnits );
        m_DrawArea->Refresh();
    }
}

void HexEditPanel::ClampCursorToVisibleArea()
{
    FileContentBase::OffsetT startOffs = DetectStartOffset();
    FileContentBase::OffsetT endOffs   = startOffs + m_LineBytes * m_Lines;

    bool changed = false;

    if ( m_Current < startOffs )
    {
        m_Current = startOffs + ( m_Current % m_LineBytes );
        changed = true;
    }
    else if ( m_Current >= endOffs )
    {
        m_Current = endOffs - m_LineBytes + ( m_Current % m_LineBytes );
        changed = true;
    }

    if ( m_Current >= m_Content->GetSize() )
    {
        m_Current = m_Content->GetSize() - 1;
        changed = true;
    }

    if ( changed )
    {
        PropagateOffsetChange();
    }
}

void HexEditPanel::PropagateOffsetChange( int flagsForCurrentView )
{
    if ( !m_Content ) return;

    OffsetT startOffs  = DetectStartOffset();

    OffsetT blockStart = m_Current;
    OffsetT blockEnd   = m_Current + 1;

    // First let's calculate block size
    for ( int i=0; i<MAX_VIEWS && m_Views[ i ]; ++i )
    {
        OffsetT thisBlockStart = blockStart;
        OffsetT thisBlockEnd   = blockEnd;
        m_Views[i]->CalculateBlockSize( startOffs, m_Current, thisBlockStart, thisBlockEnd );
        blockStart = wxMin( blockStart, thisBlockStart );
        blockEnd   = wxMax( blockEnd,   thisBlockEnd   );
    }

    // Next we can propagate the offset
    for ( int i=0; i<MAX_VIEWS && m_Views[ i ]; ++i )
    {
        m_Views[i]->JumpToOffset( startOffs, m_Current, blockStart, blockEnd, ( m_Views[ i ] == m_ActiveView ) ? flagsForCurrentView : -1 );
    }
}

void HexEditPanel::OnDrawAreaEraseBackground(wxEraseEvent& /*event*/)
{
}

void HexEditPanel::OnForwardFocus(wxFocusEvent& /*event*/)
{
    m_DrawArea->SetFocus();
}

bool HexEditPanel::Save()
{
    bool ret = m_Content->WriteFile( GetFilename() );
    UpdateModified();
    return ret;
}

bool HexEditPanel::SaveAs()
{
    wxFileName fname;
    fname.Assign(GetFilename());
    ConfigManager* mgr = Manager::Get()->GetConfigManager(_T("app"));

    wxString Path = fname.GetPath();

    if(mgr && Path.IsEmpty())
        Path = mgr->Read(_T("/file_dialogs/save_file_as/directory"), Path);

    wxFileDialog dlg(Manager::Get()->GetAppWindow(),
                      _("Save file"),
                      Path,
                      fname.GetFullName(),
                      _T("*.*"),//m_filecontent->GetWildcard(),
                      wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (dlg.ShowModal() != wxID_OK) // cancelled out
    {
        UpdateModified();
        return false;
    }

    SetFilename(dlg.GetPath());
    return Save();
}

bool HexEditPanel::GetModified() const
{
    return m_Content ? m_Content->Modified() : false;
}

void HexEditPanel::SetModified( bool modified )
{
    if ( m_Content ) m_Content->SetModified( modified );
}

void HexEditPanel::UpdateModified()
{
    if ( GetModified() )
    {
        SetTitle( _T("*") + GetShortName() );
    }
    else
    {
        SetTitle( GetShortName() );
    }
}

void HexEditPanel::SetFilename(const wxString& filename)
{
    EditorBase::SetFilename(filename);
    //m_Filename = filename; done by EditorBase::SetFilename
    wxFileName fname;
    fname.Assign(m_Filename);
    m_Shortname = fname.GetFullName();
}

void HexEditPanel::OnDrawAreaLeftDown(wxMouseEvent& event)
{
    if ( !m_Content ) return;

    m_DrawArea->SetFocus();

    // First we need to detect what the user has clicked on
    int line   = event.GetY() / m_FontY;
    int column = event.GetX() / m_FontX;

    // Just to prevent some weird situation
    line   = wxMin( line,   (int)m_Lines - 1 );
    column = wxMin( column, (int)m_Cols  - 1 );
    line   = wxMax( line,   0 );
    column = wxMax( column, 0 );

    // Detect what has been pressed

    if ( !m_MouseDown )
    {
        if ( column < 9 )
        {
            // Offset pressed
            return;
        }
    }
    column -= 9;

    int viewId = -1;

    for ( int i=0; i<MAX_VIEWS; ++i )
    {
        column -= 2;

        if ( !m_MouseDown )
        {
            if ( column < 0 ) break;

            if ( column < (int)m_ViewsCols[ i ] )
            {
                // Clicked on the area of i-th view
                ActivateView( m_Views[ i ] );

                viewId = i;
                break;
            }
        }
        else if ( m_Views[ i ] == m_ActiveView )
        {
            viewId = i;
            break;
        }

        column -= m_ViewsCols[ i ];
    }

    if ( viewId >= 0  )
    {
        m_MouseDown = true;

        column = wxMax( column, 0 );
        column = wxMin( column, (int)m_ViewsCols[ viewId ] );

        int positionFlags;
        int lineOffset = m_Views[ viewId ]->GetOffsetFromColumn( column, positionFlags );
        lineOffset = wxMin( lineOffset, (int)m_LineBytes - 1 );
        lineOffset = wxMax( lineOffset, 0 );

        OffsetT newCurrent = DetectStartOffset() + m_LineBytes * line + lineOffset;

        if ( newCurrent < m_Content->GetSize() )
        {
            if ( ( newCurrent != m_Current ) || ( positionFlags != m_Views[ viewId ]->GetCurrentPositionFlags() ) )
            {
                m_Current = newCurrent;
                PropagateOffsetChange( positionFlags );
                RefreshStatus();
                EnsureCarretVisible();
                m_DrawArea->Refresh();
            }
        }
    }
    else
    {
        m_MouseDown = false;
        // Clicked somewhere after all views
    }
}

void HexEditPanel::OnDrawAreaLeftUp(wxMouseEvent& /*event*/)
{
    m_MouseDown = false;
}

void HexEditPanel::OnDrawAreaMouseMove(wxMouseEvent& event)
{
    if ( m_MouseDown )
    {
        OnDrawAreaLeftDown( event );
    }
}

void HexEditPanel::OnCheckBox1Click(wxCommandEvent& /*event*/)
{
    if ( !m_Content ) return;
    BoxSizer1->Show( PreviewSizer, CheckBox1->GetValue() );
    m_DrawArea->SetFocus();
    Layout();
}

bool HexEditPanel::CanUndo() const
{
    return m_Content ? m_Content->CanUndo() : false;
}

bool HexEditPanel::CanRedo() const
{
    return m_Content ? m_Content->CanRedo() : false;
}

void HexEditPanel::Undo()
{
    if ( m_Content )
    {
        const FileContentBase::ExtraUndoData* extraData = m_Content->Undo();
        if ( extraData )
        {
            m_Current = extraData->m_PosBefore;
            ActivateView( extraData->m_View );
            PropagateOffsetChange( extraData->m_PosBeforeF );
        }
        RefreshStatus();
        EnsureCarretVisible();
        m_DrawArea->Refresh();
        UpdateModified();
    }
}

void HexEditPanel::Redo()
{
    if ( m_Content )
    {
        const FileContentBase::ExtraUndoData* extraData = m_Content->Redo();
        if ( extraData )
        {
            m_Current = extraData->m_PosAfter;
            ActivateView( extraData->m_View );
            PropagateOffsetChange( extraData->m_PosAfterF );
        }
        RefreshStatus();
        EnsureCarretVisible();
        m_DrawArea->Refresh();
        UpdateModified();
    }
}

void HexEditPanel::OnSpecialKeyDown(wxKeyEvent& event)
{
//    LogManager::Get()->DebugLog(
//        F(
//            _T("HexEditPanel::OnSpecialKeyDown: %d (%c%c%c)"),
//            (int)event.GetKeyCode(),
//            event.ControlDown() ? 'C':'c',
//            event.AltDown() ? 'A':'a',
//            event.CmdDown() ? 'M':'m' ) );

    if ( event.ControlDown() && !event.AltDown() )
    {
        switch ( event.GetKeyCode() )
        {
            case 'G': ProcessGoto(); return;
            case 'F': ProcessSearch(); return;
            default: break;
        }
    }

    event.Skip();
}

bool HexEditPanel::IsHexEditor( EditorBase* editor )
{
    return m_AllEditors.find( editor ) != m_AllEditors.end();
}

void HexEditPanel::CloseAllEditors()
{
    EditorsSet s = m_AllEditors;
    for ( EditorsSet::iterator i = s.begin(); i != s.end(); ++i )
    {
        EditorManager::Get()->QueryClose( *i );
        (*i)->Close();
    }

    assert( m_AllEditors.empty() );
}

void HexEditPanel::ProcessGoto()
{
    if ( !m_Content ) return;
    if ( !m_Content->GetSize() ) return;


    OffsetT offset;
    wxString str = wxString::Format( _T("%lld"), m_Current );
    for ( ;; )
    {
        str = cbGetTextFromUser(
            _("Enter offset\n"
              "\n"
              "Available forms are:\n"
              " * Decimal ( 100 )\n"
              " * Hexadecimal ( 1AB, 0x1AB, 1ABh )\n"
              " * Offset from current ( +100, -100, +0x1AB )"),
            _("Goto offset"),
            str );

        if ( str.IsEmpty() ) return;
        str.Trim( true ).Trim( false );

        // Decided to parse manually since wxString::ToULongLong does not work everywhere

        const wxChar* ptr = str.c_str();
        bool relativePlus  = false;
        bool relativeMinus = false;
        bool canBeDec = true;
        bool canBeHex = true;

        OffsetT dec = 0;
        OffsetT hex = 0;

        if ( *ptr == _T('+') )
        {
            relativePlus = true;
            ptr++;
        }
        else if ( *ptr == _T('-') )
        {
            relativeMinus = true;
            ptr++;
        }

        while ( wxIsspace( *ptr ) ) ptr++;

        if ( ptr[0] == _T('0') && wxToupper(ptr[1]) == _T('X') )
        {
            canBeDec = false;
            ptr += 2;
        }

        while ( *ptr )
        {
            int digitVal = wxString( _T("0123456789ABCDEF") ).Find( wxToupper( *ptr++ ) );

            if ( digitVal == wxNOT_FOUND )
            {
                canBeDec = false;
                canBeHex = false;
                break;
            }

            if ( digitVal >= 10 ) canBeDec = false;

            dec = dec *   10 + digitVal;
            hex = hex * 0x10 + digitVal;

            if ( wxToupper(ptr[0]) == _T('H') && !ptr[1] )
            {
                canBeDec = false;
                break;
            }
        }

        if ( canBeDec || canBeHex )
        {
            OffsetT val = canBeDec ? dec : hex;
            OffsetT max = m_Content->GetSize() - 1;
            if ( relativePlus )
            {
                offset = m_Current + val < max ? m_Current + val : max;
            }
            else if ( relativeMinus )
            {
                offset = m_Current > val ? m_Current - val : 0;
            }
            else
            {
                offset = wxMin( max, val );
            }
            break;
        }

        cbMessageBox( _("Invalid offset !!!.\n") );
    }

    m_Current = offset;
    PropagateOffsetChange();
    RefreshStatus();
    EnsureCarretVisible();
    m_DrawArea->Refresh();
}

void HexEditPanel::ProcessSearch()
{
    if ( !m_Content ) return;
    if ( !m_Content->GetSize() ) return;

    SearchDialog dlg( this, m_Content, m_Current );
    if ( dlg.ShowModal() == wxID_OK )
    {
        m_Current = dlg.GetOffset();
        PropagateOffsetChange();
        RefreshStatus();
        EnsureCarretVisible();
        m_DrawArea->Refresh();
    }

    m_DrawArea->SetFocus();
}

void HexEditPanel::OnButton1Click(wxCommandEvent& /*event*/)
{
    ExpressionTester( 0, m_Content, m_Current).ShowModal();
}

void HexEditPanel::ReparseExpression()
{
    Expression::Parser parser;
    if ( !parser.Parse( m_Expression->GetValue(), m_ExpressionCode ) )
    {
        int pos;
        m_ExpressionError = parser.ParseErrorDesc( pos );
    }
    else
    {
        m_ExpressionError.Clear();
    }
}

void HexEditPanel::OnReparseTimerTrigger(wxTimerEvent& /*event*/)
{
    ReparseExpression();
    RefreshStatus();
}

void HexEditPanel::Onm_ExpressionText(wxCommandEvent& /*event*/)
{
    ReparseTimer.Start( 1000, wxTIMER_ONE_SHOT );
}

void HexEditPanel::OnButton2Click(wxCommandEvent& /*event*/)
{
    cbMessageBox( Expression::Parser::GetHelpString() );
}

void HexEditPanel::OnExpressionTextEnter(wxCommandEvent& /*event*/)
{
    ReparseExpression();
    RefreshStatus();
    ReparseTimer.Stop();
}

void HexEditPanel::OnButton3Click1(wxCommandEvent& event)
{
    SelectStoredExpressionDlg dlg( this, m_Expression->GetValue() );
    if ( dlg.ShowModal() == wxID_OK )
    {
        m_Expression->SetValue( dlg.GetExpression() );
        OnExpressionTextEnter(event);
    }
}

void HexEditPanel::OnButton4Click(wxCommandEvent& /*event*/)
{
    PopupMenu( &m_BaseMenu );
}

void HexEditPanel::OnSetBaseHex(wxCommandEvent& /*event*/)
{
    m_DigitView->SetDigitBits( 4 );
    DisplayChanged();
}

void HexEditPanel::OnSetBaseBin(wxCommandEvent& /*event*/)
{
    m_DigitView->SetDigitBits( 1 );
    DisplayChanged();
}

void HexEditPanel::DisplayChanged()
{
    m_NeedRecalc = true;
    RefreshStatus();
    EnsureCarretVisible();
    m_DrawArea->Refresh();
    m_DrawArea->SetFocus();
}

void HexEditPanel::Onm_EndianessClick(wxCommandEvent& /*event*/)
{
    PopupMenu( &m_EndianessMenu );
}

void HexEditPanel::OnSetEndianessBig(wxCommandEvent& /*event*/)
{
    m_DigitView->SetLittleEndian( false );
    DisplayChanged();
}

void HexEditPanel::OnSetEndianessLittle(wxCommandEvent& /*event*/)
{
    m_DigitView->SetLittleEndian( true );
    DisplayChanged();
}

void HexEditPanel::Onm_BlockSizeClick(wxCommandEvent& /*event*/)
{
    PopupMenu( &m_BlockSizeMenu );
}

void HexEditPanel::OnSetBlockSize1(wxCommandEvent& /*event*/)
{
    m_DigitView->SetBlockBytes( 1 );
    DisplayChanged();
}

void HexEditPanel::OnSetBlockSize2(wxCommandEvent& /*event*/)
{
    m_DigitView->SetBlockBytes( 2 );
    DisplayChanged();
}

void HexEditPanel::OnSetBlockSize4(wxCommandEvent& /*event*/)
{
    m_DigitView->SetBlockBytes( 4 );
    DisplayChanged();
}

void HexEditPanel::OnSetBlockSize8(wxCommandEvent& /*event*/)
{
    m_DigitView->SetBlockBytes( 8 );
    DisplayChanged();
}

void HexEditPanel::Onm_ColsModeClick(wxCommandEvent& /*event*/)
{
    PopupMenu( &m_ColsModeMenu );
}

void HexEditPanel::OnSetColsMul1(wxCommandEvent& /*event*/)
{
    ColsMode( CM_MULT, 1 );
}

void HexEditPanel::OnSetColsMul2(wxCommandEvent& /*event*/)
{
    ColsMode( CM_MULT, 2 );
}

void HexEditPanel::OnSetColsMul3(wxCommandEvent& /*event*/)
{
    ColsMode( CM_MULT, 3 );
}

void HexEditPanel::OnSetColsMul4(wxCommandEvent& /*event*/)
{
    ColsMode( CM_MULT, 4 );
}

void HexEditPanel::OnSetColsMul5(wxCommandEvent& /*event*/)
{
    ColsMode( CM_MULT, 5 );
}

void HexEditPanel::OnSetColsMul6(wxCommandEvent& /*event*/)
{
    ColsMode( CM_MULT, 6 );
}

void HexEditPanel::OnSetColsMul7(wxCommandEvent& /*event*/)
{
    ColsMode( CM_MULT, 7 );
}

void HexEditPanel::OnSetColsMul8(wxCommandEvent& /*event*/)
{
    ColsMode( CM_MULT, 8 );
}

void HexEditPanel::OnSetColsMulOther(wxCommandEvent& /*event*/)
{
    long val = ::wxGetNumberFromUser( _("Enter number"), _("Enter number"), _("Colums setting"), 2, 2, 100, this );
    if ( val > 0 ) ColsMode( CM_MULT, val );
}

void HexEditPanel::OnSetColsValue1(wxCommandEvent& /*event*/)
{
    ColsMode( CM_SPECIFIED, 1 );
}

void HexEditPanel::OnSetColsValue2(wxCommandEvent& /*event*/)
{
    ColsMode( CM_SPECIFIED, 2 );
}

void HexEditPanel::OnSetColsValue3(wxCommandEvent& /*event*/)
{
    ColsMode( CM_SPECIFIED, 3 );
}

void HexEditPanel::OnSetColsValue4(wxCommandEvent& /*event*/)
{
    ColsMode( CM_SPECIFIED, 4 );
}

void HexEditPanel::OnSetColsValue5(wxCommandEvent& /*event*/)
{
    ColsMode( CM_SPECIFIED, 5 );
}

void HexEditPanel::OnSetColsValue6(wxCommandEvent& /*event*/)
{
    ColsMode( CM_SPECIFIED, 6 );
}

void HexEditPanel::OnSetColsValue7(wxCommandEvent& /*event*/)
{
    ColsMode( CM_SPECIFIED, 7 );
}

void HexEditPanel::OnSetColsValue8(wxCommandEvent& /*event*/)
{
    ColsMode( CM_SPECIFIED, 8 );
}

void HexEditPanel::OnSetColsValueOther(wxCommandEvent& /*event*/)
{
    long val = ::wxGetNumberFromUser( _("Enter number"), _("Enter number"), _("Colums setting"), 1, 1, 100, this );
    if ( val > 0 ) ColsMode( CM_SPECIFIED, val );
}

void HexEditPanel::OnSetColsPower2(wxCommandEvent& /*event*/)
{
    ColsMode( CM_POWER, 2 );
}

void HexEditPanel::OnSetColsPower4(wxCommandEvent& /*event*/)
{
    ColsMode( CM_POWER, 4 );
}

void HexEditPanel::OnSetColsPower8(wxCommandEvent& /*event*/)
{
    ColsMode( CM_POWER, 8 );
}

void HexEditPanel::OnSetColsPowerOther(wxCommandEvent& /*event*/)
{
    long val = ::wxGetNumberFromUser( _("Enter number"), _("Enter number"), _("Colums setting"), 2, 2, 100, this );
    if ( val > 0 ) ColsMode( CM_POWER, val );
}

void HexEditPanel::ColsMode(int mode, int value)
{
    m_ColsMode = mode;
    m_ColsValue = value;

    if ( m_ColsMode == CM_MULT && m_ColsValue == 1 )
    {
        m_ColsMode = CM_ANY;
    }

    DisplayChanged();
}

bool HexEditPanel::MatchColumnsCount(int colsCount)
{
    switch ( m_ColsMode )
    {
        case CM_MULT:
            return ( colsCount % m_ColsValue ) == 0;

        case CM_SPECIFIED:
            return colsCount == m_ColsValue;

        case CM_POWER:
            while ( colsCount > 1 )
            {
                if ( colsCount % m_ColsValue ) return false;
                colsCount /= m_ColsValue;
            }
            return true;

        default:
            return true;
    }
}

void HexEditPanel::OnButton4Click1(wxCommandEvent& /*event*/)
{
    wxArrayString tests;
    tests.Add(_("Expression parser"));
    tests.Add(_("On-Disk file edition"));

    int index = cbGetSingleChoiceIndex( _("Select tests to perform"), _("Self tests"), tests, this);
    TestCasesBase* test = 0;

    switch ( index )
    {
        case 0: test = &Expression::GetTests(); break;
        case 1: test = &FileContentDisk::GetTests(); break;
        default: break;
    }

    if ( !test ) return;

    TestCasesDlg( this, *test ).ShowModal();
}


void HexEditPanel::OnButton6Click(wxCommandEvent& /*event*/)
{
    ProcessGoto();
}

void HexEditPanel::OnButton5Click(wxCommandEvent& /*event*/)
{
    ProcessSearch();
}
