#include "wxsimportxrcdlg.h"

wxsImportXrcDlg::wxsImportXrcDlg(wxWindow* Parent, TiXmlElement* Elem):
    wxsNewWindowDlg(Parent,wxString(Elem->Attribute("class")+2,wxConvUTF8)),
    Element(Elem)
{
	wxString ResourceName = wxString(Elem->Attribute("name"),wxConvUTF8);
	Class->SetValue(ResourceName);
	wxString ResType = wxString(Elem->Attribute("class")+2,wxConvUTF8);
	SetTitle(wxString::Format(_("Importing %s resource"),ResType.c_str()));
}

wxsImportXrcDlg::~wxsImportXrcDlg()
{
}

bool wxsImportXrcDlg::PrepareResource(wxsWindowRes* Res)
{
	wxsWidget* Widget = Res->GetRootWidget();
	Widget->XmlLoad(Element);
	Res->CheckBaseProperties(true);
    return true;
}
