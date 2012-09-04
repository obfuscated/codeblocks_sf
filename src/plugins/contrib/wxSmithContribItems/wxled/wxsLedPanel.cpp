#include "wxsLedPanel.h"

namespace
{
     #include "images/wxLedPanel16.xpm"
     #include "images/wxLedPanel32.xpm"

    wxsRegisterItem<wxsLedPanel> Reg(
        _T("wxLEDPanel"),
        wxsTWidget,
        _T("wxWindows"),
        _T("Christian Graefe"),
        _T("graefe@NOSPAM!@mcs-soft.de"),
        _T("http://wxcode.sourceforge.net/components/ledpanel/"),
        _T("Led"),
        80,
        _T("LedPanel"),
        wxsCPP,
        1, 0,
        wxBitmap(wxLedPanel32_xpm),
        wxBitmap(wxLedPanel16_xpm),
        false);

    static const long    Values[] = { wxLED_COLOUR_RED, wxLED_COLOUR_CYAN, wxLED_COLOUR_MAGENTA, wxLED_COLOUR_BLUE, wxLED_COLOUR_GREEN, wxLED_COLOUR_YELLOW, wxLED_COLOUR_GREY };
    static const wxChar* Names[]  = { _("Rot"), _("Cyan"), _("Magenta"), _("Blau"), _("Gruen"), _("Gelb"), _("Grau"), NULL }; // Must end with NULL entry

    static const long    ValuesDirection[] = { wxALIGN_RIGHT, wxALIGN_LEFT, wxALIGN_CENTER, wxALIGN_TOP, wxALIGN_BOTTOM };
    static const wxChar* NamesDirection[]  = { _T("Rechts"), _T("Links"), _T("Mittig"), _T("Oben"), _T("Unten"), NULL };

    static const long    ValuesContent[] = { wxALIGN_CENTER_VERTICAL, wxALIGN_CENTER_HORIZONTAL, wxALIGN_CENTER};
    static const wxChar* NamesContent[]  = { _T("Mittig vertical"), _T("Mittig Horizontal"), _T( "Mittig"), NULL};

    static const long    ValuesScroll[] = { 0, wxLEFT, wxRIGHT, wxUP, wxDOWN};
    static const wxChar* NamesScroll[]  = { _("Kein Scroll"), _("Links"), _("Rechts"), _("Hoch"), _("Runter"), NULL};
}

wxsLedPanel::wxsLedPanel(wxsItemResData* Data) : wxsWidget( Data, &Reg.Info, NULL, NULL, flId | flVariable)
{
    //ctor
    LedMatrixSize.X      = 4;
    LedMatrixSize.Y      = 4;
    LedMatrix.X          = 65;
    LedMatrix.Y          = 9;
    Space                = 0;
    Colour               = wxLED_COLOUR_RED;
    Invert               = false;
    Inactiv              = true;
    Text                 = _("");
    TextPlace            = 1;
    Bold                 = false;
    Align                = wxALIGN_CENTER;
    ContentAlign         = wxALIGN_CENTER;
    ScrollDirection      = 0;
    ScrollSpeed          = 0;
}

wxsLedPanel::~wxsLedPanel()
{
    //dtor
}

