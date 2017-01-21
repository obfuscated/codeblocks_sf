/*
* This file is part of HexEditor plugin for Code::Blocks Studio
* Copyright (C) 2009 Bartlomiej Swiecki
*
* HexEditor plugin is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Frsee Software Foundation; either version 3 of the License, or
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

#include "SearchDialog.h"
#include "ExpressionParser.h"
#include "ExpressionExecutor.h"

//(*InternalHeaders(SearchDialog)
#include <wx/string.h>
#include <wx/intl.h>
//*)

#include <globals.h>
#include <vector>
#include <wx/progdlg.h>
#include <manager.h>
#include <configmanager.h>

#define CONF_NAME  _T("editor")
#define CONF_GROUP _T("/find_options")

namespace
{
    ConfigManager* GetConfigManager() { return Manager::Get()->GetConfigManager( CONF_NAME ); }


    #if defined ( __linux__ )  || defined ( LINUX )

        // Use native implementation
        inline const void* my_memrchr( const void* s, int c, size_t n )
        {
            return memrchr( s, c, n );
        }

    #else

        // Custom implementation, may be much slower
        inline const void* my_memrchr( const void* _s, int c, size_t n )
        {
            const char* s = (const char*)_s;
            for ( size_t i=n; i-->0; )
            {
                if ( s[ i ] == c ) return s+i;
            }
            return 0;
        }

    #endif
}

//(*IdInit(SearchDialog)
const long SearchDialog::ID_COMBOBOX1 = wxNewId();
const long SearchDialog::ID_RADIOBUTTON1 = wxNewId();
const long SearchDialog::ID_RADIOBUTTON2 = wxNewId();
const long SearchDialog::ID_RADIOBUTTON3 = wxNewId();
const long SearchDialog::ID_BUTTON1 = wxNewId();
const long SearchDialog::ID_RADIOBOX2 = wxNewId();
const long SearchDialog::ID_RADIOBOX1 = wxNewId();
//*)

SearchDialog::SearchDialog( wxWindow* parent, FileContentBase* content, FileContentBase::OffsetT current ): m_Content( content ), m_Offset( current )
{
	BuildContent(parent);
}

void SearchDialog::BuildContent(wxWindow* parent)
{
	//(*Initialize(SearchDialog)
	wxBoxSizer* BoxSizer3;
	wxBoxSizer* BoxSizer2;
	wxBoxSizer* BoxSizer1;
	wxStdDialogButtonSizer* StdDialogButtonSizer1;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, wxID_ANY, _("Search..."), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Search for"));
	m_SearchValue = new wxComboBox(this, ID_COMBOBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX1"));
	StaticBoxSizer1->Add(m_SearchValue, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	m_SearchTypeString = new wxRadioButton(this, ID_RADIOBUTTON1, _("String"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON1"));
	m_SearchTypeString->SetValue(true);
	BoxSizer2->Add(m_SearchTypeString, 0, wxALL|wxEXPAND, 5);
	m_SearchTypeHex = new wxRadioButton(this, ID_RADIOBUTTON2, _("Hex"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON2"));
	BoxSizer2->Add(m_SearchTypeHex, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	m_SearchTypeExpression = new wxRadioButton(this, ID_RADIOBUTTON3, _("Expression"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON3"));
	BoxSizer2->Add(m_SearchTypeExpression, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer2->Add(13,8,1, wxALIGN_CENTER_VERTICAL, 5);
	Button1 = new wxButton(this, ID_BUTTON1, _("\?"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON1"));
	BoxSizer2->Add(Button1, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(BoxSizer2, 0, wxEXPAND, 5);
	BoxSizer1->Add(StaticBoxSizer1, 0, wxALL|wxEXPAND, 5);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	wxString __wxRadioBoxChoices_1[2] =
	{
		_("Current position"),
		_("Beginning")
	};
	m_StartFrom = new wxRadioBox(this, ID_RADIOBOX2, _("Start from"), wxDefaultPosition, wxDefaultSize, 2, __wxRadioBoxChoices_1, 1, wxRA_HORIZONTAL, wxDefaultValidator, _T("ID_RADIOBOX2"));
	m_StartFrom->SetSelection(0);
	BoxSizer3->Add(m_StartFrom, 1, wxALL|wxEXPAND, 5);
	wxString __wxRadioBoxChoices_2[2] =
	{
		_("Up"),
		_("Down")
	};
	m_Direction = new wxRadioBox(this, ID_RADIOBOX1, _("Direction"), wxDefaultPosition, wxDefaultSize, 2, __wxRadioBoxChoices_2, 1, wxRA_HORIZONTAL, wxDefaultValidator, _T("ID_RADIOBOX1"));
	m_Direction->SetSelection(1);
	BoxSizer3->Add(m_Direction, 1, wxALL|wxEXPAND, 5);
	BoxSizer1->Add(BoxSizer3, 0, wxTOP|wxEXPAND, 5);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	StdDialogButtonSizer1->Realize();
	BoxSizer1->Add(StdDialogButtonSizer1, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);

	Connect(ID_COMBOBOX1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&SearchDialog::OnOk);
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SearchDialog::OnButton1Click);
	//*)

	ConfigManager* cfg = GetConfigManager();

    m_SearchValue->SetFocus();

    wxArrayString last = cfg->ReadArrayString( CONF_GROUP _T("/last") );
	for ( size_t i = 0; i < last.GetCount(); ++i )
	{
		if ( !last[i].IsEmpty())
		{
		    m_SearchValue->Append( last[i] );
        }
	}
	m_SearchValue->SetSelection( 0 );
	m_StartFrom->SetSelection( cfg->ReadInt( CONF_GROUP _T("/origin") ) );
	m_Direction->SetSelection( cfg->ReadInt( CONF_GROUP _T("/direction") ) );

	int type = cfg->ReadInt( CONF_GROUP _T("/hexedit/type") );
	m_SearchTypeString    ->SetValue( type==0 );
	m_SearchTypeHex       ->SetValue( type==1 );
	m_SearchTypeExpression->SetValue( type==2 );

	Connect( wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SearchDialog::OnOk) );

    // Limit vertical resizing.
    SetMaxSize(wxSize(-1, GetMinHeight()));
}

SearchDialog::~SearchDialog()
{
	ConfigManager* cfg = GetConfigManager();

	cfg->Write( CONF_GROUP _T("/origin"),    (int)m_StartFrom->GetSelection() );
	cfg->Write( CONF_GROUP _T("/direction"), (int)m_Direction->GetSelection() );

	int type =
        m_SearchTypeString->GetValue() ? 0 :
        m_SearchTypeHex   ->GetValue() ? 1 : 2;

    cfg->Write( CONF_GROUP _T("/hexedit/type"), type );

	wxString value = m_SearchValue->GetValue();

	wxArrayString last = cfg->ReadArrayString( CONF_GROUP _T("/last") );
    int lastPos = last.Index( value );
    if ( lastPos != wxNOT_FOUND )
    {
        last.RemoveAt( lastPos );
    }
    last.Insert( value, 0 );

    cfg->Write( CONF_GROUP _T("/last"), last );

	//(*Destroy(SearchDialog)
	//*)
}

void SearchDialog::OnOk(wxCommandEvent& /*event*/)
{
    if ( m_SearchTypeString->GetValue() )
    {
        SearchAscii( cbU2C( m_SearchValue->GetValue() ) );
    }
    else if ( m_SearchTypeHex->GetValue() )
    {
        SearchHex( m_SearchValue->GetValue().c_str() );
    }
    else if ( m_SearchTypeExpression->GetValue() )
    {
        SearchExpression( m_SearchValue->GetValue() );
    }
}

