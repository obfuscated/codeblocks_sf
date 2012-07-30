// Scintilla platform layer for wxWidgets

#ifndef PLATWX_H
#define PLATWX_H

#include "Platform.h"

#include <wx/colour.h>
#include <wx/gdicmn.h> // wxRect

class wxDC;

#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif

inline wxColour wxColourFromCDandAlpha(ColourDesired cd, int alpha)
{
    return wxColour((unsigned char)cd.GetRed(),
                    (unsigned char)cd.GetGreen(),
                    (unsigned char)cd.GetBlue(),
                    (unsigned char)alpha);
}

inline wxColour wxColourFromCD(ColourDesired cd)
{
    unsigned int r = cd.GetRed();
    unsigned int g = cd.GetGreen();
    unsigned int b = cd.GetBlue();
    return wxColour((unsigned char)r, (unsigned char)g, (unsigned char)b);
}

inline wxRect wxRectFromPRect(PRectangle pr)
{
    return wxRect(pr.left, pr.top, pr.Width(), pr.Height());
}

inline PRectangle PRectFromwxRect(wxRect wxr)
{
    return PRectangle(wxr.GetLeft(), wxr.GetTop(), wxr.GetRight()+1, wxr.GetBottom()+1);
}

inline Point Point::FromLong(long lpoint)
{
    return Point(lpoint & 0xFFFF, lpoint >> 16);
}

class SurfaceImpl : public Surface {
private:
    wxDC*       hDC;
    wxBitmap*   bitmap;
    bool        hDCOwned;
    int         x, y;
    bool        unicodeMode;

public:
    SurfaceImpl();
    ~SurfaceImpl();

    virtual void Init(WindowID wid);
    virtual void Init(SurfaceID sid, WindowID wid);
    virtual void InitPixMap(int width, int height,
        Surface *surface_, WindowID wid);

    virtual void Release();
    virtual bool Initialised();
    virtual void PenColour(ColourDesired fore);
    virtual int LogPixelsY();
    virtual int DeviceHeightFont(int points);
    virtual void MoveTo(int x_, int y_);
    virtual void LineTo(int x_, int y_);
    virtual void Polygon(Point *pts, int npts, ColourDesired fore,
        ColourDesired back);
    virtual void RectangleDraw(PRectangle rc, ColourDesired fore,
        ColourDesired back);
    virtual void FillRectangle(PRectangle rc, ColourDesired back);
    virtual void FillRectangle(PRectangle rc, Surface &surfacePattern);
    virtual void RoundedRectangle(PRectangle rc, ColourDesired fore,
        ColourDesired back);
    virtual void AlphaRectangle(PRectangle rc, int corner, ColourDesired fill,
        int alphaFill, ColourDesired outline, int alphaOutline, int flags);
    virtual void DrawRGBAImage(PRectangle rc, int width, int height,
        const unsigned char *pixelsImage);
    virtual void Ellipse(PRectangle rc, ColourDesired fore,
        ColourDesired back);
    virtual void Copy(PRectangle rc, Point from, Surface &surfaceSource);

    virtual void DrawTextNoClip(PRectangle rc, Font &font_, XYPOSITION ybase,
        const char *s, int len, ColourDesired fore, ColourDesired back);
    virtual void DrawTextClipped(PRectangle rc, Font &font_, XYPOSITION ybase,
        const char *s, int len, ColourDesired fore, ColourDesired back);
    virtual void DrawTextTransparent(PRectangle rc, Font &font_, XYPOSITION ybase,
        const char *s, int len, ColourDesired fore);
    virtual void MeasureWidths(Font &font_, const char *s, int len,
        XYPOSITION *positions);
    virtual XYPOSITION WidthText(Font &font_, const char *s, int len);
    virtual XYPOSITION WidthChar(Font &font_, char ch);
    virtual XYPOSITION Ascent(Font &font_);
    virtual XYPOSITION Descent(Font &font_);
    virtual XYPOSITION InternalLeading(Font &font_);
    virtual XYPOSITION ExternalLeading(Font &font_);
    virtual XYPOSITION Height(Font &font_);
    virtual XYPOSITION AverageCharWidth(Font &font_);

    virtual void SetClip(PRectangle rc);
    virtual void FlushCachedState();

    virtual void SetUnicodeMode(bool unicodeMode_);
    virtual void SetDBCSMode(int codePage);

    void BrushColour(ColourDesired back);
    void SetFont(Font &font_);
};

#ifdef SCI_NAMESPACE
}
#endif

#endif
