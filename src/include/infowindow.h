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
    typedef wxDialog wxInfoWindowBase;
#endif

#undef new
#include <list>
#include <algorithm>

class Stacker
{
    std::list<int> widths;

    public:

    int StackMe(int mySize)
    {
        mySize += 3;
        int pos = 0;

        if(!widths.empty())
            pos = *(std::max_element(widths.begin(), widths.end()));

        widths.push_back(pos + mySize);
        return pos + mySize;
    };

    void ReleaseMe(int myPos)
    {
        std::list<int>::iterator it = std::find(widths.begin(), widths.end(), myPos);
        if(it != widths.end())
            widths.erase(it);
    };
};


class InfoWindow : public wxInfoWindowBase
{
    wxTimer *m_timer;
    int left;
    int top;
    int hMin;
    int pos;
    unsigned int status;
    unsigned int m_delay;
    unsigned int ks;
    std::list<wxString>::iterator my_message_iterator;
    static Stacker stacker;
    static int screenWidth;
    static int screenHeight;
    static std::list<wxString> active_messages; // if a new message is in this, don't display it (already is displayed). Check Display()

    InfoWindow(const wxString& title, const wxString& message, unsigned int delay, unsigned int hysteresis);
    virtual ~InfoWindow();
    void OnTimer(wxTimerEvent& e);
    void OnMove(wxMouseEvent& e);
    void OnClick(wxMouseEvent& e);
    DECLARE_EVENT_TABLE()

    public:

    static void Display(const wxString& title, const wxString& message, unsigned int delay = 5000, unsigned int hysteresis = 1);
};

#endif
