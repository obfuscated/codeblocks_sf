// Scintilla source code edit control
// PlatWX.cxx - implementation of platform facilities on wxWidgets
// Copyright 1998-1999 by Neil Hodgson <neilh@scintilla.org>
//                        Robin Dunn <robin@aldunn.com>
// The License.txt file describes the conditions under which this software may be distributed.

#include "PlatWX.h"
#include "Scintilla.h"
#include "FontQuality.h"

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include <wx/menu.h>
    #include <wx/dcmemory.h>
    #include <wx/settings.h>
#endif // WX_PRECOMP

#if wxUSE_DISPLAY
    #include <wx/display.h>
#endif
#if defined(wxHAS_RAW_BITMAP) || defined(wxHAVE_RAW_BITMAP)
    #include <wx/rawbmp.h>
#endif

#include <wx/dynlib.h>
#include <wx/encconv.h>
#include <wx/listctrl.h>
#include <wx/msgdlg.h>
#include <wx/mstream.h>
#include <wx/image.h>
#include <wx/imaglist.h>
#include <wx/tokenzr.h>

#include <ctype.h>

#include "wx/wxscintilla.h"

#ifdef __WXMSW__
    #include <windows.h> // for GetDoubleClickTime
    #include <wx/msw/winundef.h>
#endif

