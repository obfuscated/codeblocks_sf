#include "wxsstatictext.h"

#include <wx/stattext.h>
#include <messagemanager.h>


WXS_ST_BEGIN(wxsStaticTextStyles)
    WXS_ST_CATEGORY("wxStaticText")
    WXS_ST(wxALIGN_LEFT)
    WXS_ST(wxALIGN_RIGHT)
    WXS_ST(wxALIGN_CENTRE)
    WXS_ST(wxST_NO_AUTORESIZE)
WXS_ST_END()

WXS_EV_BEGIN(wxsStaticTextEvents)
    WXS_EV_DEFAULTS()
WXS_EV_END()


wxsItemInfo wxsStaticText::Info =
{
    _T("wxStaticText"),
    wxsTWidget,
    _("wxWidgets license"),
    _("wxWidgets team"),
    _T(""),
    _T("www.wxwidgets.org"),
    _T("Standard"),
    80,
    _T("StaticText"),
    2, 6,
    NULL,
    NULL,
    0
};


wxsStaticText::wxsStaticText(wxsWindowRes* Resource):
    wxsWidget(
        Resource,
        wxsBaseProperties::flAll,
        &Info,
        wxsStaticTextEvents,
        wxsStaticTextStyles,
        _T("")),
        Label(_("Label"))

{}


void wxsStaticText::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code<< GetVarName() << _T(" = new wxStaticText(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << wxsGetWxString(Label) << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");

            SetupWindowCode(Code,Language);
            return;
        }
    }

    wxsLANGMSG(wxsStaticText::BuildCreatingCode,Language);
}


wxObject* wxsStaticText::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    wxStaticText* Preview = new wxStaticText(Parent,GetId(),Label,Pos(Parent),Size(Parent),Style());

    return SetupWindow(Preview,Exact);
}


void wxsStaticText::EnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsStaticText,Label,0,_("Label"),_T("label"),_T(""),true,false)
//    WXS_BOOL  (wxsStaticText,IsDefault,0,_("Is default"),_("default"),false)
}

void wxsStaticText::EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/stattext.h>")); return;
    }

    wxsLANGMSG(wxsStaticText::EnumDeclFiles,Language);
}
