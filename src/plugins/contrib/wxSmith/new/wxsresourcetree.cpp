#include "wxsresourcetree.h"
#include "wxsmith.h"
#include "wxsproject.h"
#include "wxsglobals.h"
#include "wxsitem.h"
#include "resources/wxswindowres.h"
//#include "resources/wxswindowresdataobject.h"
//#include "wxsevent.h"
//#include "wxswidgetfactory.h"
//#include "wxswinundobuffer.h"

wxsResourceTree* wxsResourceTree::Singleton = NULL;

static const long wxsConfigureProjectId = wxNewId();

wxsResourceTree::wxsResourceTree(wxWindow* Parent): wxTreeCtrl(Parent,-1), IsExt(false), BlockSelect(false)
{
    Singleton = this;
}

wxsResourceTree::~wxsResourceTree()
{
    if ( Singleton == this ) Singleton = NULL;
}

wxTreeItemId wxsResourceTree::NewProjectItem(wxsProject* Project)
{
    wxTreeItemId Id;
    if ( !IsExt )
    {
        Id = AppendItem(GetRootItem(),
            Project->GetCBProject()->GetTitle(),-1,-1,
            new wxsResourceTreeData(Project));
    }
    else
    {
        Id = InsertItem(GetRootItem(),GetChildrenCount(GetRootItem(),false),
            Project->GetCBProject()->GetTitle(),-1,-1,
            new wxsResourceTreeData(Project));
    }

    Expand(GetRootItem());
    return Id;
}

wxTreeItemId wxsResourceTree::ExternalResourcesId()
{
    if ( !IsExt )
    {
        ExtId = AppendItem(GetRootItem(),
            _("External resources"),-1,-1,NULL);
        IsExt = true;
    }
    return ExtId;
}

void wxsResourceTree::DeleteExternalResourcesId()
{
    if ( IsExt )
    {
        Delete(ExtId);
        IsExt = false;
    }
}

void wxsResourceTree::OnSelectResource(wxTreeEvent& event)
{
    if ( BlockSelect ) return;
    BlockSelect = true;

    wxsResourceTreeData* Data = ((wxsResourceTreeData*)GetItemData(event.GetItem()));
    if ( Data )
    {
        switch ( Data->Type )
        {
            case wxsResourceTreeData::tItem:
                {
                    wxsItem* Item = Data->Item;
                    wxsWindowRes* Res = Item->GetResource();
                    Res->GetRootItem()->ClearSelection();
                    Item->SetIsSelected(true);
                    Res->SelectionChanged(Item);
                }
                break;

            case wxsResourceTreeData::tResource:
                {
                    wxsResource* Res = Data->Resource;
                    Res->EditOpen();
                }
                break;

            default:;
        }
    }

    BlockSelect = false;
}

void wxsResourceTree::OnBeginDrag(wxTreeEvent& event)
{
//    wxsResourceTreeData* Data = ((wxsResourceTreeData*)GetItemData(event.GetItem()));
//    if ( !Data ) return;
//    if ( Data->Type == wxsResourceTreeData::tWidget &&
//         Data->Widget->GetParent() )
//    {
//        Dragged = Data->Widget;
//        event.Allow();
//    }
}

void wxsResourceTree::OnEndDrag(wxTreeEvent& event)
{
//    wxsResourceTreeData* Data = ((wxsResourceTreeData*)GetItemData(event.GetItem()));
//    if ( !Data ) return;
//    // Because we're dragging widgets, both widgets muse have open eeditors
//    if ( Data->Type == wxsResourceTreeData::tWidget )
//    {
//        wxsWidget* Dest = Data->Widget;
//        if ( Dest == Dragged ) return;
//
//        // Determining insertion method
//        bool InsertBefore = true;
//
//        if (  !Dest->GetParent() ||
//             ( Dest->GetInfo().Sizer &&
//              !Dest->GetParent()->GetInfo().Sizer ) )
//        {
//            InsertBefore = false;
//        }
//
//        // Copying data
//        wxsWindowResDataObject Data;
//        Data.AddWidget(Dragged);
//
//        wxsWidget* Insert = Data.BuildWidget(Dest->GetResource());
//        if ( !Insert )
//        {
//        	DBGLOG(_("wxSmith: Error while cloning widget"));
//        	return;
//        }
//
//        wxsWindowEditor* Editor1 = (wxsWindowEditor*)Dest->GetResource()->GetEditor();
//        wxsWindowEditor* Editor2 = (wxsWindowEditor*)Dragged->GetResource()->GetEditor();
//        Editor1->StartMultipleChange();
//        bool Done;
//        if ( InsertBefore )
//        {
//            Done = Editor1->InsertBefore(Insert,Dest);
//        }
//        else
//        {
//            Done = Editor1->InsertInto(Insert,Dest);
//        }
//
//        if ( Editor1 != Editor2 )
//        {
//        	Editor1->EndMultipleChange();
//        	Editor2->StartMultipleChange();
//        }
//
//        if ( Done && !::wxGetKeyState(WXK_CONTROL) )
//        {
//        	wxsKILL(Dragged);
//        }
//
//        Dest->GetResource()->CheckBaseProperties(true,NULL);
//
//        Editor2->EndMultipleChange();
//
//        if ( Done )
//        {
//        	Editor1->GetUndoBuff()->StoreChange();
//        }
//    }
}

void wxsResourceTree::OnRightClick(wxTreeEvent& event)
{
    wxMenu Popup;
    wxsResourceTreeData* Data = ((wxsResourceTreeData*)GetItemData(event.GetItem()));
    if ( Data )
    {
        switch ( Data->Type )
        {
            case wxsResourceTreeData::tProject:
                SelectedProject = Data->Project;
                Popup.Append(wxsConfigureProjectId,_("Configure"));
                PopupMenu(&Popup);
                break;

            default:;
        }
    }
}

void wxsResourceTree::OnConfigureProject(wxCommandEvent& event)
{
//    if ( SelectedProject )
//    {
//        SelectedProject->Configure();
//    }
}


void wxsResourceTree::SelectionChanged(wxsItem* RootItem)
{
    // TODO: Support for multiple selection
    UnselectAll();
    SelectItem(RootItem->GetLastTreeItemId());
}

BEGIN_EVENT_TABLE(wxsResourceTree,wxTreeCtrl)
    EVT_TREE_SEL_CHANGED(wxID_ANY,wxsResourceTree::OnSelectResource)
    EVT_TREE_BEGIN_DRAG(wxID_ANY,wxsResourceTree::OnBeginDrag)
    EVT_TREE_BEGIN_RDRAG(wxID_ANY,wxsResourceTree::OnBeginDrag)
    EVT_TREE_END_DRAG(wxID_ANY,wxsResourceTree::OnEndDrag)
    EVT_TREE_ITEM_RIGHT_CLICK(wxID_ANY,wxsResourceTree::OnRightClick)
    EVT_MENU(wxsConfigureProjectId,wxsResourceTree::OnConfigureProject)
END_EVENT_TABLE()
