#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/intl.h>
    #include <wx/stattext.h>
    #include <wx/sizer.h>
    #include <wx/settings.h>
    #include "infowindow.h"
    #include "manager.h"
#endif

BEGIN_EVENT_TABLE(InfoWindow, wxInfoWindowBase)
EVT_TIMER(-1, InfoWindow::OnTimer)
EVT_MOTION(InfoWindow::OnMove)
EVT_LEFT_DOWN(InfoWindow::OnClick)
EVT_RIGHT_DOWN(InfoWindow::OnClick)
END_EVENT_TABLE()

const wxColour titleBackground(96,96,96); // dark grey
const wxColour textBackground(255,255,160); // yellowish


const char *iBitmap[] = {
"20 20 38 1",
"  c #606060",
". c gray38",
"X c #646464",
"o c #656565",
"O c gray40",
"+ c gray45",
"@ c gray50",
"# c blue",
"$ c #0808FF",
"% c #0A0AFF",
"& c #0B0BFF",
"* c #2121FF",
"= c #2828FF",
"- c #3838FF",
"; c #4545FF",
": c #4646FF",
"> c #5656FF",
", c #5959FF",
"< c gray51",
"1 c #8E8E8E",
"2 c gray59",
"3 c #9191A7",
"4 c #8888EF",
"5 c #8080F7",
"6 c #8282F7",
"7 c #8787FE",
"8 c #8A8AFE",
"9 c #9696FE",
"0 c #9797FF",
"q c #ABABFF",
"w c #CDCDFF",
"e c #D5D5FF",
"r c #E4E4F9",
"t c gray96",
"y c #F6F6F6",
"u c gray97",
"i c #F8F8F8",
"p c gray100",
"     X+<1221<+O     ",
"   X+35,=&&=,63+X   ",
"  X@4:########:4@X  ",
" X@5*#&>0ww0,&#*5@X ",
" +4*#-qppppppq:#*4+ ",
"X3:#:eppp##pppe:#:3X",
"+6#&qpppp##ppppq&#6+",
"<,#,ppppp##ppppp,#,@",
"1=#9ppppp##ppppp0#=1",
"2&#wppppp##pppppw#&2",
"i&#wppppp##pppppw#&u",
"i=#9ppppp##ppppp0#=u",
"u,#>pppppppppppp>#,y",
"y8#&qpppp##ppppq$#8y",
"yr:#:eppp##pppe:#:rt",
"yy9*#:qppppppq:#*9yt",
"yyy7*#$>0ww0>&#*7ytt",
"ttyy9:########:9yytt",
"ttttyr8,=&&=,8ryyttt",
"ttttyyyyiuuuuyyyyttt"
};


Stacker InfoWindow::stacker;

// in wxGTK this initialization raises an assertion (makes sense too)
// so initialize them to -1 and we 'll set them up correctly in InfoWindow's ctor the first time
int InfoWindow::screenWidth = -1;//wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
int InfoWindow::screenHeight = -1;//wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
std::list<wxString> InfoWindow::active_messages;

namespace
{
    // while in windows world, sleep(1) takes anywhere between 20-50 milliseconds,
    // in linux sleep(1) means sleep 1 millisecond.
    // so we need conditional compilation here in order for the scrolling effect to be
    // visible under non-windows platforms :)
    static const int scroll_millis = platform::windows ? 1 : 5;
}

class ForwardingTextControl : public wxStaticText
{
    void OnEvent(wxMouseEvent& e){e.ResumePropagation(10); e.Skip();};
    DECLARE_EVENT_TABLE();
    public:
    ForwardingTextControl(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0) : wxStaticText(parent, id, label, pos, size, style){};
};

BEGIN_EVENT_TABLE(ForwardingTextControl, wxStaticText)
EVT_MOUSE_EVENTS(ForwardingTextControl::OnEvent)
END_EVENT_TABLE()