void wxsLedPanel::OnBuildCreatingCode()
{

    if( LedMatrixSize.X == -1)
        LedMatrixSize.X = 4;
    if( LedMatrixSize.Y == -1)
        LedMatrixSize.Y = 4;
    if( LedMatrix.X == -1)
        LedMatrix.X = 65;
    if( LedMatrix.Y == -1)
        LedMatrix.Y = 9;

    switch ( GetLanguage() )
    {
        case wxsCPP:
            AddHeader(_T("<wx/wxledpanel.h>"),GetInfo().ClassName);
            Codef( _T("%C(%W,%I,wxSize( %ld, %ld),wxSize( %ld, %ld),%ld);\n"), LedMatrixSize.X, LedMatrixSize.Y, LedMatrix.X, LedMatrix.Y, Space);
            Codef( _T( "%ASetContentAlign( %ld);\n"), ContentAlign);
            Codef( _T( "%ASetLEDColour( (wxLEDColour)%ld);\n"), Colour);
            Codef( _T( "%AShowInvertet(%b);\n"), Invert);
            Codef( _T( "%AShowInactivLEDs( %b);\n"), Inactiv);
            if( ScrollDirection != 0)
            {
                Codef( _T( "%ASetScrollDirection( (wxDirection)%ld);\n"), ScrollDirection);
                Codef( _T( "%ASetScrollSpeed( %ld);\n"), ScrollSpeed);
            }
            if( Text.Len() > 0)
            {
                Codef( _T( "%ASetLetterSpace( %ld);\n"), TextPlace);
                if( Bold)
                    Codef( _T("%ASetFontType( wxLEDFont7x7);\n"));
                Codef( _T( "%ASetText( _T(\"%s\"), %ld);\n"), Text.wx_str(), Align);
            }
            break;

        default:
            wxsCodeMarks::Unknown(_T("wxsLedPanel::OnBuildCreatingCode"),GetLanguage());
    }
}

wxObject* wxsLedPanel::OnBuildPreview(wxWindow* Parent,long Flags)
{

    wxLEDPanel* test = new wxLEDPanel(Parent,GetId(),wxSize( LedMatrixSize.X, LedMatrixSize.X),wxSize(LedMatrix.X,LedMatrix.Y),Space);
    test->SetContentAlign( ContentAlign);
    test->SetLEDColour( (wxLEDColour)Colour);
    test->ShowInvertet( Invert);
    test->ShowInactivLEDs( Inactiv);
    if( Text.Len() > 0)
    {
        test->SetLetterSpace( TextPlace);
        if( Bold)
            test->SetFontType( wxLEDFont7x7);
        test->SetText( Text, Align);
    }
    return test;
}

void wxsLedPanel::OnEnumWidgetProperties(long Flags)
{

    WXS_SIZE(
             wxsLedPanel,
             LedMatrixSize,
             _("LedMatrix Size"),
             _("Width"),
             _("Height"),
             _("Dialog-Units"),
             _T("ledMatrix_Size"));

    WXS_SIZE(
             wxsLedPanel,
             LedMatrix,
             _("LedCtrl Size"),
             _("Width"),
             _("Height"),
             _("Dialog-Units"),
             _T("ledCtrl_Size"));

    WXS_LONG(
             wxsLedPanel,
             Space,
             _("Space between led"),
             _T("Space"),
             0);

    WXS_ENUM(
             wxsLedPanel,
             Colour,
             _("Led Colour"),
             _T("Led_Colour"),
             Values,
             Names,
             wxLED_COLOUR_RED);

    WXS_BOOL(
             wxsLedPanel,
             Invert,
             _("Invert"),
             _T("Invert_Leds"),
             false);

    WXS_BOOL(
             wxsLedPanel,
             Inactiv,
             _("Inactive"),
             _T("Inactive_Leds"),
             true);

    WXS_FLAGS(
              wxsLedPanel,
              ContentAlign,
              _("Content Align"),
              _T("Content_Align"),
              ValuesContent,
              NamesContent,
              wxALIGN_CENTER );

    WXS_STRING(
               wxsLedPanel,
               Text,
               _("Text"),
               _T("Text"),
               _T(""),
               false);

    WXS_LONG(
             wxsLedPanel,
             TextPlace,
             _("Text Space"),
             _T("Text_Space"),
             1);

    WXS_BOOL(
             wxsLedPanel,
             Bold,
             _("Bold"),
             _T("Bold"),
             false);

    WXS_FLAGS(
              wxsLedPanel,
              Align,
              _("Align"),
              _T("Align"),
              ValuesDirection,
              NamesDirection,
              wxALIGN_CENTER );

    WXS_ENUM(
             wxsLedPanel,
             ScrollDirection,
             _("Scroll"),
             _T("Scroll"),
             ValuesScroll,
             NamesScroll,
             0);

    WXS_LONG(
             wxsLedPanel,
             ScrollSpeed,
             _("Scroll Speed"),
             _T("Scroll_Speed"),
             0);
}
