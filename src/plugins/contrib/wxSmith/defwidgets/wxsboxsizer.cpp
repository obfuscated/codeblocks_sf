#include "../wxsheaders.h"
#include "wxsboxsizer.h"

#include "../properties/wxsenumproperty.h"

wxString wxsBoxSizer::GetProducingCode(const wxsCodeParams& Params)
{
    return wxString::Format(_T("%s = new wxBoxSizer(%s);"),
        BaseProperties.VarName.c_str(),
        (Orient == wxVERTICAL) ? _T("wxVERTICAL") : _T("wxHORIZONTAL") );
}

bool wxsBoxSizer::MyXmlLoad()
{
	wxString Value = XmlGetVariable(_T("orient"));
	Orient = ( Value == _T("wxVERTICAL") ) ? wxVERTICAL : wxHORIZONTAL;
    return true;
}

bool wxsBoxSizer::MyXmlSave()
{
    XmlSetVariable(_T("orient"),(Orient == wxVERTICAL) ? _T("wxVERTICAL") : _T("wxHORIZONTAL"));
    return true;
}


void wxsBoxSizer::MyCreateProperties()
{
	static const wxChar* Names[] = { _("Horizontal"), _("Vertical"), NULL };
	static long Values[] = { wxHORIZONTAL, wxVERTICAL };
    Properties.AddProperty( _("Orientation:"), new wxsEnumProperty(Orient,Names,Values), 0);
    wxsWidget::MyCreateProperties();
}

void wxsBoxSizer::Init()
{
	Orient = wxHORIZONTAL;
}

wxSizer* wxsBoxSizer::ProduceSizer(wxWindow* Panel)
{
	return new wxBoxSizer(Orient);
}