#ifdef SCI_NAMESPACE
using namespace Scintilla;
namespace Scintilla
{
#endif

//----------------------------------------------------------------------

Font::Font() : fid(0), ascent(0) {}

Font::~Font()
{
    delete reinterpret_cast<wxFont *>(fid);
    fid = 0;
}

void Font::Create(const FontParameters &fp) {
    Release();

    // The minus one is done because since Scintilla uses SC_CHARSET_DEFAULT
    // internally and we need to have wxFONENCODING_DEFAULT == SC_CHARSET_DEFAULT
    // so we adjust the encoding before passing it to Scintilla.  See also
    // wxScintilla::StyleSetCharacterSet
    wxFontEncoding encoding = (wxFontEncoding)(fp.characterSet-1);

    wxFontEncodingArray ea = wxEncodingConverter::GetPlatformEquivalents(encoding);
    if (ea.GetCount())
        encoding = ea[0];

/* C::B begin */
    wxFont* font = new wxFont(fp.size,
                    wxDEFAULT,
                    fp.italic                  ? wxITALIC : wxNORMAL,
                    fp.weight < SC_WEIGHT_BOLD ? wxNORMAL : wxBOLD,
                    false,
                    sci2wx(fp.faceName),
                    encoding);
/* C::B end */
    fid = font;
}


void Font::Release()
{
    if (fid)
        delete (wxFont*)fid;

    fid = 0;
}


SurfaceImpl::SurfaceImpl() :
    hDC(0), bitmap(0), hDCOwned(0), x(0), y(0),
    unicodeMode(0)
{}

SurfaceImpl::~SurfaceImpl()
{
    Release();
}

void SurfaceImpl::Init(WindowID wid)
{
#if 0
    Release();
    hDC = new wxMemoryDC();
    hDCOwned = true;
#else
    // On Mac and GTK the DC is not really valid until it has a bitmap
    // selected into it.  So instead of just creating the DC with no bitmap,
    // go ahead and give it one.
    InitPixMap(1, 1, NULL, wid);
#endif
}

void SurfaceImpl::Init(SurfaceID hDC_, WindowID /*wid*/)
{
    Release();
    hDC = static_cast<wxDC*>(hDC_);
}

void SurfaceImpl::InitPixMap(int width,
    int height,
    Surface * /*surface_*/,
    WindowID /*wid*/)
{
    Release();
    if (width < 1) width = 1;
    if (height < 1) height = 1;
    hDCOwned = true;
    hDC = new wxMemoryDC();
    bitmap = new wxBitmap(width, height);
    (static_cast<wxMemoryDC*>(hDC))->SelectObject(*bitmap);
}


void SurfaceImpl::Release()
{
    if (bitmap) {
        (static_cast<wxMemoryDC*>(hDC))->SelectObject(wxNullBitmap);
        delete bitmap;
    }
    if (hDCOwned && hDC) {
        delete hDC;
    }

    hDC = 0;
    bitmap = 0;
    hDCOwned = false;
}

bool SurfaceImpl::Initialised()
{
    return hDC != 0;
}


void SurfaceImpl::PenColour(ColourDesired fore)
{
/* C::B begin */
    if( fore.AsLong() == -1 ) {
        hDC->SetPen( *wxTRANSPARENT_PEN );
    } else {
        hDC->SetPen(wxPen(wxColourFromCD(fore)));
    }
/* C::B end */
}

void SurfaceImpl::BrushColour(ColourDesired back)
{
/* C::B begin */
    if( back.AsLong() == -1 ) {
        hDC->SetBrush( *wxTRANSPARENT_BRUSH );
    } else {
        hDC->SetBrush(wxBrush(wxColourFromCD(back)));
    }
/* C::B end */
}

void SurfaceImpl::SetFont(Font &font_)
{
    if (font_.GetID()) {
        hDC->SetFont(*(reinterpret_cast<wxFont*>(font_.GetID())));
    }
}

int SurfaceImpl::LogPixelsY()
{
    return hDC->GetPPI().y;
}

int SurfaceImpl::DeviceHeightFont(int points)
{
    return points;
}

void SurfaceImpl::MoveTo(int x_, int y_)
{
    x = x_;
    y = y_;
}

void SurfaceImpl::LineTo(int x_, int y_)
{
    hDC->DrawLine(x, y, x_,y_);
    x = x_;
    y = y_;
}

void SurfaceImpl::Polygon(Point *pts,
    int npts,
    ColourDesired fore,
    ColourDesired back)
{
    PenColour(fore);
    BrushColour(back);
/* C::B begin */
    // Since Point now has float as x and y, it seems no longer to be interchangeable with wxPoint,
    // and the polygon is not drawn, so we convert it explicitely
    wxPoint points[npts];
    for (int i = 0; i < npts; ++i)
        points[i] = wxPoint(pts[i].x, pts[i].y);
    hDC->DrawPolygon(npts, points);
/* C::B end */
}

void SurfaceImpl::RectangleDraw(PRectangle rc,
    ColourDesired fore,
    ColourDesired back)
{
    PenColour(fore);
    BrushColour(back);
    hDC->DrawRectangle(wxRectFromPRect(rc));
}

void SurfaceImpl::FillRectangle(PRectangle rc, ColourDesired back)
{
    BrushColour(back);
    hDC->SetPen(*wxTRANSPARENT_PEN);
    hDC->DrawRectangle(wxRectFromPRect(rc));
}

void SurfaceImpl::FillRectangle(PRectangle rc, Surface &surfacePattern)
{
    SurfaceImpl *surface = static_cast<SurfaceImpl *>(&surfacePattern);
    wxBrush br;
    if (surface->bitmap)
        br = wxBrush(*(surface->bitmap));
    else    // Something is wrong so display in red
        br = wxBrush(*wxRED);
    hDC->SetPen(*wxTRANSPARENT_PEN);
    hDC->SetBrush(br);
    hDC->DrawRectangle(wxRectFromPRect(rc));
}

void SurfaceImpl::RoundedRectangle(PRectangle rc,
    ColourDesired fore,
    ColourDesired back)
{
    PenColour(fore);
    BrushColour(back);
    hDC->DrawRoundedRectangle(wxRectFromPRect(rc), 4);
}

#ifdef __WXMSW__
// On Windows, wxBitmap contains rgb values pre-multiplied with alpha
#define wxPy_premultiply(p, a)   ((p) * (a) / 0xff)
#else
#define wxPy_premultiply(p, a)   (p)
#endif

void SurfaceImpl::AlphaRectangle(PRectangle rc,
                                 int corner,
                                 ColourDesired fill,
                                 int alphaFill,
                                 ColourDesired outline,
                                 int alphaOutline,
                                 int flags)
{
#if defined(wxHAS_RAW_BITMAP) || defined(wxHAVE_RAW_BITMAP)

    // TODO:  do something with cornerSize
    wxUnusedVar(corner);
    wxUnusedVar(flags);

    wxRect r = wxRectFromPRect(rc);
    wxBitmap bmp(r.width, r.height, 32);
/* C::B begin */
    if (!bmp.IsOk())
        return;
/* C::B end */
    wxAlphaPixelData pixData(bmp);
/* C::B begin */
    #if !wxCHECK_VERSION(2, 9, 0)
    pixData.UseAlpha(); // wx/rawbmp.h:669 - Call can simply be removed.
    #endif
/* C::B end */

    // Set the fill pixels
    ColourDesired cdf(fill.AsLong());
    int red   = cdf.GetRed();
    int green = cdf.GetGreen();
    int blue  = cdf.GetBlue();

    wxAlphaPixelData::Iterator p(pixData);
    for (y=0; y<r.height; y++) {
        p.MoveTo(pixData, 0, y);
        for (x=0; x<r.width; x++) {
            p.Red()   = wxPy_premultiply(red,   alphaFill);
            p.Green() = wxPy_premultiply(green, alphaFill);
            p.Blue()  = wxPy_premultiply(blue,  alphaFill);
            p.Alpha() = alphaFill;
            ++p;
        }
    }

    // Set the outline pixels
    ColourDesired cdo(outline.AsLong());
    red   = cdo.GetRed();
    green = cdo.GetGreen();
    blue  = cdo.GetBlue();

    for (x=0; x<r.width; x++) {
        p.MoveTo(pixData, x, 0);
/* C::B begin */
        if (p.m_ptr) {
            p.Red()   = wxPy_premultiply(red,   alphaOutline);
            p.Green() = wxPy_premultiply(green, alphaOutline);
            p.Blue()  = wxPy_premultiply(blue,  alphaOutline);
            p.Alpha() = alphaOutline;
            p.MoveTo(pixData, x, r.height-1);
            p.Red()   = wxPy_premultiply(red,   alphaOutline);
            p.Green() = wxPy_premultiply(green, alphaOutline);
            p.Blue()  = wxPy_premultiply(blue,  alphaOutline);
            p.Alpha() = alphaOutline;
        }
/* C::B end */
    }
    for (y=0; y<r.height; y++) {
        p.MoveTo(pixData, 0, y);
/* C::B begin */
        if (p.m_ptr) {
            p.Red()   = wxPy_premultiply(red,   alphaOutline);
            p.Green() = wxPy_premultiply(green, alphaOutline);
            p.Blue()  = wxPy_premultiply(blue,  alphaOutline);
            p.Alpha() = alphaOutline;
            p.MoveTo(pixData, r.width-1, y);
            p.Red()   = wxPy_premultiply(red,   alphaOutline);
            p.Green() = wxPy_premultiply(green, alphaOutline);
            p.Blue()  = wxPy_premultiply(blue,  alphaOutline);
            p.Alpha() = alphaOutline;
        }
/* C::B end */
    }

    // Draw the bitmap
/* C::B begin */
    if (bmp.IsOk())
        hDC->DrawBitmap(bmp, r.x, r.y, true);
/* C::B end */

#else
    wxUnusedVar(corner);
    wxUnusedVar(flags);
    wxUnusedVar(alphaFill);
    wxUnusedVar(alphaOutline);
    RectangleDraw(rc, outline, fill);
#endif
}

void SurfaceImpl::DrawRGBAImage(PRectangle rc, int width, int height, const unsigned char *pixelsImage)
{
#if defined(wxHAS_RAW_BITMAP) || defined(wxHAVE_RAW_BITMAP)

    wxRect r = wxRectFromPRect(rc);
    wxBitmap bmp(r.width, r.height, 32);
/* C::B begin */
    if (!bmp.IsOk())
        return;
/* C::B end */
    wxAlphaPixelData pixData(bmp);
/* C::B begin */
    #if !wxCHECK_VERSION(2, 9, 0)
    pixData.UseAlpha(); // wx/rawbmp.h:669 - Call can simply be removed.
    #endif
/* C::B end */

    wxAlphaPixelData::Iterator p(pixData);
    for (int y=0; y<height; y++) {
        p.MoveTo(pixData, 0, y);
        for (int x=0; x<width; x++) {
            unsigned char alpha = pixelsImage[3];
            p.Red()   = wxPy_premultiply(pixelsImage[0], alpha);
            p.Green() = wxPy_premultiply(pixelsImage[1], alpha);
            p.Blue()  = wxPy_premultiply(pixelsImage[2], alpha);
            p.Alpha() = alpha;
            p++;
            pixelsImage += 4;
        }
    }

    // Draw the bitmap
    if (bmp.IsOk())
        hDC->DrawBitmap(bmp, r.x, r.y, true);
#else
    wxUnusedVar(rc);
    wxUnusedVar(width);
    wxUnusedVar(height);
    wxUnusedVar(pixelsImage);
    ColourDesired cdo(0,0,0); ColourDesired cdf(255,255,255);
    RectangleDraw(rc, cdo, cdf);
#endif
}

void SurfaceImpl::Ellipse(PRectangle rc,
    ColourDesired fore,
    ColourDesired back)
{
    PenColour(fore);
    BrushColour(back);
    hDC->DrawEllipse(wxRectFromPRect(rc));
}

void SurfaceImpl::Copy(PRectangle rc, Point from, Surface &surfaceSource)
{
    wxRect r = wxRectFromPRect(rc);
    hDC->Blit(r.x, r.y, r.width, r.height,
              ((SurfaceImpl&)surfaceSource).hDC,
              from.x, from.y, wxCOPY);
}

void SurfaceImpl::DrawTextNoClip(PRectangle rc,
    Font &font,
    XYPOSITION ybase,
    const char *s,
    int len,
    ColourDesired fore,
    ColourDesired back)
{
    SetFont(font);
    hDC->SetTextForeground(wxColourFromCD(fore));
    hDC->SetTextBackground(wxColourFromCD(back));
    FillRectangle(rc, back);

    // ybase is where the baseline should be, but wxWin uses the upper left
    // corner, so I need to calculate the real position for the text...
    hDC->DrawText(sci2wx(s, len), rc.left, ybase - font.ascent);
}

void SurfaceImpl::DrawTextClipped(PRectangle rc,
    Font &font,
    XYPOSITION ybase,
    const char *s,
    int len,
    ColourDesired fore,
    ColourDesired back)
{
    SetFont(font);
    hDC->SetTextForeground(wxColourFromCD(fore));
    hDC->SetTextBackground(wxColourFromCD(back));
    FillRectangle(rc, back);
    hDC->SetClippingRegion(wxRectFromPRect(rc));

    // see comments above
    hDC->DrawText(sci2wx(s, len), rc.left, ybase - font.ascent);
    hDC->DestroyClippingRegion();
}


void SurfaceImpl::DrawTextTransparent(PRectangle rc,
    Font &font,
    XYPOSITION ybase,
    const char *s,
    int len,
    ColourDesired fore)
{
    SetFont(font);
    hDC->SetTextForeground(wxColourFromCD(fore));
/* C::B begin */
    #if wxCHECK_VERSION(2, 9, 0)
    hDC->SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
    #else
    hDC->SetBackgroundMode(wxTRANSPARENT);
    #endif
/* C::B end */

    // ybase is where the baseline should be, but wxWin uses the upper left
    // corner, so I need to calculate the real position for the text...
    hDC->DrawText(sci2wx(s, len), rc.left, ybase - font.ascent);

/* C::B begin */
    #if wxCHECK_VERSION(2, 9, 0)
    hDC->SetBackgroundMode(wxBRUSHSTYLE_SOLID);
    #else
    hDC->SetBackgroundMode(wxSOLID);
    #endif
/* C::B end */
}

void SurfaceImpl::SetClip(PRectangle rc)
{
    hDC->SetClippingRegion(wxRectFromPRect(rc));
}

void SurfaceImpl::MeasureWidths(Font &font,
    const char *s,
    int len,
    XYPOSITION *positions)
{
    wxString str = sci2wx(s, len);
    wxArrayInt tpos;

    SetFont(font);

    hDC->GetPartialTextExtents(str, tpos);

#if wxUSE_UNICODE
    // Map the widths for UCS-2 characters back to the UTF-8 input string
    // NOTE:  I don't think this is right for when sizeof(wxChar) > 2, ie wxGTK2
    // so figure it out and fix it!
    int i = 0;
    size_t ui = 0;
    while (i < len) {
        unsigned char uch = (unsigned char)s[i];
        positions[i++] = tpos[ui];
        if (uch >= 0x80) {
            if (uch < (0x80 + 0x40 + 0x20)) {
                positions[i++] = tpos[ui];
            } else {
                positions[i++] = tpos[ui];
                positions[i++] = tpos[ui];
            }
        }
        ui++;
    }
#else

    // If not unicode then just use the widths we have
#if wxUSE_STL
    std::copy(tpos.begin(), tpos.end(), positions);
#else
    memcpy(positions, tpos.begin(), len * sizeof(int));
#endif
#endif
}


XYPOSITION SurfaceImpl::WidthText(Font &font, const char *s, int len)
{
    SetFont(font);
    int w;
    int h;

    hDC->GetTextExtent(sci2wx(s, len), &w, &h);
    return w;
}

XYPOSITION SurfaceImpl::WidthChar(Font &font, char ch)
{
    SetFont(font);
    int w;
    int h;
    char s[2] = { ch, 0 };

    hDC->GetTextExtent(sci2wx(s, 1), &w, &h);
    return w;
}

#define EXTENT_TEST wxT(" `~!@#$%^&*()-_=+\\|[]{};:\"\'<,>.?/1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")

XYPOSITION SurfaceImpl::Ascent(Font &font)
{
    SetFont(font);
    int w, h, d, e;
    hDC->GetTextExtent(EXTENT_TEST, &w, &h, &d, &e);
    font.ascent = h - d;
    return font.ascent;
}

XYPOSITION SurfaceImpl::Descent(Font &font)
{
    SetFont(font);
    int w, h, d, e;
    hDC->GetTextExtent(EXTENT_TEST, &w, &h, &d, &e);
    return d;
}

XYPOSITION SurfaceImpl::InternalLeading(Font & /* font */)
{
    return 0;
}

XYPOSITION SurfaceImpl::ExternalLeading(Font &font)
{
    SetFont(font);
    int w, h, d, e;
    hDC->GetTextExtent(EXTENT_TEST, &w, &h, &d, &e);
    return e;
}

XYPOSITION SurfaceImpl::Height(Font &font)
{
    SetFont(font);
    return hDC->GetCharHeight() + 1;
}

XYPOSITION SurfaceImpl::AverageCharWidth(Font &font)
{
    SetFont(font);
    return hDC->GetCharWidth();
}

void SurfaceImpl::FlushCachedState()
{
}

void SurfaceImpl::SetUnicodeMode(bool unicodeMode_)
{
    unicodeMode=unicodeMode_;
}

void SurfaceImpl::SetDBCSMode(int /* codePage */)
{
    // dbcsMode = codePage == SC_CP_DBCS;
}


Surface *Surface::Allocate(int technology)
{
    return new SurfaceImpl;
}

//----------------------------------------------------------------------

inline wxWindow* GETWIN(WindowID wid) { return (wxWindow*)wid; }

Window::~Window()
{
}

void Window::Destroy()
{
    if (wid) {
        Show(false);
        GETWIN(wid)->Destroy();
    }
    wid = 0;
}

bool Window::HasFocus()
{
    return wxWindow::FindFocus() == GETWIN(wid);
}

PRectangle Window::GetPosition()
{
    if (! wid) return PRectangle();
    wxRect rc(GETWIN(wid)->GetPosition(), GETWIN(wid)->GetSize());
    return PRectFromwxRect(rc);
}

void Window::SetPosition(PRectangle rc)
{
    wxRect r = wxRectFromPRect(rc);
    GETWIN(wid)->SetSize(r);
}

void Window::SetPositionRelative(PRectangle rc, Window)
{
    SetPosition(rc); // ????
}

PRectangle Window::GetClientPosition()
{
    if (! wid) return PRectangle();
    wxSize sz = GETWIN(wid)->GetClientSize();
    return  PRectangle(0, 0, sz.x, sz.y);
}

void Window::Show(bool show)
{
    GETWIN(wid)->Show(show);
}

void Window::InvalidateAll()
{
    GETWIN(wid)->Refresh(false);
}

void Window::InvalidateRectangle(PRectangle rc)
{
    wxRect r = wxRectFromPRect(rc);
    GETWIN(wid)->Refresh(false, &r);
}

void Window::SetFont(Font &font)
{
    GETWIN(wid)->SetFont(*((wxFont*)font.GetID()));
}

void Window::SetCursor(Cursor curs)
{
    wxStockCursor cursorId;

    switch (curs) {
    case cursorText:
        cursorId = wxCURSOR_IBEAM;
        break;
    case cursorArrow:
        cursorId = wxCURSOR_ARROW;
        break;
    case cursorUp:
        cursorId = wxCURSOR_ARROW; // ** no up arrow...  wxCURSOR_UPARROW;
        break;
    case cursorWait:
        cursorId = wxCURSOR_WAIT;
        break;
    case cursorHoriz:
        cursorId = wxCURSOR_SIZEWE;
        break;
    case cursorVert:
        cursorId = wxCURSOR_SIZENS;
        break;
    case cursorReverseArrow:
        cursorId = wxCURSOR_RIGHT_ARROW;
        break;
    case cursorHand:
        cursorId = wxCURSOR_HAND;
        break;
    default:
        cursorId = wxCURSOR_ARROW;
        break;
    }

    wxCursor wc = wxCursor(cursorId);
    if(curs != cursorLast)
    {
        GETWIN(wid)->SetCursor(wc);
        cursorLast = curs;
    }
}


void Window::SetTitle(const char *s)
{
    GETWIN(wid)->SetLabel (sci2wx(s));
}

// Returns rectangle of monitor pt is on
PRectangle Window::GetMonitorRect(Point pt)
{
    wxRect rect;
    if (! wid) return PRectangle();
#if wxUSE_DISPLAY
    // Get the display the point is found on
    int n = wxDisplay::GetFromPoint(wxPoint(pt.x, pt.y));
    wxDisplay dpy(n == wxNOT_FOUND ? 0 : n);
    rect = dpy.GetGeometry();
#else
    wxUnusedVar(pt);
#endif
    return PRectFromwxRect(rect);
}

#ifdef SCI_NAMESPACE
}
#endif

