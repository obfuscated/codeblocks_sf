#include "wxsLed.h"
#include "wx/led.h"

#include <prep.h>

namespace
{
    #include "images/wxled16.xpm"
    #include "images/wxled32.xpm"

    wxsRegisterItem<wxsLed> Reg(
        _T("wxLed"),
        wxsTWidget,
        _T("wxWindows"),
        _T("Thomas Monjalon"),
        _T(""),
        _T(""),
        _T("Led"),
        80,
        _T("Led"),
        wxsCPP,
        1, 0,
        wxBitmap(wxled32),
        wxBitmap(wxled16),
        false);
}

wxsLed::wxsLed(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        NULL,
        NULL,
        flVariable | flId|flEnabled)
{
    //ctor
    m_Disable = *wxBLACK;
    m_On      = *wxGREEN;
    m_Off     = *wxRED;
    GetBaseProps()->m_Enabled = false;
}

wxsLed::~wxsLed()
{
    //dtor
}

void wxsLed::OnBuildCreatingCode()
{
    wxString dis = m_Disable.BuildCode(GetCoderContext());
    wxString on  = m_On.BuildCode(GetCoderContext());
    wxString off = m_Off.BuildCode(GetCoderContext());

    switch ( GetLanguage())
    {
        case wxsCPP:
            AddHeader(_T("<wx/led.h>"),GetInfo().ClassName);
            Codef(_T("%C(%W,%I,%s,%s,%s,%P,%S);\n"), dis.wx_str(), on.wx_str(), off.wx_str());
            if ( !GetBaseProps()->m_Enabled)
                Codef(_T("%ADisable();\n"));
            break;

        case wxsUnknownLanguage: // fall-through
        default:
            wxsCodeMarks::Unknown(_T("wxsLed::OnBuildCreatingCode"),GetLanguage());
    }
}

wxObject* wxsLed::OnBuildPreview(wxWindow* Parent,cb_unused long Flags)
{
    wxLed *Led = new wxLed(Parent,GetId(),m_Disable.GetColour(),m_On.GetColour(),
                           m_Off.GetColour(),Pos(Parent),Size(Parent));
    if (!GetBaseProps()->m_Enabled)
        Led->Disable();
    return Led;
}

void wxsLed::OnEnumWidgetProperties(cb_unused long Flags)
{
    WXS_COLOUR(
    wxsLed,
    m_Disable,
    _("Disable Colour"),
    _T("Disable_Colour"));

    WXS_COLOUR(
    wxsLed,
    m_On,
    _("On Colour"),
    _T("On_Colour"));

    WXS_COLOUR(
    wxsLed,
    m_Off,
    _("Off Colour"),
    _T("Off_Colour"));
}
