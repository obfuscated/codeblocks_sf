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

    WXS_ST(wxNO_3D)
    WXS_ST(wxTAB_TRAVERSAL)
    WXS_ST(wxWS_EX_VALIDATE_RECURSIVELY)
    WXS_ST(wxDIALOG_EX_METAL)
    WXS_ST(wxMAXIMIZE_BOX)
    WXS_ST(wxMINIMIZE_BOX)
    WXS_ST(wxFRAME_SHAPED)

WXS_ST_END(wxsDialogStyles)

wxsDialog::wxsDialog(wxsWidgetManager* Man):
    wxsWindow(Man,propWindow),
    Centered(false)
{
}

wxsDialog::~wxsDialog()
{
}

const wxsWidgetInfo& wxsDialog::GetInfo()
{
    return *wxsStdManager.GetWidgetInfo(wxsDialogId);
}

void wxsDialog::CreateObjectProperties()
{
    wxsWidget::CreateObjectProperties();
	PropertiesObject.AddProperty("Title:",Title,0,false,false);
	PropertiesObject.AddProperty("Centered:",Centered,1,false,false);
}