//----------------------------------------------------------------------
// Helper classes for ListBox


// This is a simple subclass of wxListView that just resets focus to the
// parent when it gets it.
class wxSCIListBox : public wxListView
{
public:
    wxSCIListBox(wxWindow* parent, wxWindowID id,
                 const wxPoint& pos, const wxSize& size,
                 long style)
        : wxListView()
    {
#ifdef __WXMSW__
        Hide(); // don't flicker as we move it around...
#endif
        Create(parent, id, pos, size, style);
    }


    void OnFocus(wxFocusEvent& event) {
        GetParent()->SetFocus();
        event.Skip();
    }

    void OnKillFocus(wxFocusEvent& WXUNUSED(event)) {
        // Do nothing.  Prevents base class from resetting the colors...
    }

#ifdef __WXMAC__
    // For some reason I don't understand yet the focus doesn't really leave
    // the listbox like it should, so if we get any events feed them back to
    // the wxScintilla
    void OnKeyDown(wxKeyEvent& event) {
        GetGrandParent()->GetEventHandler()->ProcessEvent(event);
    }
    void OnChar(wxKeyEvent& event) {
        GetGrandParent()->GetEventHandler()->ProcessEvent(event);
    }

    // And we need to force the focus back when being destroyed
    ~wxSCIListBox() {
        GetGrandParent()->SetFocus();
    }
#endif

private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxSCIListBox, wxListView)
    EVT_SET_FOCUS( wxSCIListBox::OnFocus)
    EVT_KILL_FOCUS(wxSCIListBox::OnKillFocus)
