#include "wxsheaders.h"
#include "wxsextresmanager.h"

#include "resources/wxswindowres.h"
#include "wxsmith.h"

wxsExtResManager::wxsExtResManager()
{
    if ( Singleton==NULL ) Singleton = this;
}

wxsExtResManager::~wxsExtResManager()
{
    for ( FilesMapI i = Files.begin(); i!=Files.end(); ++i )
    {
        delete i->second;
    }
    Files.clear();
    if ( Singleton==this ) Singleton = NULL;
}

int wxsExtResManager::OpenXrc(const wxString& FileName)
{
    TiXmlDocument Doc;
    if ( !Doc.LoadFile(cbU2C(FileName)) )
    {
        wxMessageBox(_("Error occured while loading XRC file"));
        return 1;
    }

    TiXmlHandle Hnd(&Doc);
    TiXmlElement* Elem = Hnd.FirstChildElement("resource").FirstChildElement("object").Element();

    if ( !Elem )
    {
        wxMessageBox(_("Invalid XRC file structure"));
        return 1;
    }

    wxString Class = cbC2U(Elem->Attribute("class"));
    wxString Name = cbC2U(Elem->Attribute("name"));

    wxsWindowRes* NewRes = NULL;

    if ( Class == _T("wxDialog") )
    {
        NewRes = new wxsDialogRes(Name,FileName);
    }
    else if ( Class == _T("wxFrame") )
    {
        NewRes = new wxsFrameRes(Name,FileName);
    }
    else if ( Class == _T("wxPanel") )
    {
        NewRes = new wxsPanelRes(Name,FileName);
    }
    else
    {
        wxMessageBox(_("Unsupported resource type"));
        return 1;
    }

    NewRes->Load();

    if ( Files.empty() )
    {
        TreeId = wxsTREE()->AppendItem(wxsTREE()->GetRootItem(),_("External resources"));
    }

    NewRes->BuildTree(wxsTREE(),TreeId);
    NewRes->EditOpen();

    Files[FileName] = NewRes;
    return 0;
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
                wxsTREE()->Delete(TreeId);
            }
            return;
        }
    }
}

wxsExtResManager* wxsExtResManager::Singleton = NULL;
