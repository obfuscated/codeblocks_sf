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

wxsPanel::wxsPanel(wxsWidgetManager* Man,wxsWindowRes* Res):
    wxsWindow(Man,Res,propWidget)
{
}

wxsPanel::~wxsPanel()
{
}

const wxsWidgetInfo& wxsPanel::GetInfo()
{
    return *wxsStdManager.GetWidgetInfo(wxsPanelId);
}


wxString wxsPanel::GetProducingCode(wxsCodeParams& Params)
{
    return wxString::Format(_T("%s = new wxPanel(%s,%s,%s,%s,%s);"),
        BaseParams.VarName.c_str(),
        Params.ParentName.c_str(),
        BaseParams.IdName.c_str(),
        GetCodeDefines().Pos.c_str(),
        GetCodeDefines().Size.c_str(),
        GetCodeDefines().Style.c_str());
}

wxString wxsPanel::GetDeclarationCode(wxsCodeParams& Params)
{
	return wxString::Format(_T("wxPanel* %s;"),BaseParams.VarName.c_str());
}


WXS_ST_BEGIN(wxsPanelrStyles)
    WXS_ST(wxNO_3D)
    WXS_ST(wxTAB_TRAVERSAL)
    WXS_ST(wxWS_EX_VALIDATE_RECURSIVELY)
WXS_ST_END(wxsPanelrStyles)

wxsPanelr::wxsPanelr(wxsWidgetManager* Man,wxsWindowRes* Res):
    wxsWindow(Man,Res,propWindow)
{
}

wxsPanelr::~wxsPanelr()
{
}

const wxsWidgetInfo& wxsPanelr::GetInfo()
{
    return *wxsStdManager.GetWidgetInfo(wxsPanelrId);
}