#ifdef __WXMAC__
    EVT_KEY_DOWN(  wxSCIListBox::OnKeyDown)
    EVT_CHAR(      wxSCIListBox::OnChar)
#endif
END_EVENT_TABLE()



#if wxUSE_POPUPWIN //-----------------------------------
#include <wx/popupwin.h>

// A popup window to place the wxSCIListBox upon
class wxSCIListBoxWin : public wxPopupWindow
{
private:
    wxListView*         lv;
    CallBackAction      doubleClickAction;
    void*               doubleClickActionData;
public:
    wxSCIListBoxWin(wxWindow* parent, wxWindowID id, Point WXUNUSED(location)) :
        wxPopupWindow(parent, wxBORDER_NONE)
    {

/* C::B begin */
        SetBackgroundColour(*wxBLACK); // for our simple border
/* C::B end */

        lv = new wxSCIListBox(parent, id, wxDefaultPosition, wxDefaultSize,
                              wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_NO_HEADER | wxSIMPLE_BORDER);
        lv->SetCursor(wxCursor(wxCURSOR_ARROW));
        lv->InsertColumn(0, wxEmptyString);
        lv->InsertColumn(1, wxEmptyString);

        // NOTE: We need to fool the wxListView into thinking that it has the
        // focus so it will use the normal selection colour and will look
        // "right" to the user.  But since the wxPopupWindow or its children
        // can't receive focus then we have to pull a fast one and temporarily
        // parent the listctrl on the STC window and then call SetFocus and
        // then reparent it back to the popup.
        lv->SetFocus();
        lv->Reparent(this);
#ifdef __WXMSW__
        lv->Show();
#endif
#ifdef __WXOSX_COCOA__
        SetBackgroundColour(wxColour(0xC0, 0xC0, 0xC0));
#endif
    }


