/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "splashscreen.h"

#include <wx/dc.h>

#include "appglobals.h"
#include "configmanager.h"
#include "pluginmanager.h"

BEGIN_EVENT_TABLE(cbSplashScreen, wxSplashScreen)
    EVT_CLOSE(cbSplashScreen::OnCloseWindow)
END_EVENT_TABLE()

cbSplashScreen::cbSplashScreen(const wxBitmap& bitmap)
  : wxSplashScreen(bitmap, wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_NO_TIMEOUT,
                   0, nullptr, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                   wxBORDER_NONE | wxFRAME_NO_TASKBAR | wxFRAME_SHAPED | wxSTAY_ON_TOP)
{
}

void cbSplashScreen::DrawReleaseInfo(wxDC  &dc)
{
    static const wxString release(wxT(RELEASE));
    static const wxString revision = wxT(" ")+ ConfigManager::GetRevisionString();

    wxFont largeFont(15, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    wxFont smallFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

    wxCoord lf_width, lf_height, lf_descend;
    dc.GetTextExtent(release,  &lf_width, &lf_height, &lf_descend, nullptr, &largeFont);

    wxCoord sf_width, sf_height, sf_descend;
    dc.GetTextExtent(release + revision, &sf_width, &sf_height, &sf_descend, nullptr, &smallFont);

    int text_center = 450;
    int x_offset = text_center;
    int y = 220;

    dc.SetTextForeground(*wxBLACK);

#if SVN_BUILD
    // only render SVN revision when not building official release
    x_offset = text_center - (sf_width)/2;
    dc.SetFont(smallFont);
    dc.DrawText(release + revision, x_offset, (y - sf_height + sf_descend));
#else
    x_offset = text_center - (lf_width)/2;
    dc.SetFont(largeFont);
    dc.DrawText(release,  x_offset, (y  - lf_height + lf_descend));
#endif

    if (PluginManager::GetSafeMode())
    {
        wxCoord sm_width, sm_height, sm_descend;
        dc.GetTextExtent(_("SAFE MODE"), &sm_width, &sm_height, &sm_descend, nullptr, &smallFont);
        dc.SetFont(smallFont);
        dc.SetTextForeground(*wxRED);
        x_offset = text_center - (sm_width)/2;
        dc.DrawText(_("SAFE MODE"), x_offset, (y  - sm_height + sm_descend + lf_height+10));
        dc.SetTextForeground(*wxBLACK);
    }
}

void cbSplashScreen::OnCloseWindow(wxCloseEvent &)
{
    // Don't destroy it here. It creates a dangling pointer in app.cpp and crashes C::B
    Hide();
}
