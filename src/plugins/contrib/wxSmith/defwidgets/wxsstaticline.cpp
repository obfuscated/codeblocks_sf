#include "../wxsheaders.h"
#include "wxsstaticline.h"

#include "wx/statline.h"

WXS_ST_BEGIN(wxsStaticLineStyles)
    WXS_ST_CATEGORY("wxStaticLine")
    WXS_ST(wxLI_HORIZONTAL)
    WXS_ST(wxLI_VERTICAL)
WXS_ST_END(wxsStaticLineStyles)

WXS_EV_BEGIN(wxsStaticLineEvents)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsStaticLineEvents)


wxsDWDefineBegin(wxsStaticLineBase,wxStaticLine,
     WXS_THIS = new wxStaticLine(WXS_PARENT,WXS_ID,WXS_POS,WXS_SIZE,WXS_STYLE);
    )
wxsDWDefineEnd()

wxsStaticLine::wxsStaticLine(wxsWidgetManager* Man,wxsWindowRes* Res):
    wxsStaticLineBase(Man,Res)
{
    wxsBaseProperties& Params = GetBaseProperties();
    Params.DefaultSize = false;
    Params.SizeX = 10;
    Params.SizeY = -1;
}

bool wxsStaticLine::PropertiesUpdated(bool Validate,bool Correct)
{
    // Need to additionally check size params
    wxsBaseProperties& Params = GetBaseProperties();

    if ( GetStyle() & wxLI_VERTICAL )
    {
        if ( Params.SizeY == -1 )
        {
            Params.SizeY = Params.SizeX;
        }
        Params.SizeX = -1;
    }
    else
    {
        if ( Params.SizeX == -1 )
        {
            Params.SizeX = Params.SizeY;
        }
        Params.SizeY = -1;
    }

    return wxsWidget::PropertiesUpdated(Validate,Correct);
}