    // Set position in client coords
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO) {
        if (x != wxDefaultCoord) {
            GetParent()->ClientToScreen(&x, NULL);
        }
        if (y != wxDefaultCoord) {
            GetParent()->ClientToScreen(NULL, &y);
        }
        wxPopupWindow::DoSetSize(x, y, width, height, sizeFlags);
    }

    // return position as if it were in client coords
    virtual void DoGetPosition( int *x, int *y ) const {
        int sx, sy;
        wxPopupWindow::DoGetPosition(&sx, &sy);
        GetParent()->ScreenToClient(&sx, &sy);
        if (x) *x = sx;
        if (y) *y = sy;
    }


    bool Destroy() {
        if ( !wxPendingDelete.Member(this) )
            wxPendingDelete.Append(this);
        return true;
    }


    int IconWidth() {
        wxImageList* il = lv->GetImageList(wxIMAGE_LIST_SMALL);
        if (il != NULL) {
            int w, h;
            il->GetSize(0, w, h);
            return w;
        }
        return 0;
    }


    void SetDoubleClickAction(CallBackAction action, void *data) {
        doubleClickAction = action;
        doubleClickActionData = data;
    }


    void OnFocus(wxFocusEvent& event) {
        GetParent()->SetFocus();
        event.Skip();
    }

    void OnSize(wxSizeEvent& event) {
        // resize the child to fill the popup
        wxSize sz = GetClientSize();
        int x, y, w, h;
        x = y = 0;
        w = sz.x;
        h = sz.y;
#ifdef __WXOSX_COCOA__
        // make room for the parent's bg color to show, to act as a border
        x = y = 1;
        w -= 2;
        h -= 2;
#endif
        lv->SetSize(x, y, w, h);
        // reset the column widths
        lv->SetColumnWidth(0, IconWidth()+4);
        lv->SetColumnWidth(1, w - 2 - lv->GetColumnWidth(0) -
                           wxSystemSettings::GetMetric(wxSYS_VSCROLL_X));
        event.Skip();
    }

    void OnActivate(wxListEvent& WXUNUSED(event)) {
        doubleClickAction(doubleClickActionData);
    }

    wxListView* GetLB() { return lv; }

private:
    DECLARE_EVENT_TABLE()

};

BEGIN_EVENT_TABLE(wxSCIListBoxWin, wxPopupWindow)
    EVT_SET_FOCUS          (          wxSCIListBoxWin::OnFocus)
    EVT_SIZE               (          wxSCIListBoxWin::OnSize)
    EVT_LIST_ITEM_ACTIVATED(wxID_ANY, wxSCIListBoxWin::OnActivate)
END_EVENT_TABLE()

#else // !wxUSE_POPUPWIN -----------------------------------
#include <wx/frame.h>

// A normal window to place the wxSTCListBox upon, but make it behave as much
// like a wxPopupWindow as possible
class wxSCIListBoxWin : public wxFrame {
private:
    wxListView*         lv;
    CallBackAction      doubleClickAction;
    void*               doubleClickActionData;
public:
    wxSCIListBoxWin(wxWindow* parent, wxWindowID id, Point location) :
        wxFrame(parent, id, wxEmptyString, wxPoint(location.x, location.y), wxSize(0,0),
                wxFRAME_NO_TASKBAR
                | wxFRAME_FLOAT_ON_PARENT
#ifdef __WXMAC__
                | wxPOPUP_WINDOW
                | wxNO_BORDER
#else
                | wxSIMPLE_BORDER
#endif
            )
    {

        lv = new wxSCIListBox(this, id, wxDefaultPosition, wxDefaultSize,
                              wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_NO_HEADER | wxSIMPLE_BORDER);
        lv->SetCursor(wxCursor(wxCURSOR_ARROW));
        lv->InsertColumn(0, wxEmptyString);
        lv->InsertColumn(1, wxEmptyString);

        // Eventhough we immediately reset the focus to the parent, this helps
        // things to look right...
        lv->SetFocus();

        Hide();
    }


    // On OSX and (possibly others) there can still be pending
    // messages/events for the list control when Scintilla wants to
    // close it, so do a pending delete of it instead of destroying
    // immediately.
    bool Destroy()
    {
#ifdef __WXMAC__
        // The bottom edge of this window is not getting properly
        // refreshed upon deletion, so help it out...
        wxWindow* p = GetParent();
        wxRect r(GetPosition(), GetSize());
        r.SetHeight(r.GetHeight()+1);
        p->Refresh(false, &r);
#endif
        if ( !wxPendingDelete.Member(this) )
            wxPendingDelete.Append(this);
        return true;
    }


    int IconWidth()
    {
        wxImageList* il = lv->GetImageList(wxIMAGE_LIST_SMALL);
        if (il != NULL) {
            int w, h;
            il->GetSize(0, w, h);
            return w;
        }
        return 0;
    }


    void SetDoubleClickAction(CallBackAction action, void *data)
    {
        doubleClickAction = action;
        doubleClickActionData = data;
    }


