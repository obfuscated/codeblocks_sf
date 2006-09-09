#include "wxsimportxrcdlg.h"
#include "wxsitem.h"
#include "editors/wxscorrector.h"
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

	// Loading everything from xrc file
	Item->XmlRead(Element,true,false);

	// Correcting data right afrer importing it
	wxsCorrector Corrector(Res);
	Corrector.GlobalCheck();

	// Saving resource data
	Res->SetModified(true);
	Res->SaveResource();
    return true;
}
