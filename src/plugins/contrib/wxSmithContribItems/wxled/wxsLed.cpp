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
        true);
}

wxsLed::wxsLed(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        NULL,
        NULL,
        flVariable | flId|flEnabled),
        m_State(true)
{
    //ctor
    m_Disable = wxColour(128,128,128);
    m_EnableOn  = *wxGREEN;
    m_EnableOff  = *wxRED;
    GetBaseProps()->m_Enabled = false;
}

wxsLed::~wxsLed()
{
    //dtor
}

void wxsLed::OnBuildCreatingCode()
{

    wxString ss  = m_Disable.BuildCode(GetCoderContext());
    wxString ss2 = m_EnableOn.BuildCode(GetCoderContext());
    wxString ss3 = m_EnableOff.BuildCode(GetCoderContext());

    switch ( GetLanguage())
    {
        case wxsCPP:
            AddHeader(_T("<wx/led.h>"),GetInfo().ClassName);
            Codef(_T("%C(%W,%I,%s,%s,%s,%P,%S);\n"), ss.wx_str(), ss2.wx_str(), ss3.wx_str());
            if ( !GetBaseProps()->m_Enabled)
                Codef(_T("%ADisable();\n"));
            if(m_State)
                Codef(_T("%ASwitchOn();\n"));
            else
                Codef(_T("%ASwitchOff();\n"));
            break;

        default:
            wxsCodeMarks::Unknown(_T("wxsLed::OnBuildCreatingCode"),GetLanguage());
    }
}

wxObject* wxsLed::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxLed *Led = new wxLed(Parent,GetId(),m_Disable.GetColour(), m_EnableOn.GetColour(), m_EnableOff.GetColour(),Pos(Parent),Size(Parent));
    if( !GetBaseProps()->m_Enabled)
        Led->Disable();
    if( m_State)
        Led->SwitchOn();
    else
        Led->SwitchOff();
    return Led;
}

void wxsLed::OnEnumWidgetProperties(long Flags)
{

    WXS_COLOUR(
    wxsLed,
    m_Disable,
    _("Disable Colour"),
    _T("disable_colour"));

    WXS_COLOUR(
    wxsLed,
    m_EnableOn,
    _("On Colour"),
    _T("on_colour"));

    WXS_COLOUR(
    wxsLed,
    m_EnableOff,
    _("Off Colour"),
    _T("off_colour"));

    WXS_BOOL(
        wxsLed,
        m_State,
        _("On"),
        _T("on_or_off"),
        true
    );
}

/*bool wxsLed::OnXmlWrite(TiXmlElement* Element, bool IsXRC, bool IsExtra)
{

    TiXmlElement* ColourElem = Element->InsertEndChild(TiXmlElement("colour"))->ToElement();
    ColourElem->SetAttribute("enableon_colour",cbU2C(m_EnableOn.GetColour().GetAsString()));
    ColourElem->SetAttribute("enableoff_colour",cbU2C(m_EnableOff.GetColour().GetAsString()));
    ColourElem->SetAttribute("disable_color",cbU2C(m_Disable.GetColour().GetAsString()));
    ColourElem->SetAttribute("on_or_off_state",m_State);

    return wxsWidget::OnXmlWrite(Element, IsXRC, IsExtra);
}

bool wxsLed::OnXmlRead(TiXmlElement* Element, bool IsXRC, bool IsExtra)
{

    TiXmlElement* ColourElem = Element->FirstChildElement("disable_colour");
    if(ColourElem != NULL)
    {
        m_Disable.GetColour().Set(cbC2U(Element->Value()));
    }
    ColourElem = Element->FirstChildElement("on_colour");
    if(ColourElem != NULL)
    {
        m_EnableOn.GetColour().Set(cbC2U(Element->Value()));
    }
    ColourElem = Element->FirstChildElement("off_colour");
    if(ColourElem != NULL)
    {
        m_EnableOff.GetColour().Set(cbC2U(Element->Value()));
    }
    ColourElem = Element->FirstChildElement("on_or_off");
    if(ColourElem != NULL)
    {
        m_State = static_cast<bool>(wxAtoi(cbC2U(Element->Value())));
    }

    return wxsWidget::OnXmlRead(Element, IsXRC, IsExtra);
}*/