    void OnFocus(wxFocusEvent& event)
    {
        ActivateParent();
        GetParent()->SetFocus();
        event.Skip();
    }

    void OnSize(wxSizeEvent& event)
    {
        // resize the child
        wxSize sz = GetClientSize();
        lv->SetSize(sz);
        // reset the column widths
        lv->SetColumnWidth(0, IconWidth()+4);
        lv->SetColumnWidth(1, sz.x - 2 - lv->GetColumnWidth(0) -
                           wxSystemSettings::GetMetric(wxSYS_VSCROLL_X));
        event.Skip();
    }

    void ActivateParent()
    {
        // Although we're a frame, we always want the parent to be active, so
        // raise it whenever we get shown, focused, etc.
        wxTopLevelWindow *frame = wxDynamicCast(
            wxGetTopLevelParent(GetParent()), wxTopLevelWindow);
        if (frame)
            frame->Raise();
    }


    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO)
    {
        // convert coords to screen coords since we're a top-level window
        if (x != wxDefaultCoord) {
            GetParent()->ClientToScreen(&x, NULL);
        }
        if (y != wxDefaultCoord) {
            GetParent()->ClientToScreen(NULL, &y);
        }
        wxFrame::DoSetSize(x, y, width, height, sizeFlags);
    }

    virtual bool Show(bool show = true)
    {
        bool rv = wxFrame::Show(show);
        if (rv && show)
            ActivateParent();
#ifdef __WXMAC__
        GetParent()->Refresh(false);
#endif
        return rv;
    }

    void OnActivate(wxListEvent& WXUNUSED(event))
    {
        doubleClickAction(doubleClickActionData);
    }

    wxListView* GetLB() { return lv; }

private:
    DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(wxSCIListBoxWin, wxWindow)
    EVT_SET_FOCUS          (          wxSCIListBoxWin::OnFocus)
    EVT_SIZE               (          wxSCIListBoxWin::OnSize)
    EVT_LIST_ITEM_ACTIVATED(wxID_ANY, wxSCIListBoxWin::OnActivate)
END_EVENT_TABLE()

#endif // wxUSE_POPUPWIN -----------------------------------


inline wxSCIListBoxWin* GETLBW(WindowID win) {
    return ((wxSCIListBoxWin*)win);
}

inline wxListView* GETLB(WindowID win) {
    return GETLBW(win)->GetLB();
}

//----------------------------------------------------------------------

class ListBoxImpl : public ListBox {
public:
    ListBoxImpl();
    ~ListBoxImpl();

    virtual void SetFont(Font &font);
    virtual void Create(Window &parent, int ctrlID, Point location_,
                        int lineHeight_, bool unicodeMode_, int technology_);
    virtual void SetAverageCharWidth(int width);
    virtual void SetVisibleRows(int rows);
    virtual int GetVisibleRows() const;
    virtual PRectangle GetDesiredRect();
    virtual int CaretFromEdge();
    virtual void Clear();
    virtual void Append(char *s, int type = -1);
            void Append(const wxString& text, int type);
    virtual int Length();
    virtual void Select(int n);
    virtual int GetSelection();
    virtual int Find(const char *prefix);
    virtual void GetValue(int n, char *value, int len);
    virtual void RegisterImage(int type, const char *xpmData);
    virtual void RegisterRGBAImage(int type, int width, int height,
        const unsigned char *pixelsImage);
    virtual void ClearRegisteredImages();
    virtual void SetDoubleClickAction(CallBackAction action, void *data);
    virtual void SetList(const char* list, char separator, char typesep);
private:
    bool                unicodeMode;
    int                 visibleRows;
    wxImageList*        images;
    wxArrayInt*         imagesTypeMap;
    int                 lineHeight;
    int                 aveCharWidth;
    size_t              maxStrWidth;
    Point               location;       // Caret location at which the list is opened
};


ListBoxImpl::ListBoxImpl()
    : unicodeMode(false), visibleRows(5),
      images(NULL),       imagesTypeMap(NULL),
      lineHeight(10),     aveCharWidth(8),
      maxStrWidth(0)
{}

ListBoxImpl::~ListBoxImpl() { wxDELETE(images); wxDELETE(imagesTypeMap); }


void ListBoxImpl::Create(Window &parent,
    int ctrlID,
    Point location_,
    int lineHeight_,
    bool unicodeMode_,
    int technology_)
{
    location = location_;
    lineHeight =  lineHeight_;
    unicodeMode = unicodeMode_;
    maxStrWidth = 0;
    wid = new wxSCIListBoxWin (GETWIN(parent.GetID()), ctrlID, location_);
    if (images != NULL)
        GETLB(wid)->SetImageList(images, wxIMAGE_LIST_SMALL);
}

void ListBoxImpl::SetFont(Font &font)
{
    GETLB(wid)->SetFont(*((wxFont*)font.GetID()));
}

void ListBoxImpl::SetAverageCharWidth(int width) { aveCharWidth = width; }

void ListBoxImpl::SetVisibleRows(int rows)
{
    visibleRows = rows;
}

int ListBoxImpl::GetVisibleRows() const
{
    return visibleRows;
}

PRectangle ListBoxImpl::GetDesiredRect()
{
    // wxListCtrl doesn't have a DoGetBestSize, so instead we kept track of
    // the max size in Append and calculate it here...
    int maxw = maxStrWidth * aveCharWidth;
    int maxh ;

    // give it a default if there are no lines, and/or add a bit more
    if (maxw == 0) maxw = 100;
    maxw += aveCharWidth * 3 +
            GETLBW(wid)->IconWidth() + wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
    if (maxw > 350)
        maxw = 350;

    // estimate a desired height
    int count = GETLB(wid)->GetItemCount();
    if (count) {
        wxRect rect;
        GETLB(wid)->GetItemRect(0, rect);
        maxh = count * rect.GetHeight();
        if (maxh > 140)  // TODO:  Use visibleRows??
            maxh = 140;

        // Try to make the size an exact multiple of some number of lines
        int lines = maxh / rect.GetHeight();
        maxh = (lines + 1) * rect.GetHeight() + 2;
    }
    else
        maxh = 100;

    PRectangle rc;
    rc.top = 0;
    rc.left = 0;
    rc.right = maxw;
    rc.bottom = maxh;
    return rc;
}

