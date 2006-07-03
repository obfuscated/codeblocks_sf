#include "../wxsheaders.h"
#include "wxsdialog.h"

#include <wx/frame.h>
#include <wx/sizer.h>

#include "wxsstdmanager.h"
#include "../wxspropertiesman.h"

WXS_ST_BEGIN(wxsDialogStyles)

    WXS_ST(wxSTAY_ON_TOP)
    WXS_ST(wxCAPTION)
    WXS_ST(wxDEFAULT_DIALOG_STYLE)
    WXS_ST(wxTHICK_FRAME)
    WXS_ST(wxSYSTEM_MENU)
    WXS_ST(wxRESIZE_BORDER)
    WXS_ST(wxRESIZE_BOX)
    WXS_ST(wxCLOSE_BOX)
    WXS_ST(wxDIALOG_MODAL)
    WXS_ST(wxDIALOG_MODELESS)
    WXS_ST(wxDIALOG_NO_PARENT)
    WXS_ST(wxTAB_TRAVERSAL)
    WXS_ST(wxWS_EX_VALIDATE_RECURSIVELY)
    WXS_ST(wxDIALOG_EX_METAL)
    WXS_ST(wxMAXIMIZE_BOX)
    WXS_ST(wxMINIMIZE_BOX)
    WXS_ST(wxFRAME_SHAPED)
// NOTE (cyberkoa#1#): wxNO_3D & wxDIALOG_EX_CONTEXTHELP is only available
// on WXMSW wxDIALOG_EX_METAL only for WXMAC
// NOTE (cyberkoa#1#):  wxDIALOG_EX_CONTEXTHELP & wxDIALOG_EX_METAL are
// extended style which need 2 steps construction
    WXS_ST_MASK(wxNO_3D,wxsSFWin,0,true)
    WXS_EXST_MASK(wxDIALOG_EX_CONTEXTHELP,wxsSFWin,0,true)
    WXS_EXST_MASK(wxDIALOG_EX_METAL,wxsSFOSX,0,true)

WXS_ST_END(wxsDialogStyles)

WXS_EV_BEGIN(wxsDialogEvents)
    WXS_EV(EVT_INIT_DIALOG,wxInitDialogEvent,Init)
    WXS_EV_DEFAULTS()
WXS_EV_END(wxsDialogEvents)

wxsDialog::wxsDialog(wxsWidgetManager* Man,wxsWindowRes* Res):
    wxsWindow(Man,Res,propWindow),
    Centered(false)
{
    SetStyle(wxDEFAULT_DIALOG_STYLE);
}

wxsDialog::~wxsDialog()
{
}

const wxsWidgetInfo& wxsDialog::GetInfo()
{
    return *wxsStdManager.GetWidgetInfo(wxsDialogId);
}

void wxsDialog::MyCreateProperties()
{
	Properties.AddProperty(_("Title:"),Title);
	Properties.AddProperty(_("Centered:"),Centered);
    wxsWidget::MyCreateProperties();
}

bool wxsDialog::MyXmlLoad()
{
	Title = XmlGetVariable(_T("title"));
	Centered = XmlGetInteger(_T("centered"),0) != 0;
	return true;
}

bool wxsDialog::MyXmlSave()
{
	if ( !Title.empty() ) XmlSetVariable(_T("title"),Title);
	if ( Centered ) XmlSetInteger(_T("centered"),1);
	return true;
}

wxString wxsDialog::GetProducingCode(const wxsCodeParams& Params)
{
    return wxString::Format(_T("Create(parent,id,%s,%s,%s,%s,%s);%s"),
        wxsGetWxString(Title).c_str(),Params.Pos.c_str(),
        Params.Size.c_str(),Params.Style.c_str(),Params.Name.c_str(),
        Params.InitCode.c_str());
}

wxString wxsDialog::GetFinalizingCode(const wxsCodeParams& Params)
{
    return Centered ? _T("Center();\n") : _T("");
}
