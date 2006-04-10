#include "wxsgenericdirctrl.h"

#include <wx/dirctrl.h>
#include <messagemanager.h>


WXS_ST_BEGIN(wxsGenericDirCtrlStyles)
    WXS_ST_CATEGORY("wxGenericDirCtrl")
    WXS_ST(wxDIRCTRL_DIR_ONLY)
    WXS_ST(wxDIRCTRL_3D_INTERNAL)
    WXS_ST(wxDIRCTRL_SELECT_FIRST)
    WXS_ST(wxDIRCTRL_SHOW_FILTERS)
    WXS_ST_MASK(wxDIRCTRL_EDIT_LABELS,wxsSFAll,0,false)
WXS_ST_END()


WXS_EV_BEGIN(wxsGenericDirCtrlEvents)
    WXS_EV_DEFAULTS()
WXS_EV_END()


wxsItemInfo wxsGenericDirCtrl::Info =
{
    _T("wxGenericDirCtrl"),
    wxsTWidget,
    _("wxWidgets license"),
    _("wxWidgets team"),
    _T(""),
    _T("www.wxwidgets.org"),
    _T("Standard"),
    30,
    _T("GenDirCtrl"),
    2, 6,
    NULL,
    NULL,
    0
};


wxsGenericDirCtrl::wxsGenericDirCtrl(wxsWindowRes* Resource):
    wxsWidget(
        Resource,
        wxsBaseProperties::flAll,
        &Info,
        wxsGenericDirCtrlEvents,
        wxsGenericDirCtrlStyles,
        _T("")),
        DefaultFilter(0)

{}



void wxsGenericDirCtrl::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code<< GetVarName() << _T(" = new wxGenericDirCtrl(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << wxsGetWxString(DefaultFolder) << _T(",")
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(",")
                << wxsGetWxString(Filter) << _T(",")
                << wxString::Format(_T("%d"),DefaultFilter) << _T(");\n");

            SetupWindowCode(Code,Language);
            return;
        }
    }

    wxsLANGMSG(wxsGenericDirCtrl::BuildCreatingCode,Language);
}

wxObject* wxsGenericDirCtrl::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    wxGenericDirCtrl* Preview = new wxGenericDirCtrl(Parent,GetId(),DefaultFolder,Pos(Parent),Size(Parent),Style(),Filter,DefaultFilter);
    return SetupWindow(Preview,Exact);
}



void wxsGenericDirCtrl::EnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsGenericDirCtrl,DefaultFolder,0,_("Default Folder"),_T("defaultfolder"),_T(""),true,false)
    WXS_STRING(wxsGenericDirCtrl,Filter,0,_("Filter"),_T("filter"),_T(""),true,false)
    WXS_LONG(wxsGenericDirCtrl,DefaultFilter,0,_("Default Filter"),_T("defaultfilter"),0)
}

void wxsGenericDirCtrl::EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/dirctrl.h>")); return;
    }

    wxsLANGMSG(wxsGenericDirCtrl::EnumDeclFiles,Language);
}
