#include "wxsLcdWindow.h"

#include "wx/lcdwindow.h"

namespace
{

    #include "images/wxLcdWindow16.xpm"
    #include "images/wxLcdWindow32.xpm"

    wxsRegisterItem<wxsLcdWindow> Reg(
        _T("wxLCDWindow"),
        wxsTWidget,
        _T("wxWindows"),
        _T(""),
        _T(""),
        _T("http://wxcode.sourceforge.net/components/lcdwindow"),
        _T("Led"),
        80,
        _T("LcdWindow"),
        wxsCPP,
        1, 0,
        wxBitmap(wxLcdWindow32_xpm),
        wxBitmap(wxLcdWindow16_xpm),
        false);
}

wxsLcdWindow::wxsLcdWindow(wxsItemResData* Data) : wxsWidget( Data, &Reg.Info, NULL, NULL, flVariable | flId | flPosition | flSize | flMinMaxSize | flExtraCode)
{
    //ctor

    NumberOfDigits    = 8;
    Content           = _("");
    ColourLight       = wxColour( 0, 255, 0);
    ColourGray        = wxColour( 0, 64, 0);
    BackGround        = wxColour( 0, 0, 0);
}

wxsLcdWindow::~wxsLcdWindow()
{
    //dtor
}

void wxsLcdWindow::OnBuildCreatingCode()
{

    wxString s1 = ColourLight.BuildCode(GetCoderContext()).c_str();
    wxString s2 = ColourGray.BuildCode(GetCoderContext()).c_str();
    wxString s3 = BackGround.BuildCode( GetCoderContext()).c_str();

    switch ( GetLanguage())
    {
        case wxsCPP:
            AddHeader(_T("<wx/lcdwindow.h>"),GetInfo().ClassName);
            Codef(_T("%C(%W,%P,%S);\n"));
            Codef( _T( "%ASetNumberDigits( %d);\n"), static_cast<int>(NumberOfDigits));
            if( ColourLight.GetColour() != wxColour( 00, 255, 00))
                Codef( _T( "%ASetLightColour( %s);\n"), s1.wx_str());
            if( ColourGray.GetColour() != wxColour( 00, 64, 00))
                Codef( _T( "%ASetGrayColour( %s);\n"), s2.wx_str());
            if( BackGround.GetColour() != wxColour( 0, 0, 0))
                Codef( _T( "%ASetBackgroundColour( %s);\n"), s3.wx_str());
            if( Content.Len() > 0)
                Codef( _T( "%ASetValue( _T(\"%s\"));\n"), Content.wx_str());
            break;

        default:
            wxsCodeMarks::Unknown(_T("wxsLcdWindow::OnBuildCreatingCode"),GetLanguage());
    }
}

wxObject* wxsLcdWindow::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxLCDWindow* test = new wxLCDWindow( Parent, Pos( Parent), Size( Parent));
    test->SetNumberDigits( NumberOfDigits);
    if( Content.Len() > 0)
        test->SetValue( Content);
    if( ColourLight.GetColour() != wxColour( 00, 255, 00))
        test->SetLightColour( ColourLight.GetColour());
    if( ColourGray.GetColour() != wxColour( 00, 64, 00))
        test->SetGrayColour( ColourGray.GetColour());
    if( BackGround.GetColour() != wxColour( 0,0,0))
        test->SetBackgroundColour( BackGround.GetColour());

    return test;
}

void wxsLcdWindow::OnEnumWidgetProperties(long Flags)
{

    WXS_LONG(
             wxsLcdWindow,
             NumberOfDigits,
             _("Number of Digits"),
             _T("Number_Digits"),
             8);

    WXS_SHORT_STRING(
                     wxsLcdWindow,
                     Content,
                     _("Content"),
                     _T("Content"),
                     ::wxNow().Mid( 11, 8),
                     false);
    WXS_COLOUR(
               wxsLcdWindow,
               ColourLight,
               _("Light Colour"),
               _T("Light_Colour"));

    WXS_COLOUR(
                wxsLcdWindow,
                ColourGray,
                _("Back Colour"),
               _T("Back_Colour"));
    WXS_COLOUR(
                wxsLcdWindow,
                BackGround,
                _("Background Colour"),
                _T("Background"));
}
