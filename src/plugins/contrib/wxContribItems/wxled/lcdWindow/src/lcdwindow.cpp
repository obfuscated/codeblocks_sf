#include "wx/lcdwindow.h"

#define LCD_NUMBER_SEGMENTS 8


BEGIN_EVENT_TABLE( wxLCDWindow, wxWindow )
	EVT_PAINT( wxLCDWindow::OnPaint )
	EVT_SIZE( wxLCDWindow::OnSize )
END_EVENT_TABLE()


wxLCDWindow::wxLCDWindow( wxWindow *parent, wxPoint pos, wxSize size ) : wxWindow( parent, -1, pos, size, wxSUNKEN_BORDER | wxFULL_REPAINT_ON_RESIZE )
{
	mSegmentLen = 40;
	mSegmentWidth = 10;
	mSpace = 5;

	mNumberDigits = 6;

	mLightColour = wxColour( 0, 255, 0 );
	mGrayColour = wxColour( 0, 64, 0 );

	SetBackgroundColour( wxColour( 0, 0, 0 ) );
}


wxLCDWindow::~wxLCDWindow()
{

}


void wxLCDWindow::OnPaint( wxPaintEvent &event )
{
	wxPaintDC dc( this );

	int dw = GetClientSize().GetWidth();
	int dh = GetClientSize().GetHeight();

	int bw = GetBitmapWidth();
	int bh = GetBitmapHeight();

	double xs = ( double ) dw / bw;
	double ys = ( double ) dh / bh;

	double as = xs > ys? ys : xs;

	dc.SetUserScale( as, as );
	dc.SetDeviceOrigin( ( ( dw - bw * as ) / 2 ), ( ( dh - bh * as ) / 2 ) );
//	dc.BeginDrawing();

	DoDrawing( &dc );

//	dc.EndDrawing();

	return;
}


void wxLCDWindow::OnSize( wxSizeEvent &event )
{
	event.Skip();
	return ;
}


void wxLCDWindow::DoDrawing( wxDC *dc )
{
	wxString buf;
	buf.Printf( wxT("%s"), mValue.c_str() );
	while( buf.Replace( wxT(".."), wxT(". .") ) );

	char current;
	char next;
	int buflen = buf.Len();
	int ac = buflen - 1;

	for( int c = 0; c < mNumberDigits; c++ )
	{
ReadString:
		ac >= 0? current = buf.GetChar( ac ): current = wxT(' ');
		ac >= 0 && ac < buflen - 1? next = buf.GetChar( ac + 1 ): next = wxT(' ');

		if( current == wxT('.') )
		{
			ac--;
			goto ReadString;
		}

		wxDigitData *data = new wxDigitData;

		data->value = current;
		data->comma = false;

		if( next == wxT('.') )
		{
			data->comma = true;
		}

		DrawDigit( dc, c, data  );
		ac--;

		delete data;
	}

	return ;
}


void wxLCDWindow::DrawDigit( wxDC *dc, int digit, wxDigitData *data )
{
	unsigned char dec = Decode( data->value );

	if( data->value == wxT(':') )
	{
		DrawTwoDots( dc, digit );

		return ;
	}

	for( int c = 0; c < LCD_NUMBER_SEGMENTS - 1; c++ )
	{
		DrawSegment( dc, digit, c, ( dec >> c ) & 1 );
	}

	DrawSegment( dc, digit, 7, data->comma );

	return ;
}


void wxLCDWindow::DrawTwoDots( wxDC *dc, int digit )
{
	int sl = mSegmentLen;
	int sw = mSegmentWidth;
//	int sp = mSpace;

	int x = DigitX( digit );
	int y = DigitY( digit );

	wxBrush brushOn( mLightColour, wxSOLID );

	x += ( sl / 2 ) - sw;
	y += ( sl / 2 ) - sw;

	dc->SetBrush( brushOn );
	dc->SetPen( wxPen( GetBackgroundColour(), 1, wxSOLID ) );

	dc->DrawEllipse( x, y, 2 * sw, 2 * sw );

	y += sl;

	dc->DrawEllipse( x, y, 2 * sw, 2 * sw );

	return ;
}


