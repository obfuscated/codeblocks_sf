#include "wxsLed.h"
#include "wx/led.h"
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
    m_Enable  = *wxGREEN;
    GetBaseProps()->m_Enabled = false;
}

wxsLed::~wxsLed()
{
    //dtor
}

void wxsLed::OnBuildCreatingCode()
{

    wxString ss  = m_Disable.BuildCode(GetCoderContext());
    wxString ss2 = m_Enable.BuildCode(GetCoderContext());

    switch ( GetLanguage())
    {
        case wxsCPP:
            AddHeader(_T("<wx/led.h>"),GetInfo().ClassName);
            Codef(_T("%C(%W,%I,%s,%s,%P,%S);\n"), ss.wx_str(), ss2.wx_str());
            if ( !GetBaseProps()->m_Enabled)
                Codef(_T("%ADisable();\n"));
            break;

        default:
            wxsCodeMarks::Unknown(_T("wxsLed::OnBuildCreatingCode"),GetLanguage());
    }
}

wxObject* wxsLed::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxLed *Led = new wxLed(Parent,GetId(),m_Disable.GetColour(), m_Enable.GetColour(),Pos(Parent),Size(Parent));
    if( !GetBaseProps()->m_Enabled)
        Led->Disable();
    return Led;
}

void wxsLed::OnEnumWidgetProperties(long Flags)
{

    WXS_COLOUR(
    wxsLed,
    m_Disable,
    _("Disable Colour"),
    _T("Disable_Colour"));

    WXS_COLOUR(
    wxsLed,
    m_Enable,
    _("Enable Colour"),
    _T("Enable_Colour"));
}