InfoWindow::InfoWindow(const wxString& title, const wxString& message, unsigned int delay, unsigned int hysteresis)
            : wxInfoWindowBase(Manager::Get()->GetAppWindow(),
#if !wxUSE_POPUPWIN
              wxID_ANY, wxEmptyString, wxPoint(-21,-21), wxSize(20,20),
#endif
              wxSIMPLE_BORDER | wxWS_EX_TRANSIENT | wxCLIP_CHILDREN),
              m_timer(new wxTimer(this, 0)), status(0), m_delay(delay), ks(2)
    {
        my_message_iterator = active_messages.insert(active_messages.begin(), message);

        wxBoxSizer *bs = new wxBoxSizer(wxVERTICAL);

        wxWindow* o = 0;

        ForwardingTextControl *titleC = 0;

        if(platform::gtk)
        {
            wxBoxSizer *pbs = new wxBoxSizer(wxVERTICAL);
            wxPanel* pnl = new wxPanel(this, -1, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
            pnl->SetBackgroundColour(titleBackground);
            titleC = new ForwardingTextControl(pnl, -1, title, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
            pbs->Add(titleC, 0, wxALL|wxALIGN_CENTER, 5);
            pnl->SetSizer(pbs);
            pbs->SetSizeHints(pnl);
            o = pnl;
        }
        else
        {
            titleC = new ForwardingTextControl(this, -1, title, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
            titleC->SetBackgroundColour(titleBackground);
            o = titleC;
        }

        titleC->SetForegroundColour(*wxWHITE);
        titleC->SetFont(wxFont(11, wxSWISS, wxNORMAL, wxBOLD));
        bs->Add(o, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5);

        ForwardingTextControl *text = new ForwardingTextControl(this, -1, message, wxDefaultPosition, wxDefaultSize, 0);
        text->SetBackgroundColour(textBackground);
        bs->Add(text, 0, wxALIGN_CENTER|wxALL, 12);
        SetBackgroundColour(textBackground);
        SetSizer(bs);
        bs->SetSizeHints(this);
        Layout();

        if(!platform::gtk)
        {
            // since we used a panel, no more bitmap :(
            new wxStaticBitmap(this, -1, wxBitmap(iBitmap), wxPoint(4, o->GetRect().GetBottom() - 9));
        }
        wxCoord w, h;
        GetClientSize(&w, &h);

        pos = stacker.StackMe(w);

        // setup variables first time we enter here
        if (screenWidth == -1)
            screenWidth = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
        if (screenHeight == -1)
            screenHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);

        left = screenWidth - pos;
        hMin = screenHeight - h;
        top = screenHeight;

        Move(left, top);

        Show();
        m_timer->Start(hysteresis, false);
    };


InfoWindow::~InfoWindow()
{
    delete m_timer;
    stacker.ReleaseMe(pos);

    active_messages.erase(my_message_iterator);
};

void InfoWindow::OnTimer(wxTimerEvent& e)
{
    switch(status)
    {
    case 0:
        status = 1;
        m_timer->Start(scroll_millis, false);
        break;
    case 1:
        top -= 2;
        Move(left, top);
        if(top <= hMin)
        {
            status = 2;
            m_timer->Start(m_delay, true);
        }
        break;
    case 2:
        status = 3;
        m_timer->Start(scroll_millis, false);
        break;
    case 3:
        top += ks;
        Move(left, top);
        if(top > screenHeight)
        {
            Hide();
            Destroy();
        }
        break;
    };
};

void InfoWindow::OnMove(wxMouseEvent& e)
{
    if(status == 2)
        m_timer->Start(m_delay, true);
}

void InfoWindow::OnClick(wxMouseEvent& e)
{
    ks = 6;
    status = 3;
    m_timer->Start(scroll_millis, false);
}

// static
void InfoWindow::Display(const wxString& title, const wxString& message, unsigned int delay, unsigned int hysteresis)
{
    if (std::find(active_messages.begin(), active_messages.end(), message) != active_messages.end())
    {
        const wxString dups = _T("Multiple information windows with the same\nmessage have been suppressed.");
        if (std::find(active_messages.begin(), active_messages.end(), dups) == active_messages.end())
            Display(_T("Info"), dups, delay);
        return; // currently displaying already
    }
    new InfoWindow(title, message, delay, hysteresis);
}
