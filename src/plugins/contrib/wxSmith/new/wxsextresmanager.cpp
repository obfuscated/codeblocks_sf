#include "wxsextresmanager.h"
#include "wxsmith.h"
#include "wxsresourcefactory.h"

wxsExtResManager::wxsExtResManager()
{
}

wxsExtResManager::~wxsExtResManager()
{
    int TestCnt = m_Files.size();
    while ( !m_Files.empty() )
    {
        // Each delete should remove one entry
        delete m_Files.begin()->second;
        // Just in case of invalid resources to avoid infinite loops
        if ( --TestCnt < 0 ) break;
    }
}

bool wxsExtResManager::CanOpen(const wxString& FileName)
{
    if ( m_Files.find(FileName) != m_Files.end() ) return true;
    return wxsResourceFactory::CanHandleExternal(FileName);
}

bool wxsExtResManager::Open(const wxString& FileName)
{
    if ( m_Files.find(FileName) == m_Files.end() )
    {
        wxsResource* NewResource = wxsResourceFactory::BuildExternal(FileName);
        if ( !NewResource ) return false;
        NewResource->BuildTreeEntry(wxsTree()->ExternalResourcesId());
        m_Files[FileName] = NewResource;
        NewResource->EditOpen();
        return true;
    }

    m_Files[FileName]->EditOpen();
    return true;
}

void wxsExtResManager::EditorClosed(wxsResource* Res)
{
    for ( FilesMapI i = m_Files.begin(); i!=m_Files.end(); ++i )
    {
        if ( i->second == Res )
        {
            m_Files.erase(i);
            wxsTree()->Delete(Res->GetTreeItemId());
            delete Res;
            if ( m_Files.empty() )
            {
                wxsTree()->DeleteExternalResourcesId();
            }
            return;
        }
    }
}

wxsExtResManager wxsExtResManager::m_Singleton;

/*
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
*/