void wxLCDWindow::DrawSegment( wxDC *dc, int digit, int segment, bool state )
{
	int sl = mSegmentLen;
	int sw = mSegmentWidth;
//	int sp = mSpace;

	int x = DigitX( digit );
	int y = DigitY( digit );

	wxBrush brushOn( mLightColour, wxSOLID );
	wxBrush brushOff( mGrayColour, wxSOLID );

	if( state )
	{
		dc->SetBrush( brushOn );
	}
	else
	{
		dc->SetBrush( brushOff );
	}

	dc->SetPen( wxPen( GetBackgroundColour(), 1, wxSOLID ) );

	wxPoint points[4];

	switch( segment )
	{
	case 0:
		points[0].x = x;
		points[0].y = y;
		points[1].x = x + sl;
		points[1].y = y;
		points[2].x = x + sl - sw;
		points[2].y = y + sw;
		points[3].x = x + sw;
		points[3].y = y + sw;
		break;
	case 5:
		y += 2 * sl - sw;
		points[0].x = x + sw;
		points[0].y = y;
		points[1].x = x + sl - sw;
		points[1].y = y;
		points[2].x = x + sl;
		points[2].y = y + sw;
		points[3].x = x;
		points[3].y = y + sw;
		break;
	case 4:
		y += sl;
		x += sl - sw;
		points[0].x = x;
		points[0].y = y + sw / 2;
		points[1].x = x + sw;
		points[1].y = y;
		points[2].x = x + sw;
		points[2].y = y + sl;
		points[3].x = x;
		points[3].y = y + sl - sw;
		break;
	case 2:
		x += sl - sw;
		points[0].x = x;
		points[0].y = y + sw;
		points[1].x = x + sw;
		points[1].y = y;
		points[2].x = x + sw;
		points[2].y = y + sl;
		points[3].x = x;
		points[3].y = y + sl - sw / 2;
		break;
	case 3:
		y += sl;
		points[0].x = x;
		points[0].y = y;
		points[1].x = x;
		points[1].y = y + sl;
		points[2].x = x + sw;
		points[2].y = y + sl - sw;
		points[3].x = x + sw;
		points[3].y = y + sw - sw / 2;
		break;
	case 1:
		points[0].x = x;
		points[0].y = y;
		points[1].x = x;
		points[1].y = y + sl;
		points[2].x = x + sw;
		points[2].y = y + sl - sw / 2;
		points[3].x = x + sw;
		points[3].y = y + sw;
		break;
	case 6:
	default:
		break;
	}

	if( segment < 6 )
	{
		dc->DrawPolygon( 4, points );
	}

	if( segment == 6 )
	{
		y += sl - sw / 2;
		wxPoint p6[6];

		p6[0].x = x;
		p6[0].y = y + sw / 2;
		p6[1].x = x + sw;
		p6[1].y = y;
		p6[2].x = x + sl - sw;
		p6[2].y = y;
		p6[3].x = x + sl;
		p6[3].y = y + sw / 2;
		p6[4].x = x + sl - sw;
		p6[4].y = y + sw;
		p6[5].x = x + sw;
		p6[5].y = y + sw;

		dc->DrawPolygon( 6, p6 );
	}

	if( segment == 7 )
	{
		y += 2 * sl;
		x += sl;

		dc->DrawEllipse( x + 1, y - sw, sw, sw );
	}

	return ;
}


// Protected functions that calculate sizes.
// Needed by OnPaint

int wxLCDWindow::GetDigitWidth( void )
{
	return mSegmentLen + mSegmentWidth + mSpace;
}


