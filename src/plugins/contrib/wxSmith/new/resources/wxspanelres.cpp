#include "wxspanelres.h"
#include "wxsdialogpreviewdlg.h"
#include "../wxscustomwidgetxmlhandler.h"
#include "../wxsglobals.h"
#include "../wxsitemfactory.h"
#include "../wxsproject.h"
#include "../wxsitem.h"

wxsItem* wxsPanelRes::BuildRootItem()
{
    return wxsGEN(_T("wxPanel resource"),this);
}

wxString wxsPanelRes::GetRootItemClass()
{
    return _T("wxPanel");
}

wxWindow* wxsPanelRes::BuildPreview()
{
    wxDialog* Dlg = new wxsDialogPreviewDlg(this);
    Dlg->Create(NULL,-1,
        wxString::Format(_("Frame preview: %s"),GetClassName().c_str()),
        wxDefaultPosition,wxDefaultSize,
        wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

    if ( UsingXRC() )
    {
        SaveResource();
        wxXmlResource Res(
            GetProject() ?
            GetProject()->GetProjectFileName(GetXrcFile()):
            GetXrcFile() );
        Res.InitAllHandlers();
        Res.AddHandler(new wxsCustomWidgetXmlHandler());
       	wxPanel* Panel = Res.LoadPanel(Dlg,GetClassName());
       	if ( !Panel )
       	{
       	    delete Dlg;
       	    return NULL;
       	}
       	Dlg->Fit();
    }
    else
    {
        GetRootItem()->BuildPreview(Dlg,true);
    }

    return Dlg;
}

wxString wxsPanelRes::BuildXrcLoadingCode()
{
    return _T("wxXmlResource::Get()->LoadPanel(this,parent,") +
        wxsGetWxString(GetClassName()) + _T(");\n");
}