int ListBoxImpl::CaretFromEdge()
{
    return 4 + GETLBW(wid)->IconWidth();
}

void ListBoxImpl::Clear()
{
    GETLB(wid)->DeleteAllItems();
}

void ListBoxImpl::Append(char *s, int type)
{
    Append(sci2wx(s), type);
}

void ListBoxImpl::Append(const wxString& text, int type) {
    long count  = GETLB(wid)->GetItemCount();
    long itemID  = GETLB(wid)->InsertItem(count, wxEmptyString);
    long idx = -1;
    GETLB(wid)->SetItem(itemID, 1, text);
    maxStrWidth = wxMax(maxStrWidth, text.length());
    if (type != -1) {
        wxCHECK_RET(imagesTypeMap, wxT("Unexpected NULL imagesTypeMap"));
        idx = imagesTypeMap->Item(type);
    }
    GETLB(wid)->SetItemImage(itemID, idx, idx);
}

int ListBoxImpl::Length()
{
    return GETLB(wid)->GetItemCount();
}

void ListBoxImpl::Select(int n)
{
    bool select = true;
    if (n == -1) {
        n = 0;
        select = false;
    }
    GETLB(wid)->EnsureVisible(n);
/* C::B begin */
    GETLB(wid)->Focus(n);
/* C::B end */
    GETLB(wid)->Select(n, select);
}

int ListBoxImpl::GetSelection()
{
    return GETLB(wid)->GetFirstSelected();
}

int ListBoxImpl::Find(const char *prefix)
{
    wxListView *list = GETLB(wid);
    wxString sPrefix = unicodeMode ? sci2wx(prefix) : wxString::From8BitData(prefix);
    for (int i=0; i<list->GetItemCount(); ++i) {
        if (list->GetItemText(i).StartsWith(sPrefix))
            return i;
    }

    return wxNOT_FOUND;
}

void ListBoxImpl::GetValue(int n, char *value, int len)
{
    wxListItem item;
    item.SetId(n);
    item.SetColumn(1);
    item.SetMask(wxLIST_MASK_TEXT);
    GETLB(wid)->GetItem(item);
    strncpy(value, wx2sci(item.GetText()), len);
    value[len-1] = '\0';
}

void ListBoxImpl::RegisterImage(int type, const char *xpmData)
{
    wxMemoryInputStream stream(xpmData, strlen(xpmData)+1);
    wxImage img(stream, wxBITMAP_TYPE_XPM);
    wxBitmap bmp(img);
/* C::B begin */
    if (!bmp.IsOk())
        return;
/* C::B end */

    if (!images) {
        // assumes all images are the same size
        images = new wxImageList(bmp.GetWidth(), bmp.GetHeight(), true);
        imagesTypeMap = new wxArrayInt;
    }

    int idx = images->Add(bmp);

    // do we need to extend the mapping array?
    wxArrayInt& itm = *imagesTypeMap;
    if ( itm.GetCount() < (size_t)type+1)
        itm.Add(-1, type - itm.GetCount() + 1);

    // Add an item that maps type to the image index
    itm[type] = idx;
}

void ListBoxImpl::RegisterRGBAImage(int type, int width, int height, const unsigned char* pixelsImage) {
/* C::B begin */
    // TODO!!!
/* C::B end */
}

void ListBoxImpl::ClearRegisteredImages()
{
    wxDELETE(images);
    wxDELETE(imagesTypeMap);
    if (wid)
        GETLB(wid)->SetImageList(NULL, wxIMAGE_LIST_SMALL);
}

void ListBoxImpl::SetList(const char* list, char separator, char typesep)
{
    GETLB(wid)->Freeze();
    Clear();
    wxStringTokenizer tkzr(sci2wx(list), (wxChar)separator);
    while ( tkzr.HasMoreTokens() ) {
        wxString token = tkzr.GetNextToken();
        long type = -1;
        int pos = token.Find(typesep);
        if (pos != -1) {
            token.Mid(pos+1).ToLong(&type);
            token.Truncate(pos);
        }
        Append(token, (int)type);
    }
    GETLB(wid)->Thaw();
}

ListBox::ListBox() {}

ListBox::~ListBox() {}

ListBox *ListBox::Allocate()
{
    return new ListBoxImpl();
}
void ListBoxImpl::SetDoubleClickAction(CallBackAction action, void *data)
{
    GETLBW(wid)->SetDoubleClickAction(action, data);
}

#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif

//----------------------------------------------------------------------

Menu::Menu() : mid(0) {}

void Menu::CreatePopUp()
{
    Destroy();
    mid = new wxMenu();
}

void Menu::Destroy()
{
    if (mid) {
        wxMenu *menu = static_cast<wxMenu *>(mid);
        delete menu;
    }
    mid = 0;
}

void Menu::Show(Point pt, Window &w)
{
    wxMenu *menu = static_cast<wxMenu *>(mid);
    GETWIN(w.GetID())->PopupMenu(menu, pt.x - 4, pt.y);
    Destroy();
}

//----------------------------------------------------------------------

/* C::B begin */
class DynamicLibraryImpl : public DynamicLibrary {
protected:
    wxDynamicLibrary *lib;
public:
    DynamicLibraryImpl(const char *modulePath) {
        wxString path = sci2wx(modulePath);
        lib = new wxDynamicLibrary(path);
    }

    virtual ~DynamicLibraryImpl() {
        if (lib && lib->IsLoaded())
            lib->Unload();
        delete lib;
        lib = 0;
    }

    virtual Function FindFunction(const char *name) {
        wxString symbol = sci2wx(name);
        if (lib && lib->HasSymbol(symbol)) {
            return static_cast<Function>(lib->GetSymbol(symbol));
        }
        return NULL;
    }

    virtual bool IsValid() {
        if (lib && lib->IsLoaded())
            return true;
        return false;
    }
};

DynamicLibrary *DynamicLibrary::Load(const char *modulePath)
{
    return static_cast<DynamicLibrary *>(new DynamicLibraryImpl(modulePath));
}

/* C::B end */

//----------------------------------------------------------------------

ColourDesired Platform::Chrome()
{
    wxColour c = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    return ColourDesired(c.Red(), c.Green(), c.Blue());
}

