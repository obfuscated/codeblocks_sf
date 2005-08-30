#include "wxsstaticboxsizer.h"

#include "../properties/wxsenumproperty.h"
#include "../wxsglobals.h"

wxString wxsStaticBoxSizer::GetProducingCode(wxsCodeParams& Params)
{
    return wxString::Format(_T("%s = new wxStaticBoxSizer(%s,%s,%s);"),
        BaseParams.VarName.c_str(),
        (Orient == wxVERTICAL) ? _T("wxVERTICAL") : _T("wxHORIZONTAL"),
        Params.ParentName.c_str(),
        GetWxString(Label).c_str() );
}

bool wxsStaticBoxSizer::MyXmlLoad()
{
	wxString Value = XmlGetVariable(_T("orient"));
	Orient = ( Value == _T("wxVERTICAL") ) ? wxVERTICAL : wxHORIZONTAL;
	Label = XmlGetVariable(_T("label"));
    return true;
}

bool wxsStaticBoxSizer::MyXmlSave()
{
    XmlSetVariable(_T("orient"),(Orient == wxVERTICAL) ? _T("wxVERTICAL") : _T("wxHORIZONTAL"));
    XmlSetVariable(_T("label"),Label);
    return true;
}


void wxsStaticBoxSizer::CreateObjectProperties()
{
    PropertiesObject.AddProperty( _("Label:"), Label, 0 );
	static const wxChar* Names[] = { _("Horizontal"), _("Vertical"), NULL };
	static long Values[] = { wxHORIZONTAL, wxVERTICAL };
    wxsWidget::CreateObjectProperties();
    PropertiesObject.AddProperty( _("Orientation:"), new wxsEnumProperty(&PropertiesObject,Orient,Names,Values), 1);
}

void wxsStaticBoxSizer::Init()
{
	Orient = wxHORIZONTAL;
	Label = _T("Label");
}

wxSizer* wxsStaticBoxSizer::ProduceSizer(wxWindow* Panel)
{
	return new wxStaticBoxSizer(Orient,Panel,Label);
}
