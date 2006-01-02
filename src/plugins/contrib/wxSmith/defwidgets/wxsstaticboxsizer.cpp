#include "../wxsheaders.h"
#include "wxsstaticboxsizer.h"

#include "../properties/wxsenumproperty.h"
#include "../wxsglobals.h"

wxString wxsStaticBoxSizer::GetProducingCode(const wxsCodeParams& Params)
{
    return wxString::Format(_T("%s = new wxStaticBoxSizer(%s,%s,%s);"),
        Params.VarName.c_str(),
        (Orient == wxVERTICAL) ? _T("wxVERTICAL") : _T("wxHORIZONTAL"),
        Params.ParentName.c_str(),
        wxsGetWxString(Label).c_str() );
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


void wxsStaticBoxSizer::MyCreateProperties()
{
	static const wxChar* Names[] = { _("Horizontal"), _("Vertical"), NULL };
	static long Values[] = { wxHORIZONTAL, wxVERTICAL };
    Properties.AddProperty( _("Label:"), Label);
    Properties.AddProperty( _("Orientation:"), new wxsEnumProperty(Orient,Names,Values));
    wxsWidget::MyCreateProperties();
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
