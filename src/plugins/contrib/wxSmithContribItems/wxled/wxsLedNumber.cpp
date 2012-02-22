#include "wxsLedNumber.h"

#include "wx/gizmos/ledctrl.h"

namespace
{

    #include "images/LedNumber16.xpm"
    #include "images/LedNumber32.xpm"

    wxsRegisterItem<wxsLedNumber> Reg(
        _T("wxLEDNumberCtrl"),
        wxsTWidget,
        _T("wxWindows"),
        _T("Matt Kimball"),
        _T(""),
        _T("http://wxcode.sourceforge.net/complist.php"),
        _T("Led"),
        80,
        _T("LedNumber"),
        wxsCPP,
        1, 0,
        wxBitmap(LedNumber32_xpm),
        wxBitmap(LedNumber16_xpm),
        false);

        static const long    Values[] = { wxLED_ALIGN_LEFT, wxLED_ALIGN_CENTER, wxLED_ALIGN_RIGHT};
        static const wxChar* Names[]  = { _("Left"), _("Center"), _("Right"), NULL }; // Must end with NULL entry
}

wxsLedNumber::wxsLedNumber(wxsItemResData* Data) : wxsWidget( Data, &Reg.Info, NULL, NULL, flVariable | flId | flPosition | flSize | flColours | flMinMaxSize | flExtraCode)
{
    //ctor
    Content      = _("");
    Align        = wxLED_ALIGN_LEFT;
    Faded        = true;
    GetBaseProps()->m_Fg = wxColour( 0, 255, 0);
    GetBaseProps()->m_Bg = wxColor( 0 ,0 ,0);
}

wxsLedNumber::~wxsLedNumber()
{
    //dtor
}

void wxsLedNumber::OnBuildCreatingCode()
{

    wxString FGCol = GetBaseProps()->m_Fg.BuildCode(GetCoderContext());
    wxString BGCol = GetBaseProps()->m_Bg.BuildCode(GetCoderContext());

    switch ( GetLanguage() )
    {
        case wxsCPP:
            AddHeader(_T("<wx/gizmos/ledctrl.h>"),GetInfo().ClassName);
            Codef(_T("%C(%W,%I,%P,%S,%d|wxFULL_REPAINT_ON_RESIZE %s);\n"), Align, (Faded ? _("| wxLED_DRAW_FADED") : _("")));
            Codef( _T( "%ASetMinSize( %S);\n"));
            #if wxCHECK_VERSION(2, 9, 0)
                if ( !FGCol.empty() )
                    Codef(_T("%ASetForegroundColour(%s);\n"),FGCol.wx_str());
                if ( !BGCol.empty() )
                    Codef(_T("%ASetBackgroundColour(%s);\n"),BGCol.wx_str());
            #else
                if ( !FGCol.empty() )
                   Codef(_T("%ASetForegroundColour(%s);\n"),FGCol.c_str());
                if ( !BGCol.empty() )
                    Codef(_T("%ASetBackgroundColour(%s);\n"),BGCol.c_str());
            #endif
            if( Content.Len() > 0)
                Codef( _T( "%ASetValue( _T(\"%s\"));\n"), Content.c_str());
            break;

        default:
            wxsCodeMarks::Unknown(_T("wxsLedNumber::OnBuildCreatingCode"),GetLanguage());
    }
}

wxObject* wxsLedNumber::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxLEDNumberCtrl* test = new wxLEDNumberCtrl(Parent,GetId(),Pos(Parent),Size(Parent), Align|wxFULL_REPAINT_ON_RESIZE);
    test->SetMinSize( Size( Parent));

    test->SetForegroundColour(GetBaseProps()->m_Fg.GetColour());
    test->SetBackgroundColour(GetBaseProps()->m_Bg.GetColour());

    if( Content.Len() > 0)
        test->SetValue( Content);

    test->SetDrawFaded( Faded);

    return test;
}

void wxsLedNumber::OnEnumWidgetProperties(long Flags)
{

    WXS_SHORT_STRING(
                wxsLedNumber,
                Content,
                _("Content"),
                _T("Content"),
                _T(""),
                false);


    WXS_ENUM(
                wxsLedNumber,
                Align,
                _("Align"),
                _T("Align"),
                Values,
                Names,
                wxLED_ALIGN_LEFT);

    WXS_BOOL(
             wxsLedNumber,
             Faded,
             _("Faded"),
             _T("Faded"),
             true);
}

