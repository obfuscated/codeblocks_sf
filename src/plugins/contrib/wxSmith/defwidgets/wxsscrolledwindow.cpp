#include "../wxsheaders.h"
#include "wxsscrolledwindow.h"
#include "wxsstdmanager.h"


WXS_ST_BEGIN(wxsScrolledWindowStyles)
    WXS_ST(wxHSCROLL)
    WXS_ST(wxVSCROLL)
    WXS_ST(wxNO_3D)
    WXS_ST(wxTAB_TRAVERSAL)
    WXS_ST(wxWS_EX_VALIDATE_RECURSIVELY)
WXS_ST_END(wxsScrolledWindowStyles)

WXS_EV_BEGIN(wxsScrolledWindowEvents)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsScrolledWindowEvents)

wxsScrolledWindow::wxsScrolledWindow(wxsWidgetManager* Man,wxsWindowRes* Res):
    wxsWindow(Man,Res,propNone)
{
    ChangeBPT(wxsREMSource,propWidgetS);
    ChangeBPT(wxsREMFile,propWidgetF);
    ChangeBPT(wxsREMMixed,propWidgetM);
    SetStyle(wxHSCROLL);
    int Bits = GetBaseProperties().StyleBits;
    SetStyle(wxVSCROLL);
    GetBaseProperties().StyleBits |= Bits;
    // TODO (SpOoN#1#): Change this workaround to something normal
}

wxsScrolledWindow::~wxsScrolledWindow()
{
}

const wxsWidgetInfo& wxsScrolledWindow::GetInfo()
{
    return *wxsStdManager.GetWidgetInfo(wxsScrolledWindowId);
}


wxString wxsScrolledWindow::GetProducingCode(wxsCodeParams& Params)
{
	const CodeDefines& CDefs = GetCodeDefines();
    return wxString::Format(_T("%s = new wxScrolledWindow(%s,%s,%s,%s,%s);\n%s"),
        GetBaseProperties().VarName.c_str(),
        Params.ParentName.c_str(),
        GetBaseProperties().IdName.c_str(),
        CDefs.Pos.c_str(),
        CDefs.Size.c_str(),
        CDefs.Style.c_str(),
        CDefs.InitCode.c_str()
        );
}

wxString wxsScrolledWindow::GetDeclarationCode(wxsCodeParams& Params)
{
	return wxString::Format(_T("wxScrolledWindow* %s;"),GetBaseProperties().VarName.c_str());
}

