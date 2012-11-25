/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "splashscreen.h"
#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/dcscreen.h>
#include "configmanager.h"
#include "pluginmanager.h"
#include "appglobals.h"
#include "prep.h" // haven't included sdk in this source file

namespace
{
  int cbSplashScreen_timer_id = wxNewId();
}

BEGIN_EVENT_TABLE(cbSplashScreen, wxFrame)
    EVT_PAINT(cbSplashScreen::OnPaint)
    EVT_TIMER(cbSplashScreen_timer_id, cbSplashScreen::OnTimer)
    EVT_ERASE_BACKGROUND(cbSplashScreen::OnEraseBackground)
    EVT_CLOSE(cbSplashScreen::OnCloseWindow)
    EVT_CHAR(cbSplashScreen::OnChar)
    EVT_MOUSE_EVENTS(cbSplashScreen::OnMouseEvent)
END_EVENT_TABLE()

void cbSplashScreen::DoPaint(wxDC &dc)
{
#ifdef __WIN32__
  #if wxCHECK_VERSION(2, 9, 0)
//    dc.SetDeviceClippingRegion(m_region); // was used for 08/02 splash screen (transparent regions)
  #else
//    dc.SetClippingRegion(m_region); // was used for 08/02 splash screen (transparent regions)
  #endif
#endif

    dc.DrawBitmap(m_label, 0, 0, false);

    {  // keep this (kind of) in sync with dlgabout.cpp!
        static const wxString release(wxT(RELEASE));
        static const wxString revision = ConfigManager::GetRevisionString();

        wxFont largeFont(16, wxSWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        wxFont smallFont(9,  wxSWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

        wxCoord lf_width, lf_heigth, sf_width, sf_heigth, sm_width, sm_height;

        dc.GetTextExtent(release,  &lf_width, &lf_heigth, 0, 0, &largeFont);
        dc.GetTextExtent(revision, &sf_width, &sf_heigth, 0, 0, &smallFont);

        dc.GetTextExtent(_("SAFE MODE"), &sm_width, &sm_height, 0, 0, &largeFont);

        int x_offset = 280;
        int y_offset = 150;

        lf_width >>= 1;
        sf_width >>= 1;
        int y      = y_offset - ((lf_heigth + sf_heigth + 8) >> 1);

        dc.SetTextForeground(*wxBLACK);

        dc.SetFont(largeFont);
#if SVN_BUILD
        dc.DrawText(release,  x_offset - lf_width, y);
        // only render SVN revision when not building official release
        dc.SetFont(smallFont);
        dc.DrawText(revision, x_offset - sf_width, y +  lf_heigth);
#else
        dc.DrawText(release,  x_offset - lf_width, y + (lf_heigth >> 1));
#endif

        if (PluginManager::GetSafeMode())
        {
            dc.SetFont(largeFont);
            dc.SetTextForeground(*wxRED);
            dc.DrawText(_("SAFE MODE"), (dc.GetSize().GetX() - sm_width) / 2, y + lf_heigth);
            dc.SetTextForeground(*wxBLACK);
        }
    }
}

void cbSplashScreen::OnPaint(wxPaintEvent &)
{
    // an obscure statement in the wxWidgets book says to
    // allocate the DC even if you don't paint to avoid
    // a paint loop.    //pecan 2006/04/3
    wxPaintDC paint_dc(this);
    DoPaint(paint_dc);
}

void cbSplashScreen::OnEraseBackground(wxEraseEvent &event)
{
    wxDC *dc = event.GetDC();

    if (dc)
        DoPaint(*dc); // why not? :)
}

void cbSplashScreen::OnTimer(wxTimerEvent &)
{
    Close(true);
}

void cbSplashScreen::OnCloseWindow(wxCloseEvent &)
{
    m_timer.Stop();
    // Don't destroy it here. It creates a dangling pointer in app.cpp and crashes C::B
    Hide();
}

void cbSplashScreen::OnChar(wxKeyEvent &)
{
    Close(true);
}

void cbSplashScreen::OnMouseEvent(wxMouseEvent &event)
{
    if (event.LeftDown() || event.RightDown())
        Close(true);
}

cbSplashScreen::cbSplashScreen(wxBitmap &label, long timeout, wxWindow *parent, wxWindowID id, long style)
: wxFrame(parent, id, wxEmptyString, wxPoint(0, 0), wxSize(440, 217), style),
  m_timer(this, cbSplashScreen_timer_id), m_region(0, 0, 181, 181)
{
    m_region.Union(50,  35, 181, 181);
    m_region.Union(166, 13, 181, 181);
    m_region.Union(259, 29, 181, 181);

    int w = label.GetWidth();
    int h = label.GetHeight();

    SetClientSize(w, h);
    CentreOnScreen();

    wxScreenDC screen_dc;
    wxMemoryDC label_dc;

    int x;
    int y;

    if (!platform::macosx)
    {
        x = GetPosition().x;
        y = GetPosition().y;
    }

    m_label.Create(w, h);

    label_dc.SelectObject(m_label);
    if (!platform::macosx)
    {
        label_dc.Blit(0, 0, w, h, &screen_dc, x, y);
    }
    else
    {
        label_dc.SetBackground(*wxWHITE_BRUSH);
        label_dc.Clear();
    }
    label_dc.DrawBitmap(label, 0, 0, true);
    label_dc.SelectObject(wxNullBitmap);

//    SetShape(m_region); // was used for 08/02 splash screen (transparent regions)

    Show(true);
    SetThemeEnabled(false); // seems to be useful by description
    SetBackgroundStyle(wxBG_STYLE_CUSTOM); // the trick for GTK+ (notice it's after Show())

    if (platform::macosx)
      Centre(wxBOTH | wxCENTRE_ON_SCREEN); // centre only works when the window is showing

    if (platform::windows || platform::macosx)
        Update();
    else
        wxYieldIfNeeded();

    if (timeout != -1)
        m_timer.Start(timeout, true);
}

cbSplashScreen::~cbSplashScreen()
{
    m_timer.Stop();
}
