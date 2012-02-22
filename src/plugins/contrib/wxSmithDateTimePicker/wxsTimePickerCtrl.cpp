#include "wxsTimePickerCtrl.h"
#include "TimeCtrl.h"

namespace
{

    #include "images/wxTimePickCtrl16.xpm"
    #include "images/wxTimePickCtrl32.xpm"

    wxsRegisterItem<wxsTimePickerCtrl> Reg(
        _T("wxTimePickerCtrl"),
        wxsTWidget,
        _T("wxWindows"),
        _T(""),
        _T(""),
        _T(""),
        _T("Advanced"),
        80,
        _T("TimePickerCtrl"),
        wxsCPP,
        1, 0,
        wxBitmap(wxTimePickCtrl32_xpm),
        wxBitmap(wxTimePickCtrl16_xpm),
        false);

    WXS_ST_BEGIN(wxsTimePickerCtrlStyles,_T(""))
        WXS_ST_DEFAULTS()
    WXS_ST_END()


    WXS_EV_BEGIN(wxsTimePickerCtrlEvents)
        WXS_EVI(EVT_DATE_CHANGED,wxEVT_DATE_CHANGED,wxDateEvent,Changed)
    WXS_EV_END()
}

wxsTimePickerCtrl::wxsTimePickerCtrl(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsTimePickerCtrlEvents,
        wxsTimePickerCtrlStyles)
{
    //ctor
}

wxsTimePickerCtrl::~wxsTimePickerCtrl()
{
    //dtor
}

void wxsTimePickerCtrl::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/timectrl.h>"),GetInfo().ClassName,0);
            AddHeader(_T("<wx/dateevt.h>"),_T("wxDateEvent"),0);
            Codef(_T("%C(%W, %I, wxDateTime::Now(), %P, %S, %T, %V, %N);\n"));
            BuildSetupWindowCode();
            return;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsTimePickerCtrl::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

/*! \brief	Build the control preview.
 *
 * \param parent wxWindow*	The parent window.
 * \param flags long				The control flags.
 * \return wxObject* 				The constructed control.
 *
 */
wxObject* wxsTimePickerCtrl::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxTimePickerCtrl* Preview = new wxTimePickerCtrl(Parent,GetId(),wxDateTime::Now(),Pos(Parent),Size(Parent),Style());
    return SetupWindow(Preview,Flags);
}

/*! \brief Enumerate the control's properties.
 *
 * \param flags long	The control flags.
 * \return void
 *
 */
void wxsTimePickerCtrl::OnEnumWidgetProperties(long Flags)
{
}
