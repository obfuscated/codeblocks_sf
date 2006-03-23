#include "wxsimportxrcdlg.h"
#include "wxsitem.h"
#include <globals.h>

wxsImportXrcDlg::wxsImportXrcDlg(wxWindow* Parent, TiXmlElement* Elem):
    wxsNewWindowDlg(Parent,cbC2U(Elem->Attribute("class")+2)),
    Element(Elem)
{
	wxString ResourceName = cbC2U(Elem->Attribute("name"));
	Class->SetValue(ResourceName);
	wxString ResType = cbC2U(Elem->Attribute("class")+2);
	SetTitle(wxString::Format(_("Importing %s resource"),ResType.c_str()));
}

wxsImportXrcDlg::~wxsImportXrcDlg()
{
}

bool wxsImportXrcDlg::PrepareResource(wxsWindowRes* Res)
{
    // Make sure root item is created
    Res->LoadResource();
	wxsItem* Item = Res->GetRootItem();
	Item->XmlRead(Element,true,false);
	Res->SetModified(true);
	Res->SaveResource();
	//Res->CheckBaseProperties(true);
    return true;
}
