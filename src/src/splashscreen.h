/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef CBSPLASH_SCREEN_H
#define CBSPLASH_SCREEN_H

#include <wx/bitmap.h>
#include <wx/dc.h>
#include <wx/timer.h>
#include <wx/frame.h>

class cbSplashScreen : public wxFrame
{
  private:
    wxBitmap m_label;
    wxTimer  m_timer;
//    wxRegion m_region; // was used for 08/02 splash screen (transparent regions)
  public:
    // A value of -1 for timeout makes it stay forever (you need to close it manually)
    cbSplashScreen(wxBitmap &label, long timeout, wxWindow *parent, wxWindowID id, long style = wxSTAY_ON_TOP | wxNO_BORDER | wxFRAME_NO_TASKBAR | wxFRAME_SHAPED);
    ~cbSplashScreen();

    static void DrawReleaseInfo(wxDC &dc);
  private:
    void DoPaint(wxDC &dc);
    void OnPaint(wxPaintEvent &);
    void OnEraseBackground(wxEraseEvent &);
    void OnTimer(wxTimerEvent &);
    void OnCloseWindow(wxCloseEvent &);
    void OnChar(wxKeyEvent &);
    void OnMouseEvent(wxMouseEvent &event);

  DECLARE_EVENT_TABLE()
};

#endif // CBSPLASH_SCREEN_H

