
// Interface Dependencies ---------------------------------------------
#ifndef HooverLine_h
#define HooverLine_h

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "HooverDrawlet.h"

// END Interface Dependencies -----------------------------------------

class HooverLine : public HooverDrawlet
{
public:
    HooverLine(wxPoint start, wxPoint stop);
    virtual ~HooverLine();


    virtual bool Draw(wxDC &dc);
    virtual void UnDraw(wxDC &dc);

private:
    wxPoint m_start, m_stop;
};

#endif




