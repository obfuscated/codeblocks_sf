#include "wxsdialogres.h"
#include "wxsdialogpreviewdlg.h"
#include "../wxscustomwidgetxmlhandler.h"
#include "../wxsglobals.h"
#include "../wxsitemfactory.h"
#include "../wxsproject.h"
#include "../wxsitem.h"

wxsItem* wxsDialogRes::BuildRootItem()
{
    return wxsGEN(_T("wxDialog"),this);
}

wxString wxsDialogRes::GetRootItemClass()
{
    return _T("wxDialog");
}

wxWindow* wxsDialogRes::BuildPreview()
{
    wxDialog* Dlg = new wxsDialogPreviewDlg(this);

    if ( UsingXRC() )
    {
        SaveResource();
        wxXmlResource Res(
            GetProject() ?
            GetProject()->GetProjectFileName(GetXrcFile()):
            GetXrcFile());
        Res.InitAllHandlers();
        Res.AddHandler(new wxsCustomWidgetXmlHandler());
        if ( !Res.LoadDialog(Dlg,NULL,GetClassName()) )
        {
            delete Dlg;
            return NULL;
        }
    }
    else
    {
        GetRootItem()->BuildPreview(Dlg,true);
    }

    return Dlg;
}
