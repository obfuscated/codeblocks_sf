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
}

//(*IdInit(SearchDialog)
const long SearchDialog::ID_COMBOBOX1 = wxNewId();
const long SearchDialog::ID_RADIOBUTTON1 = wxNewId();
const long SearchDialog::ID_RADIOBUTTON2 = wxNewId();
const long SearchDialog::ID_RADIOBUTTON3 = wxNewId();
const long SearchDialog::ID_BUTTON1 = wxNewId();
const long SearchDialog::ID_RADIOBOX2 = wxNewId();
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
	StaticBoxSizer1->Add(m_SearchValue, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	m_SearchTypeString = new wxRadioButton(this, ID_RADIOBUTTON1, _("String"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON1"));
	m_SearchTypeString->SetValue(true);
	BoxSizer2->Add(m_SearchTypeString, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_SearchTypeHex = new wxRadioButton(this, ID_RADIOBUTTON2, _("Hex"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON2"));
	BoxSizer2->Add(m_SearchTypeHex, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_SearchTypeExpression = new wxRadioButton(this, ID_RADIOBUTTON3, _("Expression"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON3"));
	BoxSizer2->Add(m_SearchTypeExpression, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer2->Add(13,8,1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button1 = new wxButton(this, ID_BUTTON1, _("\?"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT, wxDefaultValidator, _T("ID_BUTTON1"));
	BoxSizer2->Add(Button1, 0, wxRIGHT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(BoxSizer2, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(StaticBoxSizer1, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	wxString __wxRadioBoxChoices_1[2] =
	{
		_("Current position"),
		_("Beginning")
	};
	m_StartFrom = new wxRadioBox(this, ID_RADIOBOX2, _("Start from"), wxDefaultPosition, wxDefaultSize, 2, __wxRadioBoxChoices_1, 1, wxRA_HORIZONTAL, wxDefaultValidator, _T("ID_RADIOBOX2"));
	m_StartFrom->SetSelection(0);
	BoxSizer3->Add(m_StartFrom, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer3, 0, wxTOP|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	StdDialogButtonSizer1->Realize();
	BoxSizer1->Add(StdDialogButtonSizer1, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
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

	Connect( wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SearchDialog::OnOk) );
}

SearchDialog::~SearchDialog()
{
	ConfigManager* cfg = GetConfigManager();

	cfg->Write( CONF_GROUP _T("/origin"), m_StartFrom->GetSelection() );

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

void SearchDialog::OnOk(wxCommandEvent& event)
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

        int digitVal = wxString( _T("012345678ABCDEF") ).Find( wxToupper( *text ) );
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

    std::vector< unsigned char > buff( wxMax( 2*length, 0x10000 ) );

    wxProgressDialog dlg( _("Searching..."), _("Search in progress"), 1000, this, wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_CAN_ABORT );

    typedef FileContentBase::OffsetT OffsetT;

    OffsetT pos      = m_StartFrom->GetSelection()==0 ? m_Offset+1 : 0ULL;
    OffsetT left     = m_Content->GetSize() - pos;
    OffsetT buffFill = m_Content->Read( &buff[0], pos, wxMin( left, buff.size() ) );

    OffsetT startPos = pos;

    if ( !buffFill )
    {
        ReadError();
        return;
    }

    left -= buffFill;

    while ( buffFill >= length )
    {
        int offs = BlockCompare( &buff[0], buffFill, data, length );
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


        if ( !dlg.Update( (int)( (long double)(pos-startPos) / (long double)(m_Content->GetSize() -startPos )* 1000.0 ) ) )
        {
            Cancel();
            return;
        }
    }

    NotFound();
}

int SearchDialog::BlockCompare(const unsigned char* searchIn, size_t inLength, const unsigned char* searchFor, size_t forLength)
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
        if ( !memcmp( searchIn + 1, searchFor + 1, forLength - 1 ) )
        {
            return pos;
        }

        // No match, start searching from next character
        pos++;
        searchIn++;
        inLength--;
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

    OffsetT pos      = m_StartFrom->GetSelection()==0 ? m_Offset : 0ULL;
    OffsetT left     = m_Content->GetSize() - pos;
    OffsetT startPos = pos;

    for ( int i = 1; left; pos++, left-- )
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
            if ( !dlg.Update( (int)( (long double)(pos-startPos) / (long double)(m_Content->GetSize() -startPos )* 1000.0 ) ) )
            {
                Cancel();
                return;
            }
            i = 100;
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

void SearchDialog::OnButton1Click(wxCommandEvent& event)
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