void SearchDialog::SearchAscii(const char* text)
{
    if ( !*text )
    {
        cbMessageBox( _("Search string is empty") );
        return;
    }
    SearchBuffer( (const unsigned char*)text, strlen( text ) );
}

void SearchDialog::SearchHex( const wxChar* text )
{
    std::vector< unsigned char > buff;

    bool hiDigit = true;
    unsigned char val = 0;

    while ( *text )
    {
        if ( wxIsspace( *text ) )
        {
            text++;

            if ( !hiDigit )
            {
                buff.push_back( val );
                val = 0;
                hiDigit = true;
            }
            continue;
        }

        int digitVal = wxString( _T("0123456789ABCDEF") ).Find( wxToupper( *text ) );
        if ( digitVal < 0 || digitVal >= 0x10 )
        {
            cbMessageBox( _("Invalid hex string, allowed characters are: hex digits and spaces"), _("Invalid hex string") );
            return;
        }

        val <<= 4;
        val |= (unsigned char)digitVal;

        hiDigit = !hiDigit;

        if ( hiDigit )
        {
            buff.push_back( val );
            val = 0;
        }
        text++;
    }

    if ( !hiDigit )
    {
        buff.push_back( val );
    }

    if ( buff.empty() )
    {
        cbMessageBox( _("Search string is empty") );
        return;
    }

    SearchBuffer( &buff[0], buff.size() );
}

