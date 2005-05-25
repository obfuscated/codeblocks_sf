#include "wxsdialogres.h"

#include "../wxswidgetfactory.h"
#include "../wxswindoweditor.h"
#include <manager.h>
#include <wx/xrc/xmlres.h>


wxsDialogRes::wxsDialogRes(wxsProject* Project,const wxString& Class, const wxString& Xrc, const wxString& Src,const wxString& Head):
    wxsResource(Project),
    ClassName(Class),
    XrcFile(Xrc),
    SrcFile(Src),
    HFile(Head)
{
    Dialog = dynamic_cast<wxsDialog*>(wxsWidgetFactory::Get()->Generate("wxDialog"));
}

wxsDialogRes::~wxsDialogRes()
{
    EditClose();
    wxsWidgetFactory::Get()->Kill(Dialog);
    GetProject()->DeleteDialog(this);
}

wxsEditor* wxsDialogRes::CreateEditor()
{
    wxsWindowEditor* Edit = new wxsWindowEditor(Manager::Get()->GetAppWindow(),XrcFile,this);
    Edit->BuildPreview(Dialog);
    return Edit;
}

void wxsDialogRes::Save()
{
    TiXmlDocument* Doc = GenerateXml();
    
    if ( Doc )
    {
        wxString FullFileName = GetProject()->GetInternalFileName(XrcFile);
        DebLog("Saving result to %s",FullFileName.c_str());
        Doc->SaveFile(FullFileName);
        delete Doc;
    }
}

TiXmlDocument* wxsDialogRes::GenerateXml()
{
    TiXmlDocument* NewDoc = new TiXmlDocument;
    TiXmlElement* Resource = NewDoc->InsertEndChild(TiXmlElement("resource"))->ToElement();
    TiXmlElement* XmlDialog = Resource->InsertEndChild(TiXmlElement("object"))->ToElement();
    XmlDialog->SetAttribute("class","wxDialog");
    XmlDialog->SetAttribute("name",ClassName.c_str());
    if ( !Dialog->XmlSave(XmlDialog) )
    {
        delete NewDoc;
        return NULL;
    }
    return NewDoc;
}

void wxsDialogRes::ShowPreview()
{
    Save();
    
    wxXmlResource Res(GetProject()->GetInternalFileName(XrcFile));
    Res.InitAllHandlers();
    
    wxDialog* Dlg = Res.LoadDialog(NULL,ClassName);
    if ( Dlg )
    {
        Dlg->ShowModal();
        delete Dlg;
    }
}
