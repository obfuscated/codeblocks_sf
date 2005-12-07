#include "wxsheaders.h"
#include "wxsimportxrcdlg.h"

wxsImportXrcDlg::wxsImportXrcDlg(wxWindow* Parent, TiXmlElement* Elem):
    wxsNewWindowDlg(Parent,_U(Elem->Attribute("class")+2)),
    Element(Elem)
{
	wxString ResourceName = _U(Elem->Attribute("name"));
	Class->SetValue(ResourceName);
	wxString ResType = _U(Elem->Attribute("class")+2);
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
