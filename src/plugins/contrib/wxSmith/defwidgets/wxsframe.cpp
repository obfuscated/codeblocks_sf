#include "../wxsheaders.h"
#include "wxsframe.h"

#include <wx/frame.h>
#include <wx/sizer.h>

#include "wxsstdmanager.h"
#include "../wxspropertiesman.h"

WXS_ST_BEGIN(wxsFrameStyles)

    WXS_ST(wxSTAY_ON_TOP)
    WXS_ST(wxCAPTION)
    WXS_ST(wxDEFAULT_DIALOG_STYLE)
    WXS_ST(wxDEFAULT_FRAME_STYLE)
    WXS_ST(wxTHICK_FRAME)
    WXS_ST(wxSYSTEM_MENU)
    WXS_ST(wxRESIZE_BORDER)
    WXS_ST(wxRESIZE_BOX)
    WXS_ST(wxCLOSE_BOX)

    WXS_ST(wxFRAME_NO_TASKBAR)
    WXS_ST(wxFRAME_SHAPED)
    WXS_ST(wxFRAME_TOOL_WINDOW)
    WXS_ST(wxFRAME_FLOAT_ON_PARENT)
    WXS_ST(wxMAXIMIZE_BOX)
    WXS_ST(wxMINIMIZE_BOX)
    WXS_ST(wxSTAY_ON_TOP)

 //   WXS_ST(wxNO_3D)
    WXS_ST(wxTAB_TRAVERSAL)
    WXS_ST(wxWS_EX_VALIDATE_RECURSIVELY)
    WXS_EXST_MASK(wxDIALOG_EX_CONTEXTHELP,wxsSFWin,0,true)
    WXS_EXST_MASK(wxDIALOG_EX_METAL,wxsSFOSX,0,true)

// NOTE (cyberkoa##): wxMINIMIZE, wxMAXIMIZE are in the HELP file but not in XRC
//#ifdef __WXMSW__
// NOTE (cyberkoa##): There is a style wxICONIZE which is identical to wxMINIMIZE , not included.
//    WXS_ST(wxMINIMIZE)
//    WXS_ST(wxMAXIMIZE)
//#endif

WXS_ST_END(wxsFrameStyles)

WXS_EV_BEGIN(wxsFrameEvents)
    WXS_EV(EVT_INIT_DIALOG,wxInitDialogEvent,Init)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsFrameEvents)


wxsFrame::wxsFrame(wxsWidgetManager* Man,wxsWindowRes* Res):
    wxsWindow(Man,Res,propWindow),
    Centered(false)
{
    SetStyle(wxDEFAULT_FRAME_STYLE);
}

wxsFrame::~wxsFrame()
{
}

const wxsWidgetInfo& wxsFrame::GetInfo()
{
    return *wxsStdManager.GetWidgetInfo(wxsFrameId);
}

void wxsFrame::MyCreateProperties()
{
	Properties.AddProperty(_("Title:"),Title);
	Properties.AddProperty(_("Centered:"),Centered);
    wxsWidget::MyCreateProperties();
}

bool wxsFrame::MyXmlLoad()
{
	Title = XmlGetVariable(_T("title"));
	Centered = XmlGetInteger(_T("centered"),0) != 0;
	return true;
}

bool wxsFrame::MyXmlSave()
{
	if ( !Title.empty() ) XmlSetVariable(_T("title"),Title);
	if ( Centered ) XmlSetInteger(_T("centered"),1);
	return true;
}

wxString wxsFrame::GetProducingCode(const wxsCodeParams& Params)
{
    return wxString::Format(_T("Create(parent,id,%s,%s,%s,%s,%s);%s"),
        wxsGetWxString(Title).c_str(),Params.Pos.c_str(),
        Params.Size.c_str(),Params.Style.c_str(),Params.Name.c_str(),
        Params.InitCode.c_str());
}

wxString wxsFrame::GetFinalizingCode(const wxsCodeParams& Params)
{
    return Centered ? _T("Center();\n") : _T("");
}
