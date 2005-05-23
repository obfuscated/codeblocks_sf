#include "wxsdialogres.h"

#include "../wxswidgetfactory.h"
#include "../wxswindoweditor.h"
#include <manager.h>


wxsDialogRes::wxsDialogRes(wxSmith* Plugin,const wxString& Class, const wxString& Xrc, const wxString& Src,const wxString& Head):
    wxsResource(Plugin),
    ClassName(Class),
    XrcFile(Xrc),
    SrcFile(Src),
    HFile(Head)
{
    Dialog = dynamic_cast<wxsDialog*>(wxsWidgetFactory::Get()->Generate("wxDialog"));
}

wxsDialogRes::~wxsDialogRes()
{
    wxsWidgetFactory::Get()->Kill(Dialog);
}

wxsEditor* wxsDialogRes::CreateEditor()
{
    wxsWindowEditor* Edit = new wxsWindowEditor(Manager::Get()->GetAppWindow(),XrcFile,this);
    Edit->BuildPreview(Dialog);
    return Edit;
}
