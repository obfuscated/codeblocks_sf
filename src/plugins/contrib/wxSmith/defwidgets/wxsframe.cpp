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

    WXS_ST(wxNO_3D)
    WXS_ST(wxTAB_TRAVERSAL)
    WXS_ST(wxWS_EX_VALIDATE_RECURSIVELY)
// NOTE (cyberkoa##): wxFRAME_EX_METAL is an extended style and must be set by calling SetExtraStyle before Create is called (two-step construction).
    WXS_ST(wxFRAME_EX_METAL)
// NOTE (cyberkoa##): cannot use wxFRAME_EX_CONTEXTHELP together with wxMAXIMIZE_BOX or wxMINIMIZE_BOX
    WXS_ST(wxFRAME_EX_CONTEXTHELP)

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
}

wxsFrame::~wxsFrame()
{
}

const wxsWidgetInfo& wxsFrame::GetInfo()
{
    return *wxsStdManager.GetWidgetInfo(wxsFrameId);
}

void wxsFrame::CreateObjectProperties()
{
    wxsWidget::CreateObjectProperties();
	PropertiesObject.AddProperty(_("Title:"),Title,0);
	PropertiesObject.AddProperty(_("Centered:"),Centered,1);
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

wxString wxsFrame::GetProducingCode(wxsCodeParams& Params)
{
    CodeDefines CDefs = GetCodeDefines();
    return wxString::Format(_T("Create(parent,id,%s,%s,%s,%s);%s"),
        GetWxString(Title).c_str(),CDefs.Pos.c_str(),
        CDefs.Size.c_str(),CDefs.Style.c_str(),
        CDefs.InitCode.c_str());
}

wxString wxsFrame::GetFinalizingCode(wxsCodeParams& Params)
{
    return Centered ? _T("Center();\n") : _T("");
}
