/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */
#ifndef CBSPLASH_SCREEN_H
#define CBSPLASH_SCREEN_H

#include <wx/splash.h>

class cbSplashScreen : public wxSplashScreen
{
public:
    cbSplashScreen(const wxBitmap& bitmap);
    static void DrawReleaseInfo(class wxDC &dc);
private:

    void OnCloseWindow(wxCloseEvent &event);

    DECLARE_EVENT_TABLE()
};

#endif // CBSPLASH_SCREEN_H