int wxLCDWindow::GetDigitHeight( void )
{
	return ( 2 * mSegmentLen ) + ( 2 * mSpace );
}


int wxLCDWindow::GetBitmapWidth( void )
{
	return ( mNumberDigits * GetDigitWidth() ) + mSpace;
}


int wxLCDWindow::GetBitmapHeight( void )
{
	return GetDigitHeight();
}


int wxLCDWindow::DigitX( int digit )
{
	return GetBitmapWidth() - ( ( digit + 1 ) * GetDigitWidth() );
}


int wxLCDWindow::DigitY( int digit )
{
	return mSpace;
}


// Public functions accessible by the user.

void wxLCDWindow::SetNumberDigits( int ndigits )
{
	mNumberDigits = ndigits;
	Refresh( false );

	return ;
}


void wxLCDWindow::SetValue( wxString value )
{
	mValue = value;

	Refresh( false );

	return ;
}


wxString wxLCDWindow::GetValue( void )
{
	return mValue;
}


int wxLCDWindow::GetNumberDigits( void )
{
	return mNumberDigits;
}


void wxLCDWindow::SetLightColour( wxColour c )
{
	mLightColour = c;

	return ;
}


void wxLCDWindow::SetGrayColour( wxColour c )
{
	mGrayColour = c;

	return ;
}


wxColour wxLCDWindow::GetLightColour( void )
{
	return mLightColour;
}


wxColour wxLCDWindow::GetGrayColour( void )
{
	return mGrayColour;
}


int wxLCDWindow::GetDigitsNeeded( wxString value )
{
	wxString tst = value;

	while( tst.Replace( wxT("."), wxT("") ) );

	return tst.Len();
}


// The decoder function. The heart of the wxLCDWindow.


//      ***0***
//     *       *
//     1       2
//     *       *
//      ***6***
//     *       *
//     3       4
//     *       *
//      ***5***

// A 10
// B 11
// C 12
// D 13
// E 14
// F 15

//     8421 8421
//     -654 3210
//---------------------
// 0 : 0011.1111 = 0x3F
// 1 : 0001.0100 = 0x14
// 2 : 0110.1101 = 0x6D
// 3 : 0111.0101 = 0x75
// 4 : 0101.0110 = 0x56
// 5 : 0111.0011 = 0x73
// 6 : 0111.1011 = 0x7B
// 7 : 0001,0101 = 0x15
// 8 : 0111.1111 = 0x7F
// 9 : 0111.0111 = 0x77
//   : 0000.0000 = 0x00
// - : 0100.0000 = 0x40
// E : 0110.1011 = 0x6B
// r : 0100.1000 = 0x48
// o : 0111.1000 = 0x78
// ^ : 0100.0111 = 0x47
// C : 0010.1011 = 0x2B


unsigned char wxLCDWindow::Decode( char c )
{
	unsigned char ret = 0;

	struct DecodedDisplay
	{
		char ch;
		unsigned char value;
	};

	DecodedDisplay dec[] =
	{
		{ wxT('0'), 0x3F },
		{ wxT('1'), 0x14 },
		{ wxT('2'), 0x6D },
		{ wxT('3'), 0x75 },
		{ wxT('4'), 0x56 },
		{ wxT('5'), 0x73 },
		{ wxT('6'), 0x7B },
		{ wxT('7'), 0x15 },
		{ wxT('8'), 0x7F },
		{ wxT('9'), 0x77 },
		{ wxT(' '), 0x00 },
		{ wxT('-'), 0x40 },
		{ wxT('E'), 0x6B },
		{ wxT('r'), 0x48 },
		{ wxT('o'), 0x78 },
		{ wxT('^'), 0x47 },
		{ wxT('C'), 0x2B },
		{ 0, 0 }
	};

	for( int d = 0; dec[d].ch != 0; d++ )
	{
		if( dec[d].ch == c )
		{
			ret = dec[d].value;
			break;
		}
	}

	return ret;
}
