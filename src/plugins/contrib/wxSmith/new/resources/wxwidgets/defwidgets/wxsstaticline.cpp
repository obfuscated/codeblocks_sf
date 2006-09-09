#include "wxsstaticline.h"

#include <wx/statline.h>
#include <messagemanager.h>


WXS_ST_BEGIN(wxsStaticLineStyles)
    WXS_ST_CATEGORY("wxStaticLine")
    WXS_ST(wxLI_HORIZONTAL)
    WXS_ST(wxLI_VERTICAL)
WXS_ST_END()


WXS_EV_BEGIN(wxsStaticLineEvents)
    WXS_EV_DEFAULTS()
WXS_EV_END()


wxsItemInfo wxsStaticLine::Info =
{
    _T("wxStaticLine"),
    wxsTWidget,
    _("wxWidgets license"),
    _("wxWidgets team"),
    _T(""),
    _T("www.wxwidgets.org"),
    _T("Standard"),
    60,
    _T("StaticLine"),
    2, 6,
    NULL,
    NULL,
    0
};


wxsStaticLine::wxsStaticLine(wxsWindowRes* Resource):
    wxsWidget(
        Resource,
        wxsBaseProperties::flAll,
        &Info,
        wxsStaticLineEvents,
        wxsStaticLineStyles,
        _T(""))

{
    // Default the size so that it can be seen in the edit mode
    BaseProps.Size.IsDefault = false;
    BaseProps.Size.X = 10;
    BaseProps.Size.Y = -1;
}



void wxsStaticLine::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code<< GetVarName() << _T(" = new wxStaticLine(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");

            SetupWindowCode(Code,Language);
            return;
        }
    }

    wxsLANGMSG(wxsStaticLine::BuildCreatingCode,Language);
}


wxObject* wxsStaticLine::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    wxStaticLine* Preview = new wxStaticLine(Parent,GetId(),Pos(Parent),Size(Parent),Style());

    return SetupWindow(Preview,Exact);
}

void wxsStaticLine::EnumWidgetProperties(long Flags)
{
//    WXS_STRING(wxsStaticLine,Label,0,_("Label"),_T("label"),_T(""),true,false)

}

void wxsStaticLine::EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/statline.h>")); return;
    }

    wxsLANGMSG(wxsStaticLine::EnumDeclFiles,Language);
}
