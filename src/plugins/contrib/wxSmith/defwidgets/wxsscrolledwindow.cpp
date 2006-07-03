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
    int Bits = BaseProperties.StyleBits;
    SetStyle(wxVSCROLL);
    BaseProperties.StyleBits |= Bits;
    // TODO (SpOoN#1#): Change this workaround to something normal
}

wxsScrolledWindow::~wxsScrolledWindow()
{
}

const wxsWidgetInfo& wxsScrolledWindow::GetInfo()
{
    return *wxsStdManager.GetWidgetInfo(wxsScrolledWindowId);
}


wxString wxsScrolledWindow::GetProducingCode(const wxsCodeParams& Params)
{
    return wxString::Format(_T("%s = new wxScrolledWindow(%s,%s,%s,%s,%s,%s);\n%s"),
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