ColourDesired Platform::ChromeHighlight()
{
    wxColour c = wxSystemSettings::GetColour(wxSYS_COLOUR_3DHIGHLIGHT);
    return ColourDesired(c.Red(), c.Green(), c.Blue());
}

const char *Platform::DefaultFont()
{
    static char fontNameDefault[200] = "";
    if (!fontNameDefault[0]) {
        const wxFont* font = wxNORMAL_FONT;
        strcpy(fontNameDefault, font->GetFaceName().ToAscii());
    }
    return fontNameDefault;
}

int Platform::DefaultFontSize()
{
    const wxFont* font = wxNORMAL_FONT;
    return font->GetPointSize();
}

unsigned int Platform::DoubleClickTime()
{
#ifdef __WXMSW__
    return ::GetDoubleClickTime();
#else
    return 400;
#endif
}

bool Platform::MouseButtonBounce()
{
    return false;
}

bool Platform::IsKeyDown(int /*key*/)
{
    return false;
}

long Platform::SendScintilla(WindowID w,
                             unsigned int msg,
                             unsigned long wParam,
                             long lParam)
{
/* C::B begin */
    wxScintilla* sci = (wxScintilla*)w;
    return sci->SendMsg(msg, (wxUIntPtr)wParam, (wxIntPtr)lParam);
/* C::B end */
}

long Platform::SendScintillaPointer(WindowID w,
                                    unsigned int msg,
                                    unsigned long wParam,
                                    void *lParam)
{
/* C::B begin */
    wxScintilla* sci = (wxScintilla*)w;
    return sci->SendMsg(msg, (wxUIntPtr)wParam, (wxIntPtr)lParam);
/* C::B end */
}

int Platform::Minimum(int a, int b)
{
    return wxMin(a,b);
}

int Platform::Maximum(int a, int b)
{
    return wxMax(a,b);
}

int Platform::Clamp(int val, int minVal, int maxVal)
{
    return wxClip(val, minVal, maxVal);
}

void Platform::DebugDisplay(const char *s)
{
#ifdef TRACE
    wxLogDebug(stc2wx(s));
#else
    wxUnusedVar(s);
#endif
}

void Platform::DebugPrintf(const char *format, ...)
{
    char buffer[2000];
    va_list pArguments;
    va_start(pArguments, format);
    vsprintf(buffer,format,pArguments);
    va_end(pArguments);
    Platform::DebugDisplay(buffer);
}

static bool assertionPopUps = true;

bool Platform::ShowAssertionPopUps(bool assertionPopUps_)
{
    bool ret = assertionPopUps;
    assertionPopUps = assertionPopUps_;
    return ret;
}

void Platform::Assert(const char *c, const char *file, int line)
{
    char buffer[2000];
    sprintf(buffer, "Assertion [%s] failed at %s %d", c, file, line);
    if (assertionPopUps) {
        wxMessageBox(sci2wx(buffer), wxT("Assertion Failure"), wxICON_HAND | wxOK);
    } else {
        strcat(buffer, "\n");
        Platform::DebugDisplay(buffer);
    }
}


bool Platform::IsDBCSLeadByte(int codePage, char ch)
{
	// Byte ranges found in Wikipedia articles with relevant search strings in each case
	unsigned char uch = static_cast<unsigned char>(ch);
	switch (codePage) {
	case 932:
		// Shift_jis
		return ((uch >= 0x81) && (uch <= 0x9F)) ||
		       ((uch >= 0xE0) && (uch <= 0xEF));
	case 936:
		// GBK
		return (uch >= 0x81) && (uch <= 0xFE);
	case 949:
		// Korean Wansung KS C-5601-1987
		return (uch >= 0x81) && (uch <= 0xFE);
	case 950:
		// Big5
		return (uch >= 0x81) && (uch <= 0xFE);
	case 1361:
		// Korean Johab KS C-5601-1992
		return
		    ((uch >= 0x84) && (uch <= 0xD3)) ||
		    ((uch >= 0xD8) && (uch <= 0xDE)) ||
		    ((uch >= 0xE0) && (uch <= 0xF9));
	}
	return false;
}

int Platform::DBCSCharLength(int codePage, const char *s)
{
	if (codePage == 932 || codePage == 936 || codePage == 949 ||
	        codePage == 950 || codePage == 1361) {
		return IsDBCSLeadByte(codePage, s[0]) ? 2 : 1;
	} else {
		return 1;
	}
}

int Platform::DBCSCharMaxLength()
{
    return 1;
}


//----------------------------------------------------------------------

ElapsedTime::ElapsedTime()
{
    wxLongLong localTime = wxGetLocalTimeMillis();
    littleBit = localTime.GetLo();
    bigBit = localTime.GetHi();
}

double ElapsedTime::Duration(bool reset)
{
    wxLongLong prevTime(bigBit, littleBit);
    wxLongLong localTime = wxGetLocalTimeMillis();
    if (reset) {
        littleBit = localTime.GetLo();
        bigBit = localTime.GetHi();
    }
    wxLongLong duration = localTime - prevTime;
    double result = duration.ToDouble();
    result /= 1000.0;
    return result;
}

#ifdef SCI_NAMESPACE
}
#endif

//----------------------------------------------------------------------

#if wxUSE_UNICODE

#include "UniConversion.h"

// Convert using Scintilla's functions instead of wx's, Scintilla's are more
// forgiving and won't assert...

wxString sci2wx(const char* str, size_t len)
{
    if (!len)
        return wxEmptyString;

    size_t wclen = UTF16Length(str, len);
    wxWCharBuffer buffer(wclen+1);

    size_t actualLen = UTF16FromUTF8(str, len, buffer.data(), wclen+1);
    return wxString(buffer.data(), actualLen);
}



wxString sci2wx(const char* str)
{
    return sci2wx(str, strlen(str));
}


const wxWX2MBbuf wx2sci(const wxString& str)
{
    const wchar_t* wcstr = str.c_str();
    size_t wclen         = str.length();
    size_t len           = UTF8Length(wcstr, wclen);

    wxCharBuffer buffer(len+1);
    UTF8FromUTF16(wcstr, wclen, buffer.data(), len);

    // TODO check NULL termination!!

    return buffer;
}

#endif // wxUSE_UNICODE
