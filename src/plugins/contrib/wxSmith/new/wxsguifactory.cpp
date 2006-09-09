#include "wxsguifactory.h"
#include "wxsgui.h"

wxsGUIFactory* wxsGUIFactory::m_UpdateQueue = NULL;
wxsGUIFactory::GUIItemHashT wxsGUIFactory::m_Hash;

wxsGUIFactory::wxsGUIFactory()
{
    m_Next = m_UpdateQueue;
    m_UpdateQueue = this;
}

wxsGUIFactory::~wxsGUIFactory()
{
    // Assuming that factories are available
    // during all wxSmith work time, it's not
    // necessarry to remove any bindings
}

inline void wxsGUIFactory::InitializeFromQueue()
{
    while ( m_UpdateQueue )
    {
        wxsGUIFactory* NextFactory = m_UpdateQueue->m_Next;
        m_UpdateQueue->Initialize();
        m_UpdateQueue = NextFactory;
    }
}

inline void wxsGUIFactory::Initialize()
{
    // Reading all items and adding into hash list
    for ( int i = OnGetCount(); i-->0; )
    {
        GUIItem& Item = m_Hash[OnGetName(i)];
        Item.m_Factory = this;
        Item.m_Number = i;
    }
}

wxsGUI* wxsGUIFactory::Build(const wxString& Name,wxsProject* Project)
{
    InitializeFromQueue();
    GUIItem& Item = m_Hash[Name];
    if ( Item.m_Factory == NULL ) return NULL;
    wxsGUI* NewGUI = Item.m_Factory->OnCreate(Item.m_Number,Project);
    if ( NewGUI->GetName() != Name )
    {
        // Some hack? Bug in factory?
        DBGLOG(_T("wxSmith: Error while creating wxsGUI object (name mismatch)."));
        DBGLOG(_T("wxSmith:   Looks like bug in one wf wxsGUIFactory-derived classes or"));
        DBGLOG(_T("wxSmith:   some hack attempt."));
        delete NewGUI;
        return NULL;
    }
    return NewGUI;
}

wxsGUI* wxsGUIFactory::SelectNew(const wxString& Message,wxsProject* Project)
{
    InitializeFromQueue();
    if ( m_Hash.empty() )
    {
        return NULL;
    }
    if ( m_Hash.size() == 1 )
    {
        return Build(m_Hash.begin()->first,Project);
    }

    wxArrayString GUIList;
    for ( GUIItemHashT::iterator i = m_Hash.begin(); i!=m_Hash.end(); ++i )
    {
        GUIList.Add(i->first);
    }

    wxString SelectedGUI = ::wxGetSingleChoice(Message,_("Select GUI"), GUIList);
    if ( SelectedGUI.empty() )
    {
        return NULL;
    }

    return Build(SelectedGUI,Project);
}
