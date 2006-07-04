#include "sdk_precomp.h"

BEGIN_EVENT_TABLE(InfoWindow, wxPopupWindow)
EVT_TIMER(-1, InfoWindow::OnTimer)
EVT_MOTION(InfoWindow::OnMove)
EVT_LEFT_DOWN(InfoWindow::OnClick)
EVT_RIGHT_DOWN(InfoWindow::OnClick)
END_EVENT_TABLE()

const wxColour titleBackground(96,96,96);
const wxColour textBackground(245,245,245);


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
int InfoWindow::screenWidth = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
int InfoWindow::screenHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);


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
            : wxPopupWindow(Manager::Get()->GetAppWindow(), wxSIMPLE_BORDER | wxWS_EX_TRANSIENT | wxCLIP_CHILDREN),
              m_timer(new wxTimer(this, 0)), status(0), m_delay(delay), ks(2)
    {
        wxBoxSizer *bs = new wxBoxSizer(wxVERTICAL);

        ForwardingTextControl *titleC = new ForwardingTextControl(this, -1, title, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
        titleC->SetForegroundColour(*wxWHITE);
        titleC->SetBackgroundColour(titleBackground);
        titleC->SetFont(wxFont(11, wxSWISS, wxNORMAL, wxBOLD));
        bs->Add(titleC, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5);

        ForwardingTextControl *text = new ForwardingTextControl(this, -1, message, wxDefaultPosition, wxDefaultSize, 0);
        text->SetBackgroundColour(textBackground);
        bs->Add(text, 0, wxALIGN_CENTER|wxALL, 12);
        SetBackgroundColour(textBackground);
        SetSizer(bs);
        bs->SetSizeHints(this);
        Layout();

        new wxStaticBitmap(this, -1, wxBitmap(iBitmap), wxPoint(4, titleC->GetRect().GetBottom() - 9));

        wxCoord w, h;
        GetClientSize(&w, &h);

        pos = stacker.StackMe(w);

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
    };

void InfoWindow::OnTimer(wxTimerEvent& e)
{
    switch(status)
    {
    case 0:
        status = 1;
        m_timer->Start(1, false);
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
        m_timer->Start(1, false);
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
    m_timer->Start(1, false);
}