void SearchDialog::SearchBuffer(const unsigned char* data, size_t length)
{
    assert( length > 0 );

    if ( m_Content->GetSize() < length )
    {
        NotFound();
        return;
    }

    bool backwards = m_Direction->GetSelection() == 0;
    bool fromStart = m_StartFrom->GetSelection() != 0;

    std::vector< unsigned char > buff( wxMax( 2*length, 0x10000 ) );

    wxProgressDialog dlg( _("Searching..."), _("Search in progress"), 1000, this, wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_CAN_ABORT );

    typedef FileContentBase::OffsetT OffsetT;

    if ( backwards )
    {
        OffsetT pos      = fromStart ? m_Content->GetSize() : m_Offset + length - 1;
        pos              = wxMin( pos, m_Content->GetSize() );
        OffsetT buffFill = wxMin( pos, buff.size() );
        OffsetT left     = pos;
        buffFill         = m_Content->Read( &buff[0], pos - buffFill, buffFill );
        OffsetT startPos = pos;

        if ( !buffFill )
        {
            ReadError();
            return;
        }

        pos  -= buffFill;
        left -= buffFill;

        while ( buffFill >= length )
        {
            int offs = BlockCompare( &buff[0], buffFill, data, length, true );
            if ( offs >= 0 )
            {
                FoundAt( pos + offs );
                return;
            }

            if ( !left ) break;

            size_t shift = buffFill - length + 1;
            shift = wxMin( shift, left );

            memmove( &buff[ shift ], &buff[ 0 ], length-1 );

            OffsetT nowRead = m_Content->Read( &buff[ 0 ], pos - shift, shift );
            if ( nowRead < shift )
            {
                ReadError();
                return;
            }

            pos  -= shift;
            left -= shift;

            if ( !dlg.Update( (int)( (long double)(startPos-pos) / (long double)(startPos ) * 1000.0 ) ) )
            {
                Cancel();
                return;
            }
        }

    }
    else
    {
        OffsetT pos      = fromStart ? 0ULL : m_Offset+1;
        OffsetT left     = m_Content->GetSize() - pos;
        OffsetT buffFill = m_Content->Read( &buff[0], pos, wxMin( left, buff.size() ) );

        OffsetT startPos = pos;

        if ( !left )
        {
            NotFound();
            return;
        }

        if ( !buffFill )
        {
            ReadError();
            return;
        }

        left -= buffFill;

        while ( buffFill >= length )
        {
            int offs = BlockCompare( &buff[0], buffFill, data, length, false );
            if ( offs >= 0 )
            {
                FoundAt( pos + offs );
                return;
            }

            if ( !left ) break;

            size_t shift = buffFill - length + 1;

            memmove( &buff[0], &buff[ shift ], length-1 );
            buffFill -= shift;
            pos      += shift;


            OffsetT nowRead = m_Content->Read( &buff[ length-1 ], pos + length - 1, wxMin( left, buff.size() - length + 1 ) );
            if ( !nowRead )
            {
                ReadError();
                return;
            }

            buffFill += nowRead;
            left     -= nowRead;


            if ( !dlg.Update( (int)( (long double)(pos-startPos) / (long double)(m_Content->GetSize() - startPos )* 1000.0 ) ) )
            {
                Cancel();
                return;
            }
        }
    }

    NotFound();
}

