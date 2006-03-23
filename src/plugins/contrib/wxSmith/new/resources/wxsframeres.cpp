#include "wxsframeres.h"
#include "wxsframepreviewfrm.h"
#include "../wxscustomwidgetxmlhandler.h"
#include "../wxsglobals.h"
#include "../wxsitemfactory.h"
#include "../wxsproject.h"
#include "../wxsitem.h"

wxsItem* wxsFrameRes::BuildRootItem()
{
    return wxsGEN(_T("wxFrame"),this);
}

wxString wxsFrameRes::GetRootItemClass()
{
    return _T("wxFrame");
}

wxWindow* wxsFrameRes::BuildPreview()
{
    wxFrame* Frm = new wxsFramePreviewFrm(this);

    if ( UsingXRC() )
    {
        SaveResource();
        wxXmlResource Res(
            GetProject() ?
            GetProject()->GetProjectFileName(GetXrcFile()):
            GetXrcFile());
        Res.InitAllHandlers();
        Res.AddHandler(new wxsCustomWidgetXmlHandler());
        if ( !Res.LoadFrame(Frm,NULL,GetClassName()) )
        {
            delete Frm;
            return NULL;
        }
    }
    else
    {
        GetRootItem()->BuildPreview(Frm,true);
    }

    return Frm;
}
