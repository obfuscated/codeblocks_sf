#include "../wxsheaders.h"
#include "wxsstddialogbuttonsizer.h"

#include "../properties/wxsenumproperty.h"

wxString wxsStdDialogButtonSizer::GetProducingCode(const wxsCodeParams& Params)
{
    return wxString::Format(_T("%s = new wxStdDialogButtonSizer();"),
        BaseProperties.VarName.c_str() );
}

bool wxsStdDialogButtonSizer::MyXmlLoad()
{
    // TODO (TakeshiMiya##): XRC load doesn't works, because it expects a
    // "button" instead of a "sizeritem" probably.
    return true;
}

bool wxsStdDialogButtonSizer::MyXmlSave()
{
    // TODO (TakeshiMiya##): XRC save doesn't works, because it expects a
    // "button" instead of a "sizeritem" probably.
    return true;
}

void wxsStdDialogButtonSizer::MyCreateProperties()
{
    wxsWidget::MyCreateProperties();
}

void wxsStdDialogButtonSizer::Init()
{
    chkOK = true;
    chkCANCEL = true;
    chkSAVE = false;
    chkAPPLY = false;
    chkNO = false;
    chkHELP = false;
    chkCONTEXT_HELP = false;
}

wxSizer* wxsStdDialogButtonSizer::ProduceSizer(wxWindow* Panel)
{
	return new wxStdDialogButtonSizer();
}