int SearchDialog::BlockCompare(const unsigned char* searchIn, size_t inLength, const unsigned char* searchFor, size_t forLength, bool backwards )
{
    if ( !backwards )
    {
        int pos = 0;
        while ( inLength >= forLength )
        {
            const unsigned char* firstCharPosition = ( const unsigned char* ) memchr( searchIn, *searchFor, inLength - forLength + 1 );
            if ( !firstCharPosition ) return -1;

            pos      += firstCharPosition - searchIn;
            inLength -= firstCharPosition - searchIn;
            searchIn  = firstCharPosition;


            // First char matches, search for next ones
            assert( inLength >= forLength );
            if ( forLength <= 1 || !memcmp( searchIn + 1, searchFor + 1, forLength - 1 ) )
            {
                return pos;
            }

            // No match, start searching from next character
            pos++;
            searchIn++;
            inLength--;
        }
    }
    else
    {
        int pos = inLength - forLength;
        while ( pos >= 0 )
        {
            const unsigned char* firstCharPosition = ( const unsigned char* ) my_memrchr( searchIn, *searchFor, pos + 1 );
            if ( !firstCharPosition ) return -1;

            pos = firstCharPosition - searchIn;

            // First char matches, search for next ones
            assert( pos >= 0 );
            if ( forLength <= 1 || !memcmp( searchIn + pos + 1, searchFor + 1, forLength - 1 ) )
            {
                return pos;
            }

            pos--;
        }
    }

    return -1;
}

void SearchDialog::SearchExpression(const wxString& expression)
{
    Expression::Parser       parser;
    Expression::Preprocessed preprocessed;
    Expression::Executor executor;

    if ( !parser.Parse( expression, preprocessed ) )
    {
        int pos;
        cbMessageBox( _("Invalid expression:\n" ) + parser.ParseErrorDesc(pos) );
        return;
    }

    wxProgressDialog dlg( _("Searching..."), _("Search in progress"), 1000, this, wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_CAN_ABORT );

    typedef FileContentBase::OffsetT OffsetT;

    bool backwards = m_Direction->GetSelection() == 0;
    bool fromStart = m_StartFrom->GetSelection() != 0;


    OffsetT pos;
    OffsetT size;
    OffsetT cnt = 0;
    signed char posStep;


    if ( backwards )
    {
        pos     = fromStart ? m_Content->GetSize() : m_Offset;
        size    = pos;
        posStep = -1;
        pos--;
    }
    else
    {
        pos     = fromStart ? 0ULL : m_Offset + 1;
        size    = m_Content->GetSize() - pos;
        posStep = 1;
    }

    for ( int i = 1; cnt < size; pos += posStep, cnt++ )
    {
        if ( executor.Execute( preprocessed, m_Content, pos ) )
        {
            unsigned long long ull;
            long long ll;
            long double ld;

            if ( ( executor.GetResult( ull ) && !ull ) ||
                 ( executor.GetResult( ll  ) && !ll  ) ||
                 ( executor.GetResult( ld  ) && !ld  ) )
            {
                FoundAt( pos );
                return;
            }
        }

        if ( !--i )
        {
            if ( !dlg.Update( (int)( (long double)(cnt) / (long double)(size) * 1000.0 ) ) )
            {
                Cancel();
                return;
            }
            i = 0x1000;
        }
    }

    NotFound();
}

void SearchDialog::NotFound()
{
    cbMessageBox( _( "Couldn't find requested data" ), _( "Search failure" ), wxOK, this );
    EndModal( wxID_CANCEL );
}

void SearchDialog::FoundAt(FileContentBase::OffsetT pos)
{
    m_Offset = pos;
    EndModal( wxID_OK );
}

void SearchDialog::ReadError()
{
    cbMessageBox( _( "Error occurred while searching for data.\nFile may be corrupted.\nPlease backup your data." ), _( "Search error" ), wxOK, this );
    EndModal( wxID_CANCEL );
}

void SearchDialog::Cancel()
{
    EndModal( wxID_CANCEL );
}

void SearchDialog::OnButton1Click(wxCommandEvent& /*event*/)
{
    cbMessageBox(
        _( "Search for string:\n"
           "\tValue is UTF8 string\n"
           "Search for hex:\n"
           "\tValue is sequence of 2-digit hexadecimal numbers,\n"
           "\tspace splits numbers, after sequence of each 2 digits\n"
           "\tautomatic break is added (like there was a space)\n"
           "\texample:\n"
           "\t\t12 34 5 678 9ABCD is the same as:\n"
           "\t\t12 34 05 67 08 9A BC 0D\n"
           "Search for expression:\n"
           "\tCan use same expression as in preview or calculator,\n"
           "\tgiven position is 'found' when expression at this\n"
           "\tposition is equal to zero.\n" ) );
}
