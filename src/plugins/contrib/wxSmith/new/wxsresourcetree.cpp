#include "wxsresourcetree.h"
#include "wxsresourcetreeitemdata.h"

namespace
{
    class wxsResourceTreeProjectData: public wxsResourceTreeItemData
    {
        public:
            wxsResourceTreeProjectData(wxsProject* Project): m_Project(Project) {}
            wxsProject* m_Project;
            void OnRightClick()
            {

            }

    };
}

wxsResourceTree* wxsResourceTree::m_Singleton = NULL;

wxsResourceTree::wxsResourceTree(wxWindow* Parent): wxTreeCtrl(Parent,-1), m_IsExt(false)
{
    m_Singleton = this;
}

wxsResourceTree::~wxsResourceTree()
{
    if ( m_Singleton == this ) m_Singleton = NULL;
}

wxsResourceItemId wxsResourceTree::NewProjectItem(const wxString& ProjectTitle,wxsProject* Project)
{
    wxsResourceItemId Id;
    if ( !m_IsExt )
    {
        Id = AppendItem(GetRootItem(),
            ProjectTitle,-1,-1,
            new wxsResourceTreeProjectData(Project));
    }
    else
    {
        Id = InsertItem(GetRootItem(),GetChildrenCount(GetRootItem(),false),
            ProjectTitle,-1,-1,
            new wxsResourceTreeProjectData(Project));
    }

    Expand(GetRootItem());
    return Id;
}

wxsResourceItemId wxsResourceTree::ExternalResourcesId()
{
    if ( !m_IsExt )
    {
        m_ExtId = AppendItem(GetRootItem(),
            _("External resources"),-1,-1,NULL);
        m_IsExt = true;
    }
    return m_ExtId;
}

void wxsResourceTree::DeleteExternalResourcesId()
{
    if ( m_IsExt )
    {
        Delete(m_ExtId);
        m_IsExt = false;
    }
}

void wxsResourceTree::OnSelect(wxTreeEvent& event)
{
    wxsResourceTreeItemData* Data = (wxsResourceTreeItemData*)GetItemData(event.GetItem());
    if ( Data ) Data->OnSelect();
}

void wxsResourceTree::OnRightClick(wxTreeEvent& event)
{
    wxsResourceTreeItemData* Data = (wxsResourceTreeItemData*)GetItemData(event.GetItem());
    if ( Data ) Data->OnRightClick();
}

BEGIN_EVENT_TABLE(wxsResourceTree,wxTreeCtrl)
    EVT_TREE_SEL_CHANGED(wxID_ANY,wxsResourceTree::OnSelect)
    EVT_TREE_ITEM_RIGHT_CLICK(wxID_ANY,wxsResourceTree::OnRightClick)
END_EVENT_TABLE()
