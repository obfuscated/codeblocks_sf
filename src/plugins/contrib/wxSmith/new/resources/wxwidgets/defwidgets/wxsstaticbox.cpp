#include "wxsstaticbox.h"

#include <wx/statbox.h>
#include <messagemanager.h>


WXS_ST_BEGIN(wxsStaticBoxStyles)
    WXS_ST_CATEGORY("wxStaticBox")
    WXS_ST(wxBU_LEFT)
    WXS_ST(wxBU_TOP)
    WXS_ST(wxBU_RIGHT)
    WXS_ST(wxBU_BOTTOM)
    WXS_ST(wxBU_EXACTFIT)
    WXS_ST_MASK(wxNO_BORDER,wxsSFWin| wxsSFGTK ,0,true)
WXS_ST_END()



WXS_EV_BEGIN(wxsStaticBoxEvents)
    WXS_EVI(EVT_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEvent,Click)
    WXS_EV_DEFAULTS()
WXS_EV_END()


wxsItemInfo wxsStaticBox::Info =
{
    _T("wxStaticBox"),
    wxsTWidget,
    _("wxWidgets license"),
    _("wxWidgets team"),
    _T(""),
    _T("www.wxwidgets.org"),
    _T("Standard"),
    50,
    _T("Button"),
    2, 6,
    NULL,
    NULL,
    0
};



wxsStaticBox::wxsStaticBox(wxsWindowRes* Resource):
    wxsWidget(
        Resource,
        wxsBaseProperties::flAll,
        &Info,
        wxsStaticBoxEvents,
        wxsStaticBoxStyles,
        _T("")),
    Label(_("Label"))
{}



void wxsStaticBox::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code<< GetVarName() << _T(" = new wxStaticBox(")
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

    wxsLANGMSG(wxsStaticBox::BuildCreatingCode,Language);
}


wxObject* wxsStaticBox::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    wxStaticBox* Preview = new wxStaticBox(Parent,GetId(),Label,Pos(Parent),Size(Parent),Style());
    return SetupWindow(Preview,Exact);
}


void wxsStaticBox::EnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsStaticBox,Label,0,_("Label"),_T("label"),_T(""),true,false)
}

void wxsStaticBox::EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/statbox.h>")); return;
    }

    wxsLANGMSG(wxsStaticBox::EnumDeclFiles,Language);
}
