/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

/*
 * Objects of type InfoWindow are autonomous and must therefore always be instantiated using operator new.
 * In addition to this, InfoWindow does not have any non-static public class members.
 *
 * Since a (technically 100% correct) statement like
 *   new InfoWindow("foo", "bar");
 * is unintuitive, confusing or even objective to some people, this class uses a variation of the "Named Constructor Idiom".
 *
 * InfoWindow::Display("foo", "bar");
 * does the exact same thing as the above statement but looks a lot nicer.
 */

#ifndef INFOWINDOW_H
#define INFOWINDOW_H

#include <wx/event.h>
#include <wx/timer.h>
#include <wx/string.h>

#if wxUSE_POPUPWIN
    #include <wx/popupwin.h>
    typedef wxPopupWindow wxInfoWindowBase;
#else
    #include "scrollingdialog.h"
    typedef wxScrollingDialog wxInfoWindowBase;
#endif

#undef new
#include <list>
#include <algorithm>

class InfoWindow : public wxInfoWindowBase
{
        InfoWindow(const wxString& title, const wxString& message, unsigned int delay, unsigned int hysteresis);
        virtual ~InfoWindow();
        void OnTimer(wxTimerEvent& e);
        void OnMove(wxMouseEvent& e);
        void OnClick(wxMouseEvent& e);

    public:
        static void Display(const wxString& title, const wxString& message,
                            unsigned int delay = 5000, unsigned int hysteresis = 1);
    private:
        wxTimer *m_timer;
        int left;
        int top;
        int hMin;
        int pos;
        unsigned int status;
        unsigned int m_delay;
        unsigned int ks;
        std::list<wxString>::iterator my_message_iterator;
    private:
        DECLARE_EVENT_TABLE()
};

#endif
