#include "../wxsheaders.h"
#include "wxspanel.h"

#include <wx/frame.h>
#include <wx/sizer.h>

#include "wxsstdmanager.h"
#include "../wxspropertiesman.h"

WXS_ST_BEGIN(wxsPanelStyles)
    WXS_ST(wxNO_3D)
    WXS_ST(wxTAB_TRAVERSAL)
    WXS_ST(wxWS_EX_VALIDATE_RECURSIVELY)
WXS_ST_END(wxsPanelStyles)

WXS_EV_BEGIN(wxsPanelEvents)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsPanelEvents)

wxsPanel::wxsPanel(wxsWidgetManager* Man,wxsWindowRes* Res):
    wxsWindow(Man,Res,propNone)
{
    ChangeBPT(wxsREMSource,propWidgetS);
    ChangeBPT(wxsREMFile,propWidgetF);
    ChangeBPT(wxsREMMixed,propWidgetM);
}

wxsPanel::~wxsPanel()
{
}

const wxsWidgetInfo& wxsPanel::GetInfo()
{
    return *wxsStdManager.GetWidgetInfo(wxsPanelId);
}


wxString wxsPanel::GetProducingCode(const wxsCodeParams& Params)
{
    return wxString::Format(_T("%s = new wxPanel(%s,%s,%s,%s,%s,%s);\n%s"),
        Params.VarName.c_str(),
        Params.ParentName.c_str(),
        Params.IdName.c_str(),
        Params.Pos.c_str(),
        Params.Size.c_str(),
        Params.Style.c_str(),
        Params.Name.c_str(),
        Params.InitCode.c_str()
        );
}

WXS_ST_BEGIN(wxsPanelrStyles)
    WXS_ST(wxNO_3D)
    WXS_ST(wxTAB_TRAVERSAL)
    WXS_ST(wxWS_EX_VALIDATE_RECURSIVELY)
WXS_ST_END(wxsPanelrStyles)

WXS_EV_BEGIN(wxsPanelrEvents)
    WXS_EV(EVT_INIT_DIALOG,wxInitDialogEvent,Init)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsPanelrEvents)

wxsPanelr::wxsPanelr(wxsWidgetManager* Man,wxsWindowRes* Res):
    wxsWindow(Man,Res,propWindow)
{
    SetStyle(wxTAB_TRAVERSAL);
}

wxsPanelr::~wxsPanelr()
{
}

const wxsWidgetInfo& wxsPanelr::GetInfo()
{
    return *wxsStdManager.GetWidgetInfo(wxsPanelrId);
}

wxString wxsPanelr::GetProducingCode(const wxsCodeParams& Params)
{
    return wxString::Format(_T("Create(parent,id,%s,%s,%s,%s);%s"),
        Params.Pos.c_str(),Params.Size.c_str(),
        Params.Style.c_str(),Params.Name.c_str(),
        Params.InitCode.c_str());
}
