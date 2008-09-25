/*
* This file is part of HexEditor plugin for Code::Blocks Studio
* Copyright (C) 2008 Bartlomiej Swiecki
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
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision:$
* $Id:$
* $HeadURL:$
*/

#include "HexEditPanel.h"
#include "ExpressionTester.h"
#include "SelectStoredExpressionDlg.h"

//(*InternalHeaders(HexEditPanel)
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include <wx/intl.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/dcclient.h>
#include <wx/dcbuffer.h>
#include <manager.h>
#include <editormanager.h>
#include <logmanager.h>
#include <wx/textdlg.h>
#include "ExpressionExecutor.h"
#include "ExpressionParser.h"

//(*IdInit(HexEditPanel)
const long HexEditPanel::ID_STATICTEXT1 = wxNewId();
const long HexEditPanel::ID_BUTTON1 = wxNewId();
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
    , m_ScreenBuffer( 0 )
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
    BoxSizer3->Add(m_Status, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button1 = new wxButton(this, ID_BUTTON1, _("Calc"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON1"));
    BoxSizer3->Add(Button1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    CheckBox1 = new wxCheckBox(this, ID_CHECKBOX1, _("Value preview"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
    CheckBox1->SetValue(true);
    BoxSizer3->Add(CheckBox1, 0, wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(BoxSizer3, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    m_DrawArea = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS|wxFULL_REPAINT_ON_RESIZE, _T("ID_PANEL1"));
    BoxSizer2->Add(m_DrawArea, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_ContentScroll = new wxScrollBar(this, ID_SCROLLBAR1, wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL, wxDefaultValidator, _T("ID_SCROLLBAR1"));
    m_ContentScroll->SetScrollbar(0, 1, 1, 1);
    BoxSizer2->Add(m_ContentScroll, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer1->Add(BoxSizer2, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    PreviewSizer = new wxBoxSizer(wxVERTICAL);
    FlexGridSizer1 = new wxFlexGridSizer(0, 8, 5, 5);
    FlexGridSizer1->AddGrowableCol(1);
    FlexGridSizer1->AddGrowableCol(4);
    FlexGridSizer1->AddGrowableCol(7);
    StaticText1 = new wxStaticText(this, ID_STATICTEXT2, _("Byte:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    FlexGridSizer1->Add(StaticText1, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_ByteVal = new wxStaticText(this, ID_STATICTEXT3, _("-9999999999"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    FlexGridSizer1->Add(m_ByteVal, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(5,5,1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText3 = new wxStaticText(this, ID_STATICTEXT4, _("Word:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
    FlexGridSizer1->Add(StaticText3, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_WordVal = new wxStaticText(this, ID_STATICTEXT5, _("-9999999999"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
    FlexGridSizer1->Add(m_WordVal, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(5,5,1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText2 = new wxStaticText(this, ID_STATICTEXT6, _("Dword:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
    FlexGridSizer1->Add(StaticText2, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_DwordVal = new wxStaticText(this, ID_STATICTEXT7, _("-9999999999"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
    FlexGridSizer1->Add(m_DwordVal, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText4 = new wxStaticText(this, ID_STATICTEXT8, _("Float:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
    FlexGridSizer1->Add(StaticText4, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_FloatVal = new wxStaticText(this, ID_STATICTEXT9, _("-9999999999"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT9"));
    FlexGridSizer1->Add(m_FloatVal, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(5,5,1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText6 = new wxStaticText(this, ID_STATICTEXT10, _("Double:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT10"));
    FlexGridSizer1->Add(StaticText6, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_DoubleVal = new wxStaticText(this, ID_STATICTEXT11, _("-9999999999"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT11"));
    FlexGridSizer1->Add(m_DoubleVal, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(5,5,1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText8 = new wxStaticText(this, ID_STATICTEXT12, _("L-Double:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT12"));
    FlexGridSizer1->Add(StaticText8, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_LDoubleVal = new wxStaticText(this, ID_STATICTEXT13, _("-9999999999"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT13"));
    FlexGridSizer1->Add(m_LDoubleVal, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    PreviewSizer->Add(FlexGridSizer1, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    StaticText5 = new wxStaticText(this, ID_STATICTEXT14, _("Expression:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT14"));
    BoxSizer4->Add(StaticText5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_Expression = new wxTextCtrl(this, ID_TEXTCTRL1, _("byte[ @ ]"), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    BoxSizer4->Add(m_Expression, 1, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button3 = new wxButton(this, ID_BUTTON3, _("v"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON3"));
    BoxSizer4->Add(Button3, 0, wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button2 = new wxButton(this, ID_BUTTON2, _("\?"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON2"));
    BoxSizer4->Add(Button2, 0, wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_ExpressionVal = new wxStaticText(this, ID_STATICTEXT15, _("-9999999999"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT15"));
    BoxSizer4->Add(m_ExpressionVal, 1, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    PreviewSizer->Add(BoxSizer4, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1->Add(PreviewSizer, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(BoxSizer1);
    ReparseTimer.SetOwner(this, ID_TIMER1);
    BoxSizer1->SetSizeHints(this);
    
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&HexEditPanel::OnButton1Click);
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
    //*)

    // We connect these events manually
    m_DrawArea->Connect( wxEVT_SIZE, (wxObjectEventFunction)&HexEditPanel::OnContentSize,     0, this );
    m_DrawArea->SetBackgroundStyle( wxBG_STYLE_CUSTOM );
    Connect( wxEVT_SET_FOCUS, (wxObjectEventFunction)&HexEditPanel::OnForwardFocus );

    ReparseExpression();
    SetFontSize( 8 );
    ReadContent();
    RecalculateCoefs();
    RefreshStatus();

    m_SelectionStart = 0;
    m_SelectionEnd   = 0;
    m_Current        = 0;
    m_CurrentType    = curHexHi;
    m_MouseDown      = false;

    m_DrawArea->SetFocus();

    ((wxWindow*)Manager::Get()->GetEditorManager()->GetNotebook())->Layout();

    m_Shortname = title;
    SetTitle( m_Shortname );

    m_AllEditors.insert( this );

}


HexEditPanel::~HexEditPanel()
{
    m_AllEditors.erase( this );

    delete m_DrawFont;
    m_DrawFont = 0;

    delete m_Content;
    m_Content = 0;

    delete[] m_ScreenBuffer;
    m_ScreenBuffer = 0;

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

void HexEditPanel::RecalculateCoefs()
{
    wxClientDC dc( this );
    dc.GetTextExtent( _T("0123456789ABCDEF"), &m_FontX, &m_FontY, NULL, NULL, m_DrawFont );
    m_FontX /= 16;

    wxSize size = m_DrawArea->GetClientSize();

    int maxCharsInLine = size.GetWidth() / m_FontX;
    m_Cols  = ( maxCharsInLine - 15 ) / 4;
    m_Lines = size.GetHeight() / m_FontY;
    if ( m_Cols < 1  ) m_Cols  = 1;
    if ( m_Lines < 1 ) m_Lines = 1;

    FileContentBase::OffsetT contentSize = m_Content ? m_Content->GetSize() : 0;

    m_ContentScroll->SetScrollbar(
        m_ContentScroll->GetThumbPosition(),
        m_Lines,
        ( contentSize + m_Cols - 1 ) / m_Cols,
        m_Lines );

    delete[] m_ScreenBuffer;
    m_ScreenBuffer = (m_Lines && m_Cols) ? new unsigned char[ m_Lines * m_Cols ] : 0;
}

void HexEditPanel::OnContentPaint( wxPaintEvent& event )
{
    wxAutoBufferedPaintDC dc( m_DrawArea );
    dc.SetBrush( GetBackgroundColour() );
    dc.SetPen  ( GetBackgroundColour() );
    dc.DrawRectangle( GetClientRect() );

    if ( !m_Content ) return;

    dc.SetFont( *m_DrawFont );

    FileContentBase::OffsetT startOffs = DetectStartOffset();

    OffsetT size = m_Content->Read( m_ScreenBuffer, startOffs, m_Lines * m_Cols );

    for ( OffsetT i=0; i< m_Lines; i++ )
    {
        FileContentBase::OffsetT offs = startOffs + i * m_Cols;
        FileContentBase::OffsetT offsMax = offs + m_Lines * m_Cols;
        offs = wxMin( offs, m_Content->GetSize() );

        PutLine(
            dc,
            offs,
            m_ScreenBuffer + i * m_Cols,
            wxMax( wxMin( size, m_Cols ), 0 ),
            0,
            i * m_FontY,
            (int)(wxMax( offs, wxMin( offsMax, m_SelectionStart ) ) - offs),
            (int)(wxMax( offs, wxMin( offsMax, m_SelectionEnd   ) ) - offs)
            );

        size = ( size > m_Cols ) ? ( size - m_Cols ) : 0;
    }
}

void HexEditPanel::OnContentScroll( wxScrollEvent& event )
{
    if ( !m_Content || !m_Content->GetSize() )
    {
        return;
    }

    ClampCursorToVisibleArea();
    m_DrawArea->Refresh();
    RefreshStatus();
    m_DrawArea->SetFocus();
}

void HexEditPanel::PutLine( wxDC& dc, FileContentBase::OffsetT offs, unsigned char* buffer, unsigned int len, int x, int y, unsigned int selStart, unsigned int selEnd )
{
    static const char catNoChar = -1;
    static const char catNormal = 0;
    static const char catSelect = 1;
    static const char catCurCar = 2;
    static const char catCurNon = 3;

    static const int catCount = 4;

    if ( len <= 0 ) return;
    if ( len >= 0x1000 ) return;

    dc.SetLogicalFunction( wxCOPY );
    dc.SetBrush( *wxWHITE );

    // We need to categorize each character
    char categoryBuffer[ 0x1000 ];
    for ( unsigned int i=0; i<m_Cols && i < 0x1000; i++ )
    {
        if ( i >= len )
        {
            categoryBuffer[i] = catNoChar;
        }
        else if ( i < selStart || i >= selEnd )
        {
            categoryBuffer[i] = catNormal;
        }
        else
        {
            categoryBuffer[i] = catSelect;
        }
    }

    const wxColour foregrounds[ catCount ] =
    {
        *wxBLACK,
        *wxWHITE,
        *wxWHITE,
        *wxBLACK,
    };

    const wxColour backgrounds[ catCount ] =
    {
        GetBackgroundColour(),
        wxColour( 0x70, 0x70, 0x70 ),
        wxColour( 0xA0, 0xA0, 0xFF ),
        wxColour( 0x80, 0x80, 0xFF ),
    };

    // First - let's draw offset
    dc.SetTextForeground( foregrounds[0] );
    dc.SetTextBackground( backgrounds[0] );
    wxString offsStr = wxString::Format( _T("%08X: "), offs );
    dc.DrawText( offsStr, x, y );
    x += m_FontX * 10;

    // Now the content
    unsigned int pos = 0;
    int x1 = x;
    int x2 = x + m_Cols * 3 * m_FontX + 2 * m_FontX;

    while ( pos < len )
    {
        // Search for continous block of data
        unsigned int endPos = pos;

        do endPos++;
        while ( endPos < len && categoryBuffer[pos] == categoryBuffer[endPos] );

        int category = categoryBuffer[pos];
        dc.SetBrush( backgrounds[ category ] );
        dc.SetPen  ( backgrounds[ category ] );
        dc.SetTextForeground( foregrounds[ category ] );
        dc.SetTextBackground( backgrounds[ category ] );

        wxString text1;
        wxString text2;

        for ( unsigned int i = pos; i < endPos; ++i )
        {
            unsigned char ch = buffer[ i ];

            wxChar Buff[4] =
            {
                _T("0123456789ABCDEF") [ ch / 0x10 ],
                _T("0123456789ABCDEF") [ ch % 0x10 ],
                _T(' '),
                0
            };

            text1 += Buff;

            if ( ch < 0x20 || ch >= 0x7F ) ch = _T('.');
            text2 += ch;
        }

        text1.RemoveLast();

        dc.DrawRectangle( x1, y, m_FontX * text1.Length(), m_FontY );
        dc.DrawText( text1, x1, y );
        x1 += m_FontX * text1.Length() + m_FontX;

        dc.DrawRectangle( x2, y, m_FontX * text2.Length(), m_FontY );
        dc.DrawText( text2, x2, y );
        x2 += m_FontX * text2.Length();

        pos = endPos;
    }

    // Finally we add carret mark
    if ( m_Current >= offs && m_Current < offs + len )
    {
        int curOffs = m_Current - offs;
        x1 = x + 3 * m_FontX * curOffs;
        x2 = x + m_Cols * 3 * m_FontX + 2 * m_FontX + m_FontX * curOffs;

        unsigned char ch = buffer[ curOffs ];

        wxChar tmp[6] =
        {
            _T("0123456789ABCDEF") [ ch / 0x10 ], 0,
            _T("0123456789ABCDEF") [ ch % 0x10 ], 0,
            ( ch >= 0x20 && ch < 0x7F ) ? ch : ' ', 0
        };

        dc.SetBrush( backgrounds[ (int)catCurNon ] );
        dc.SetPen  ( backgrounds[ (int)catCurNon ] );
        dc.SetTextForeground( foregrounds[ (int)catCurNon ] );
        dc.SetTextBackground( backgrounds[ (int)catCurNon ] );

        switch ( m_CurrentType )
        {
            case curHexHi:
                dc.DrawRectangle( x1 + m_FontX , y, m_FontX, m_FontY );
                dc.DrawRectangle( x2, y, m_FontX, m_FontY );
                dc.DrawText( tmp+2, x1 + m_FontX, y );
                dc.DrawText( tmp+4, x2, y );
                break;

            case curHexLo:
                dc.DrawRectangle( x1, y, m_FontX, m_FontY );
                dc.DrawRectangle( x2, y, m_FontX, m_FontY );
                dc.DrawText( tmp+0, x1, y );
                dc.DrawText( tmp+4, x2, y );
                break;

            case curChar:
                dc.DrawRectangle( x1, y, 2*m_FontX, m_FontY );
                dc.DrawText( tmp+0, x1, y );
                dc.DrawText( tmp+2, x1 + m_FontX, y );
                break;
        }

        dc.SetBrush( backgrounds[ (int)catCurCar ] );
        dc.SetPen  ( backgrounds[ (int)catCurCar ] );
        dc.SetTextForeground( foregrounds[ (int)catCurCar ] );
        dc.SetTextBackground( backgrounds[ (int)catCurCar ] );

        switch ( m_CurrentType )
        {
            case curHexHi:
                dc.DrawRectangle( x1, y, m_FontX, m_FontY );
                dc.DrawText( tmp+0, x1, y );
                break;

            case curHexLo:
                dc.DrawRectangle( x1 + m_FontX, y, m_FontX, m_FontY );
                dc.DrawText( tmp+2, x1 + m_FontX, y );
                break;

            case curChar:
                dc.DrawRectangle( x2, y, m_FontX, m_FontY );
                dc.DrawText( tmp+4, x2, y );
                break;
        }

    }
}

void HexEditPanel::OnContentSize( wxSizeEvent& event )
{
    RecalculateCoefs();
    EnsureCarretVisible();
    RefreshStatus();
    event.Skip();
}

FileContentBase::OffsetT HexEditPanel::DetectStartOffset()
{
    return m_ContentScroll->GetThumbPosition() * m_Cols;
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
        m_Status->SetLabel( F( _("Position: %llX / %llX ( %lld%% )"), m_Current, size-1, ( m_Current * 100 / (size-1) ) ) );
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
            LogManager::Get()->DebugLog( F( _T("HEExpr Calculate: %d"), (int)sw.Time() ) );
            unsigned long long uint;
            long long          sint;
            long double        flt;

            if ( executor.GetResult( uint ) )
            {
                m_ExpressionVal->SetLabel( wxString::Format( _T("%llu"), uint) );
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

}

void HexEditPanel::OnDrawAreaKeyDown(wxKeyEvent& event)
{
    if ( !m_Content || !m_Content->GetSize() ) return;

    switch ( event.GetKeyCode() )
    {
        case WXK_LEFT:
            switch ( m_CurrentType )
            {
                case curChar:
                    if ( m_Current > 0 )
                    {
                        m_Current--;
                    }
                    break;

                case curHexHi:
                    if ( m_Current > 0 )
                    {
                        m_Current--;
                        m_CurrentType = curHexLo;
                    }
                    break;

                case curHexLo:
                    m_CurrentType = curHexHi;
                    break;

                default:
                    return;
            }
            break;

        case WXK_RIGHT:
            switch ( m_CurrentType )
            {
                case curChar:
                    if ( m_Current < m_Content->GetSize() - 1 )
                    {
                        m_Current++;
                    }
                    break;

                case curHexLo:
                    if ( m_Current < m_Content->GetSize() - 1 )
                    {
                        m_Current++;
                        m_CurrentType = curHexHi;
                    }
                    break;

                case curHexHi:
                    m_CurrentType = curHexLo;
                    break;

                default:
                    return;
            }
            break;

        case WXK_TAB:
            switch ( m_CurrentType )
            {
                case curChar:
                    m_CurrentType = curHexHi;
                    break;

                case curHexHi:
                case curHexLo:
                    m_CurrentType = curChar;
                    break;

                default:
                    return;
            }
            break;

        case WXK_UP:
            if ( m_Current >= (FileContentBase::OffsetT)m_Cols )
            {
                m_Current -= m_Cols;
                break;
            }
            return;

        case WXK_DOWN:
            if ( m_Current < m_Content->GetSize() - m_Cols )
            {
                m_Current += m_Cols;
                break;
            }
            return;

        case WXK_PAGEDOWN:
            for ( unsigned int i=0; i<m_Lines/2; i++ )
            {
                if ( m_Current >= m_Content->GetSize() - m_Cols ) break;
                m_Current += m_Cols;
            }
            break;

        case WXK_PAGEUP:
            for ( unsigned int i=0; i<m_Lines/2; i++ )
            {
                if ( m_Current < (FileContentBase::OffsetT)m_Cols ) break;
                m_Current -= m_Cols;
            }
            break;

        case WXK_HOME:
            m_Current = 0;
            break;

        case WXK_END:
            m_Current = m_Content->GetSize()-1;
            break;

        case WXK_INSERT:
        {
            // Insert empty byte at current position
            FileContentBase::ExtraUndoData data( m_Current, m_CurrentType, m_Current, (m_Current==curChar) ? curChar : curHexHi );
            m_Content->Add( data, m_Current, 1, 0 );
            break;
        }

        case WXK_DELETE:
        {
            FileContentBase::ExtraUndoData data( m_Current, m_CurrentType, m_Current, (m_Current==curChar) ? curChar : curHexHi );
            m_Content->Remove( data, m_Current, 1 );
            break;
        }

        default:
        {
            int keyCode = event.GetKeyCode();

            switch ( m_CurrentType )
            {
                case curHexHi:
                {
                    FileContentBase::ExtraUndoData data( m_Current, curHexHi, m_Current, curHexLo );

                    if ( keyCode >= '0' && keyCode <= '9' )
                    {
                        m_Content->WriteByte(
                            data,
                            m_Current,
                            ( m_Content->ReadByte( m_Current ) & 0xF ) |
                            ( keyCode - '0' ) << 4 );
                    }
                    else if ( keyCode >= 'A' && keyCode <= 'F' )
                    {
                        m_Content->WriteByte(
                            data,
                            m_Current,
                            ( m_Content->ReadByte( m_Current ) & 0xF ) |
                            ( keyCode - 'A' + 10 ) << 4 );
                    }
                    else if ( keyCode >= 'a' && keyCode <= 'f' )
                    {
                        m_Content->WriteByte(
                            data,
                            m_Current,
                            ( m_Content->ReadByte( m_Current ) & 0xF ) |
                            ( keyCode - 'a' + 10 ) << 4 );
                    }
                    else
                    {
                        return;
                    }
                    m_CurrentType = curHexLo;
                    break;
                }

                case curHexLo:
                {
                    FileContentBase::ExtraUndoData data( m_Current, curHexLo, m_Current, curHexHi );
                    if ( m_Current < m_Content->GetSize() - 1 )
                    {
                        data.m_PosAfter++;
                    }

                    if ( keyCode >= '0' && keyCode <= '9' )
                    {
                        m_Content->WriteByte(
                            data,
                            m_Current,
                            ( m_Content->ReadByte( m_Current ) & 0xF0 ) |
                            ( ( keyCode - '0' ) & 0xF ) );
                    }
                    else if ( keyCode >= 'A' && keyCode <= 'F' )
                    {
                        m_Content->WriteByte(
                            data,
                            m_Current,
                            ( m_Content->ReadByte( m_Current ) & 0xF0 ) |
                            ( ( keyCode - 'A' + 10 ) & 0xF ) );
                    }
                    else if ( keyCode >= 'a' && keyCode <= 'f' )
                    {
                        m_Content->WriteByte(
                            data,
                            m_Current,
                            ( m_Content->ReadByte( m_Current ) & 0xF0 ) |
                            ( ( keyCode - 'a' + 10 ) & 0xF ) );
                    }
                    else
                    {
                        return;
                    }
                    m_Current = data.m_PosAfter;
                    m_CurrentType = curHexHi;
                    break;
                }

                case curChar:
                {
                    if ( !wxIsprint( keyCode ) ) return;

                    FileContentBase::ExtraUndoData data( m_Current, curChar, m_Current, curChar );
                    if ( m_Current < m_Content->GetSize() - 1 )
                    {
                        data.m_PosAfter++;
                    }
                    m_Content->WriteByte( data, m_Current, (unsigned char)keyCode );
                    m_Current = data.m_PosAfter;
                    break;
                }
            }
        }
    }

    RefreshStatus();
    EnsureCarretVisible();
    m_DrawArea->Refresh();
    UpdateModified();
}

void HexEditPanel::EnsureCarretVisible()
{
    FileContentBase::OffsetT line = m_Current / m_Cols;
    FileContentBase::OffsetT startLine = DetectStartOffset() / m_Cols;
    FileContentBase::OffsetT endLine   = startLine + m_Lines;

    if ( line < startLine )
    {
        m_ContentScroll->SetThumbPosition( line );
        m_DrawArea->Refresh();
    }
    else if ( line >= endLine )
    {
        m_ContentScroll->SetThumbPosition( line - m_Lines + 1 );
        m_DrawArea->Refresh();
    }
}

void HexEditPanel::ClampCursorToVisibleArea()
{
    FileContentBase::OffsetT startOffs = DetectStartOffset();
    FileContentBase::OffsetT endOffs   = startOffs + m_Cols * m_Lines;

    if ( m_Current < startOffs )
    {
        m_Current = startOffs + ( m_Current % m_Cols );
    }
    else if ( m_Current >= endOffs )
    {
        m_Current = endOffs - m_Cols + ( m_Current % m_Cols );
    }

    if ( m_Current >= m_Content->GetSize() )
    {
        m_Current = m_Content->GetSize() - 1;
    }
}

void HexEditPanel::OnDrawAreaEraseBackground(wxEraseEvent& event)
{
}

void HexEditPanel::OnForwardFocus(wxFocusEvent& event)
{
    m_DrawArea->SetFocus();
}

bool HexEditPanel::Save()
{
    bool ret = m_Content->WriteFile( GetFilename() );
    UpdateModified();
    return ret;
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

void HexEditPanel::OnDrawAreaLeftDown(wxMouseEvent& event)
{
    if ( !m_Content ) return;

    m_DrawArea->SetFocus();

    // First we need to detect what the user has clicked on
    unsigned line = event.GetY() / m_FontY;

    // Just to prevent some weird situation
    if ( (int)line < 0 ) line = 0;
    if ( line >= m_Lines ) line = m_Lines-1;

    unsigned charpos = event.GetX() / m_FontX - 10;
    if ( (int)charpos < 0 ) charpos = 0;

    FileContentBase::OffsetT newCurrent = m_Current;
    CurrentType              newCurrentType = m_CurrentType;

    if ( charpos < m_Cols * 3 )
    {
        newCurrent = DetectStartOffset() + m_Cols * line + charpos / 3;
        newCurrentType = ( charpos%3 == 0 ) ? curHexHi : curHexLo;
        m_MouseDown = true;
    }
    else if ( charpos >= m_Cols*3+2 )
    {
        charpos -= m_Cols*3 + 2;
        if ( charpos >= m_Cols )
        {
            charpos = m_Cols-1;
        }
        newCurrent = DetectStartOffset() + m_Cols * line + charpos;
        newCurrentType = curChar;
        m_MouseDown = true;
    }

    if ( newCurrent >= m_Content->GetSize() )
    {
        return;
    }

    if ( newCurrent != m_Current || newCurrentType != m_CurrentType )
    {
        m_Current = newCurrent;
        m_CurrentType = newCurrentType;
        RefreshStatus();
        EnsureCarretVisible();
        m_DrawArea->Refresh();
        return;
    }
}

void HexEditPanel::OnDrawAreaLeftUp(wxMouseEvent& event)
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

void HexEditPanel::OnCheckBox1Click(wxCommandEvent& event)
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
            m_CurrentType = (CurrentType)extraData->m_PosTypeBefore;
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
            m_CurrentType = (CurrentType)extraData->m_PosTypeAfter;
        }
        RefreshStatus();
        EnsureCarretVisible();
        m_DrawArea->Refresh();
        UpdateModified();
    }
}

void HexEditPanel::OnSpecialKeyDown(wxKeyEvent& event)
{
    LogManager::Get()->DebugLog(
        F(
            _T("HexEditPanel::OnSpecialKeyDown: %d (%c%c%c)"),
            (int)event.GetKeyCode(),
            event.ControlDown() ? 'C':'c',
            event.AltDown() ? 'A':'a',
            event.CmdDown() ? 'M':'m' ) );

    if ( event.ControlDown() && !event.AltDown() )
    {
        switch ( event.GetKeyCode() )
        {
            case 'G': ProcessGoto(); return;
//            case 'F': ProcessSearch(); return;
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
        str = wxGetTextFromUser(
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
    RefreshStatus();
    EnsureCarretVisible();
    m_DrawArea->Refresh();
}

void HexEditPanel::OnButton1Click(wxCommandEvent& event)
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

void HexEditPanel::OnReparseTimerTrigger(wxTimerEvent& event)
{
    ReparseExpression();
    RefreshStatus();
}

void HexEditPanel::Onm_ExpressionText(wxCommandEvent& event)
{
    ReparseTimer.Start( 1000, wxTIMER_ONE_SHOT );
}

void HexEditPanel::OnButton2Click(wxCommandEvent& event)
{
    cbMessageBox( Expression::Parser::GetHelpString() );
}

void HexEditPanel::OnExpressionTextEnter(wxCommandEvent& event)
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
