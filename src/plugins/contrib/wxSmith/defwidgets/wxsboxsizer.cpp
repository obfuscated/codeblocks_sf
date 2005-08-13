#include "wxsboxsizer.h"

#include "../properties/wxsenumproperty.h"

wxString wxsBoxSizer::GetProducingCode(wxsCodeParams& Params)
{
    return wxString::Format(_T("%s = new wxBoxSizer(%s);"),
        BaseParams.VarName.c_str(),
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


void wxsBoxSizer::CreateObjectProperties()
{
	static const wxChar* Names[] = { _("Horizontal"), _("Vertical"), NULL };
	static long Values[] = { wxHORIZONTAL, wxVERTICAL };
    wxsWidget::CreateObjectProperties();
    PropertiesObject.AddProperty( _("Orientation:"), new wxsEnumProperty(&PropertiesObject,Orient,Names,Values), 0);
}

void wxsBoxSizer::Init()
{
	Orient = wxHORIZONTAL;
}

wxSizer* wxsBoxSizer::ProduceSizer(wxWindow* Panel)
{
	return new wxBoxSizer(Orient);
}
