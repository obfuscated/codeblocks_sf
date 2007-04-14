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

}

wxsChart::wxsChart(wxsItemResData* Data):
    wxsWidget(
        Data,               // Data passed to constructor
        &Reg.Info,          // Info taken from Registering object previously created
        NULL,               // Structure describing events, we have no events for wxChart
        wxsChartStyles)     // Structure describing styles
{
}

wxsChart::~wxsChart()
{
}


void wxsChart::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
            Codef(_T("%C(%W,%I,DEFAULT_STYLE,%P,%S,%T);\n"));
            break;

        default:
            wxsCodeMarks::Unknown(_T("wxsChart::OnBuildCreatingCode"),Language);
    }
}

wxObject* wxsChart::OnBuildPreview(wxWindow* Parent,long Flags)
{
    return new wxChartCtrl(Parent,GetId(),DEFAULT_STYLE,Pos(Parent),Size(Parent),Style());
}

void wxsChart::OnEnumWidgetProperties(long Flags)
{
}

void wxsChart::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/chart.h>")); break;
        default: wxsCodeMarks::Unknown(_T("wxsChart::OnEnumDeclFiles"),Language);
    }
}
