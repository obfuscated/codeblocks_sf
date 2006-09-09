#include "wxsresourcefactory.h"

wxsResourceFactory* wxsResourceFactory::m_UpdateQueue = NULL;
wxsResourceFactory* wxsResourceFactory::m_Initialized = NULL;
wxsResourceFactory::HashT wxsResourceFactory::m_Hash;
wxString wxsResourceFactory::m_LastExternalName;
wxsResourceFactory* wxsResourceFactory::m_LastExternalFactory = NULL;

wxsResourceFactory::wxsResourceFactory()
{
    m_Next = m_UpdateQueue;
    m_UpdateQueue = this;
}

wxsResourceFactory::~wxsResourceFactory()
{
}

void wxsResourceFactory::InitializeFromQueue()
{
    while ( m_UpdateQueue )
    {
        wxsResourceFactory* NextFactory = m_UpdateQueue->m_Next;
        m_UpdateQueue->Initialize();
        m_UpdateQueue = NextFactory;
    }
}

inline void wxsResourceFactory::Initialize()
{
    for ( int i=OnGetCount(); i-->0; )
    {
        wxString Name;
        wxString GUI;
        bool CanBeMain=false;
        OnGetInfo(i,Name,GUI,CanBeMain);
        ResourceInfo& Info = m_Hash[Name];
        Info.m_Factory = this;
        Info.m_Number = i;
        Info.m_CanBeMain = CanBeMain;
        Info.m_GUI = GUI;
    }

    m_Next = m_Initialized;
    m_Initialized = this;
}

wxsResource* wxsResourceFactory::Build(const wxString& ResourceType,wxsProject* Project)
{
    InitializeFromQueue();
    ResourceInfo& Info = m_Hash[ResourceType];
    if ( !Info.m_Factory )
    {
        return NULL;
    }
    return Info.m_Factory->OnCreate(Info.m_Number,Project);
}

bool wxsResourceFactory::CanBeMain(const wxString& ResourceType)
{
    InitializeFromQueue();
    return m_Hash[ResourceType].m_CanBeMain;
}

bool wxsResourceFactory::CanHandleExternal(const wxString& FileName)
{
    InitializeFromQueue();
    for ( wxsResourceFactory* Factory = m_Initialized; Factory; Factory=Factory->m_Next )
    {
        if ( Factory->OnCanHandleExternal(FileName) )
        {
            m_LastExternalName = FileName;
            m_LastExternalFactory = Factory;
            return true;
        }
    }
    m_LastExternalName = wxEmptyString;
    m_LastExternalFactory = NULL;
    return false;
}

wxsResource* wxsResourceFactory::BuildExternal(const wxString& FileName)
{
    InitializeFromQueue();
    if ( m_LastExternalFactory && (m_LastExternalName==FileName) )
    {
        return m_LastExternalFactory->OnBuildExternal(FileName);
    }
    for ( wxsResourceFactory* Factory = m_Initialized; Factory; Factory=Factory->m_Next )
    {
        wxsResource* Res = Factory->OnBuildExternal(FileName);
        if ( Res ) return Res;
    }
    return NULL;
}

void wxsResourceFactory::BuildSmithMenu(wxMenu* menu)
{
    InitializeFromQueue();
    for ( wxsResourceFactory* Factory = m_Initialized; Factory; Factory=Factory->m_Next )
    {
        Factory->OnBuildSmithMenu(menu);
    }
}

void wxsResourceFactory::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data)
{
    InitializeFromQueue();
    for ( wxsResourceFactory* Factory = m_Initialized; Factory; Factory=Factory->m_Next )
    {
        Factory->OnBuildModuleMenu(type,menu,data);
    }
}

void wxsResourceFactory::BuildToolBar(wxToolBar* toolBar)
{
    InitializeFromQueue();
    for ( wxsResourceFactory* Factory = m_Initialized; Factory; Factory=Factory->m_Next )
    {
        Factory->OnBuildToolBar(toolBar);
    }
}
