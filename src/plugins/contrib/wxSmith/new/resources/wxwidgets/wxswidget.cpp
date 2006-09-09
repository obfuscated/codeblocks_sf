#include "wxswidget.h"

#include <messagemanager.h>

wxsWidget::wxsWidget(
    wxsWindowRes* Resource,
    long _BasePropertiesFlags,
    const wxsItemInfo* _Info,
    const wxsEventDesc* _EventArray,
    const wxsStyle* _StyleSet,
    const wxString& _DefaultStyle):
        wxsItem(Resource),
        Info(_Info),
        EventArray(_EventArray),
        StyleSet(_StyleSet),
        DefaultStyle(_DefaultStyle),
        BasePropertiesFlags(_BasePropertiesFlags)
{
    if ( StyleSet )
    {
        wxsStyleProperty::SetFromString(StyleBits,DefaultStyle,StyleSet,false);
    }
    else
    {
        StyleBits = 0;
    }
}

void wxsWidget::EnumItemProperties(long Flags)
{
    EnumWidgetProperties(Flags);
    wxsItem::EnumItemProperties(Flags);
    SubContainer(BaseProps,GetBasePropertiesFlags());
    if ( StyleSet )
    {
        WXS_STYLE(wxsWidget,StyleBits,0,_("Style"),_T("style"),StyleSet,DefaultStyle);
        WXS_EXSTYLE(wxsWidget,ExStyleBits,0,_("Extra style"),_T("exstyle"),StyleSet,wxEmptyString);
    }
}

void wxsWidget::AddItemQPP(wxsAdvQPP* QPP)
{
    AddWidgetQPP(QPP);
    BaseProps.AddQPPChild(QPP,GetBasePropertiesFlags());
}

wxWindow* wxsWidget::SetupWindow(wxWindow* Preview,bool IsExact)
{
    BaseProps.SetupWindow(Preview,IsExact);
    long ExStyle = wxsStyleProperty::GetWxStyle(ExStyleBits,StyleSet,true);
    if ( ExStyle != 0 )
    {
        Preview->SetExtraStyle(Preview->GetExtraStyle() | ExStyle);
    }
    return Preview;
}

void wxsWidget::SetupWindowCode(wxString& Code,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            BaseProps.BuildSetupWindowCode(Code,GetVarName(),wxsCPP);
            if ( ExStyleBits )
            {
                wxString ExStyleStr = wxsStyleProperty::GetString(ExStyleBits,StyleSet,true,wxsCPP);
                if ( ExStyleStr != _T("0") )
                {
                    Code << GetVarName() << _T("->SetExtraStyle(") << GetVarName()
                         << _T("->GetExtraStyle() | ") << ExStyleStr << _T(");\n");
                }
            }
            return;
        }
    }

    wxsLANGMSG(wxsWidget::SetupWindowCode,Language);
}
