#include "wxschart.h"

#include <wx/chartctrl.h>

namespace
{
    // Loading images from xpm files
    #include "wxchart16.xpm"
    #include "wxchart32.xpm"

    // This code provides basic informations about item and register
    // it inside wxSmith
    wxsRegisterItem<wxsChart> Reg(
        _T("wxChartCtrl"),              // Class name
        wxsTWidget,                     // Item type
        _T("wxWindows"),                // License
        _T("Paolo Gava"),               // Author
        _T("paolo_gava@hotmail.com"),   // Author's email
        _T("http://wxcode.sourceforge.net/components/wxchart/"),    // Item's homepage
        _T("Contrib"),                  // Category in palette
        80,                             // Priority in palette
        _T("Chart"),                    // Base part of names for new items
        wxsCPP,                         // List of coding languages supported by this item
        1, 0,                           // Version
        wxBitmap(wxchart32_xpm),        // 32x32 bitmap
        wxBitmap(wxchart16_xpm),        // 16x16 bitmap
        false);                         // We do not allow this item inside XRC files

    // Defining styles
    WXS_ST_BEGIN(wxsChartStyles,_T("wxSIMPLE_BORDER"))
        WXS_ST_DEFAULTS()
    WXS_ST_END()


    static const long DEFAULT_STYLE_FIX = 0x1000;
    static const long Values[] = { USE_AXIS_X, USE_AXIS_Y, USE_LEGEND, USE_ZOOM_BUT, USE_DEPTH_BUT, USE_GRID, DEFAULT_STYLE_FIX };
    static const wxChar* Names[] = { _T("USE_AXIS_X"), _T("USE_AXIS_Y"), _T("USE_LEGEND"), _T("USE_ZOOM_BUT"), _T("USE_DEPTH_BUT"), _T("USE_GRID"), _T("DEFAULT_STYLE"), NULL };

}

wxsChart::wxsChart(wxsItemResData* Data):
    wxsWidget(
        Data,               // Data passed to constructor
        &Reg.Info,          // Info taken from Registering object previously created
        NULL,               // Structure describing events, we have no events for wxChart
        wxsChartStyles)     // Structure describing styles
{
    m_Flags = DEFAULT_STYLE_FIX;
}

wxsChart::~wxsChart()
{
}


void wxsChart::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            wxString StyleCode;
            for ( int i=0; Names[i]; i++ )
            {
                if ( m_Flags & Values[i] ) StyleCode << Names[i] << _T("|");
            }

            if ( StyleCode.IsEmpty() ) StyleCode = _T("0");
            else                       StyleCode.RemoveLast();

            Codef(_T("%C(%W,%I,(STYLE)(%s),%P,%S,%T);\n"),StyleCode.c_str());
            break;
        }

        default:
            wxsCodeMarks::Unknown(_T("wxsChart::OnBuildCreatingCode"),Language);
    }
}

wxObject* wxsChart::OnBuildPreview(wxWindow* Parent,long Flags)
{
    long RealFlags = m_Flags;
    if ( RealFlags & DEFAULT_STYLE_FIX ) RealFlags |= DEFAULT_STYLE;
    return new wxChartCtrl(Parent,GetId(),(STYLE)RealFlags,Pos(Parent),Size(Parent),Style());
}

void wxsChart::OnEnumWidgetProperties(long Flags)
{
    WXS_FLAGS(wxsChart,m_Flags,_("wxChart style"),_T("wxchart_style"),Values,Names, DEFAULT_STYLE_FIX )
}

void wxsChart::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/chart.h>")); break;
        default: wxsCodeMarks::Unknown(_T("wxsChart::OnEnumDeclFiles"),Language);
    }
}
