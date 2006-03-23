#include "wxsextresmanager.h"

#include "resources/wxsdialogres.h"
#include "resources/wxsframeres.h"
#include "resources/wxspanelres.h"
#include "wxsmith.h"

wxsExtResManager::wxsExtResManager()
{
}

wxsExtResManager::~wxsExtResManager()
{
    for ( FilesMapI i = Files.begin(); i!=Files.end(); ++i )
    {
        delete i->second;
    }
    Files.clear();
}

bool wxsExtResManager::Open(const wxString& FileName)
{
    FilesMapI i = Files.find(FileName);
    if ( i!=Files.end() )
    {
        // File is already opened in editor
        i->second->EditOpen();
        return true;
    }

    wxString Ext = wxFileName(FileName).GetExt().Upper();
    if ( Ext == _T("XRC") )
    {
        return OpenXrc(FileName);
    }

    return false;
}

bool wxsExtResManager::OpenXrc(const wxString& FileName)
{
    TiXmlDocument Doc;
    if ( !Doc.LoadFile(cbU2C(FileName)) )
    {
        wxMessageBox(_("Error occured while loading XRC file"));
        return false;
    }

    TiXmlHandle Hnd(&Doc);
    TiXmlElement* Elem = Hnd.FirstChildElement("resource").FirstChildElement("object").Element();

    if ( !Elem )
    {
        wxMessageBox(_("Invalid XRC file structure"));
        return false;
    }

    wxString Class = cbC2U(Elem->Attribute("class"));
    wxString Name = cbC2U(Elem->Attribute("name"));

    wxsWindowRes* NewRes = NULL;

    if ( Class == _T("wxDialog") )
    {
        NewRes = new wxsDialogRes(NULL);
    }
    else if ( Class == _T("wxFrame") )
    {
        NewRes = new wxsFrameRes(NULL);
    }
    else if ( Class == _T("wxPanel") )
    {
        NewRes = new wxsPanelRes(NULL);
    }
    else
    {
        wxMessageBox(_("Unsupported resource type"));
        return false;
    }

    NewRes->BindExternalResource(FileName,Name);
    wxTreeItemId TreeId = wxsTREE()->ExternalResourcesId();
    NewRes->BuildTree(wxsTREE(),TreeId);
    NewRes->EditOpen();
    Files[FileName] = NewRes;
    return true;
}

void wxsExtResManager::ResClosed(wxsResource* Res)
{
    for ( FilesMapI i = Files.begin(); i!=Files.end(); ++i )
    {
        if ( i->second == Res )
        {
            Files.erase(i);
            wxsTREE()->Delete(Res->GetTreeItemId());
            delete Res;
            if ( Files.empty() )
            {
                wxsTREE()->DeleteExternalResourcesId();
            }
            return;
        }
    }
}

wxsExtResManager wxsExtResManager::Singleton;
