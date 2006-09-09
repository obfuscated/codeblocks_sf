#include "wxsresource.h"
#include "wxsextresmanager.h"

wxsResource::wxsResource(wxsProject* Owner,const wxString& ResourceName,const wxString& ResourceType,wxsCodingLang Language,bool UsingXRC):
    m_ResourceType(ResourceType),
    m_ResourceName(ResourceName),
    m_Owner(Owner),
    m_Language(Language),
    m_UsingXRC(UsingXRC)
{}

wxsResource::~wxsResource()
{}

void wxsResource::EditOpen()
{
    if ( m_Editor )
    {
        m_Editor->Activate();
    }
    else
    {
        m_Editor = OnCreateEditor();
    }
}

void wxsResource::EditClose()
{
    if ( m_Editor )
    {
        m_Editor->Close();
        // Inside Close() m_Editor should be zeroed
    }
}

void wxsResource::EditorClosed()
{
    m_Editor = NULL;
    if ( !m_Owner )
    {
        wxsExtRes()->EditorClosed(this);
    }
}

void wxsResource::BuildTreeEntry(const wxsResourceItemId& Parent)
{
    m_TreeItemId = wxsTree()->AppendItem(Parent,GetResourceName());
}
