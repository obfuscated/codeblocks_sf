#include <wx/aui/aui.h>
#include <sdk.h> // Code::Blocks SDK
#ifndef CB_PRECOMP
    #include <cbauibook.h>
    #include <cbproject.h>
    #include <projectmanager.h>
#endif
//#include <configurationpanel.h>

#include "FileManager.h"

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    PluginRegistrant<FileManagerPlugin> reg(_T("FileManager"));
}

int ID_ProjectOpenInFileBrowser=wxNewId();

BEGIN_EVENT_TABLE(FileManagerPlugin, cbPlugin)
    EVT_MENU(ID_ProjectOpenInFileBrowser, FileManagerPlugin::OnOpenProjectInFileBrowser)
END_EVENT_TABLE()


// constructor
FileManagerPlugin::FileManagerPlugin()
{
    if(!Manager::LoadResource(_T("FileManager.zip")))
    {
        NotifyMissingFile(_T("FileManager.zip"));
    }
    m_fe=0;
}

// destructor
FileManagerPlugin::~FileManagerPlugin()
{
}

void FileManagerPlugin::OnAttach()
{
    //Create a new instance of the FileExplorer and attach it to the Project Manager notebook
    m_fe=new FileExplorer(Manager::Get()->GetAppWindow());
    Manager::Get()->GetProjectManager()->GetNotebook()->AddPage(m_fe,_T("Files"));
}

void FileManagerPlugin::OnRelease(bool appShutDown)
{
    if (m_fe) //remove the File Explorer from the managment pane and destroy it.
    {
        int idx = Manager::Get()->GetProjectManager()->GetNotebook()->GetPageIndex(m_fe);
        if (idx != -1)
            Manager::Get()->GetProjectManager()->GetNotebook()->RemovePage(idx);
        delete m_fe;
    }
    m_fe = 0;
}

void FileManagerPlugin::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data)
{
	if(type==mtProjectManager && data && data->GetKind()==FileTreeData::ftdkProject)
	{
	    m_project_selected=wxFileName(data->GetProject()->GetFilename()).GetPath();
        menu->Append(ID_ProjectOpenInFileBrowser, _T("Open Project Folder in File Browser"), _("Opens the folder containing the project file in the file browser"));
	}
}

void FileManagerPlugin::OnOpenProjectInFileBrowser(wxCommandEvent& event)
{
    cbAuiNotebook *m_nb=Manager::Get()->GetProjectManager()->GetNotebook();
    m_nb->SetSelection(m_nb->GetPageIndex(m_fe));
    m_fe->SetRootFolder(m_project_selected);
}
