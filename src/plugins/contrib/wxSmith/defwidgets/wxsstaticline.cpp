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
     WXS_THIS = new wxStaticLine(WXS_PARENT,WXS_ID,WXS_POS,WXS_SIZE,WXS_STYLE,WXS_NAME);
    )
wxsDWDefineEnd()

wxsStaticLine::wxsStaticLine(wxsWidgetManager* Man,wxsWindowRes* Res):
    wxsStaticLineBase(Man,Res)
{
    BaseProperties.DefaultSize = false;
    BaseProperties.SizeX = 10;
    BaseProperties.SizeY = -1;
}

bool wxsStaticLine::MyPropertiesUpdated(bool Validate,bool Correct)
{
    // Need to additionally check size params
    if ( GetStyle() & wxLI_VERTICAL )
    {
        if ( BaseProperties.SizeY == -1 )
        {
            BaseProperties.SizeY = BaseProperties.SizeX;
        }
        BaseProperties.SizeX = -1;
    }
    else
    {
        if ( BaseProperties.SizeX == -1 )
        {
            BaseProperties.SizeX = BaseProperties.SizeY;
        }
        BaseProperties.SizeY = -1;
    }

    return true;
}
