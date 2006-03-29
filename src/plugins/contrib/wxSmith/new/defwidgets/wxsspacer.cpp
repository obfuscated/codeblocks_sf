#include "wxsspacer.h"

#include <messagemanager.h>

namespace
{
    class wxsSpacerPreview: public wxPanel
    {
        public:
            wxsSpacerPreview(wxWindow* Parent,const wxSize& Size):
                wxPanel(Parent,-1,wxDefaultPosition,Size)
            {}

        private:

            void OnPaint(wxPaintEvent& event)
            {
                wxPaintDC DC(this);
                DC.SetBrush(wxBrush(wxColour(0,0,0),wxCROSSDIAG_HATCH));
                DC.SetPen(wxPen(wxColour(0,0,0),1));
                DC.DrawRectangle(0,0,GetSize().GetWidth(),GetSize().GetHeight());
            }

            DECLARE_EVENT_TABLE()
    };

    BEGIN_EVENT_TABLE(wxsSpacerPreview,wxPanel)
        EVT_PAINT(wxsSpacerPreview::OnPaint)
    END_EVENT_TABLE()

}

wxsItemInfo wxsSpacer::Info =
{
    _T("spacer"),
    wxsTSizer,
    _("wxWidgets license"),
    _("wxWidgets team"),
    _T(""),
    _T("www.wxwidgets.org"),
    _T("Layout"),
    30,
    _T(""),
    2, 6,
    NULL,
    NULL,
    0
};

wxString wxsSpacer::GetSizeCode(const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
            return Size.GetSizeCode(WindowParent,wxsCPP) + _T(".GetWidth(),") +
                   Size.GetSizeCode(WindowParent,wxsCPP) + _T(".GetHeight()");
    }

    DBGLOG(_T("wxSmith: Unknown coding language when building spacer (id: %d)"),Language);
    return wxEmptyString;
}

long wxsSpacer::GetPropertiesFlags()
{
    return wxsItem::GetPropertiesFlags() & ~(wxsFLVariable|wxsFLId);
}

wxObject* wxsSpacer::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    if ( Exact ) return NULL;
    return new wxsSpacerPreview(Parent,Size.GetSize(Parent));
}
