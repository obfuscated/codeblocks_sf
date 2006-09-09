#include "wxsdatepickerctrl.h"

#include <wx/datectrl.h>
#include <messagemanager.h>


WXS_ST_BEGIN(wxsDatePickerCtrlStyles)
    WXS_ST_CATEGORY("wxDatePickerCtrl")
    WXS_ST(wxDP_DEFAULT)
    WXS_ST(wxDP_SPIN)
    WXS_ST(wxDP_DROPDOWN)
    WXS_ST(wxDP_ALLOWNONE)
    WXS_ST(wxDP_SHOWCENTURY)
WXS_ST_END()


WXS_EV_BEGIN(wxsDatePickerCtrlEvents)
    WXS_EVI(EVT_DATE_CHANGED,wxEVT_DATE_CHANGED,wxDataEvent,Changed)
    WXS_EV_DEFAULTS()
WXS_EV_END()


wxsItemInfo wxsDatePickerCtrl::Info =
{
    _T("wxDatePickerCtrl"),
    wxsTWidget,
    _("wxWidgets license"),
    _("wxWidgets team"),
    _T(""),
    _T("www.wxwidgets.org"),
    _T("Standard"),
    30,
    _T("DatePickerCtrl"),
    2, 6,
    NULL,
    NULL,
    0
};



wxsDatePickerCtrl::wxsDatePickerCtrl(wxsWindowRes* Resource):
    wxsWidget(
        Resource,
        wxsBaseProperties::flAll,
        &Info,
        wxsDatePickerCtrlEvents,
        wxsDatePickerCtrlStyles,
        _T(""))

{}



void wxsDatePickerCtrl::BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
        {
            Code<< GetVarName() << _T(" = new wxDatePickerCtrl(")
                << WindowParent << _T(",")
                << GetIdName() << _T(",")
                << _T("wxDefaultDateTime") << _T(",")   // TODO find a way to get the wxDateTime in code
                << PosCode(WindowParent,wxsCPP) << _T(",")
                << SizeCode(WindowParent,wxsCPP) << _T(",")
                << StyleCode(wxsCPP) << _T(");\n");

            SetupWindowCode(Code,Language);
            return;
        }
    }

    wxsLANGMSG(wxsDatePickerCtrl::BuildCreatingCode,Language);
}


wxObject* wxsDatePickerCtrl::DoBuildPreview(wxWindow* Parent,bool Exact)
{
    wxDatePickerCtrl* Preview = new wxDatePickerCtrl(Parent,GetId(),wxDefaultDateTime,Pos(Parent),Size(Parent),Style());

    return SetupWindow(Preview,Exact);
}


void wxsDatePickerCtrl::EnumWidgetProperties(long Flags)
{
 //  TODO : find
 //   WXS_DATETIME(wxsDatePickerCtrl,DefaultDateTime,0,_("Default"),_T("default"),_T(""),true,false)

}

void wxsDatePickerCtrl::EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/datectrl.h>")); return;
    }

    wxsLANGMSG(wxsDatePickerCtrl::EnumDeclFiles,Language);
}

