#ifndef WXSRESOURCETREE_H
#define WXSRESOURCETREE_H

#include <wx/treectrl.h>

#include "wxsglobals.h"

class WXSCLASS wxsWidget;
class WXSCLASS wxsResource;
class WXSCLASS wxsProject;

/** Resource tree class */
class WXSCLASS wxsResourceTree: public wxTreeCtrl
{
    public:

        wxsResourceTree(wxWindow* Parent):
            wxTreeCtrl(Parent,-1)
        {}

    private:

        void OnSelectResource(wxTreeEvent& event);
        void OnRightClick(wxTreeEvent& event);
        void OnBeginDrag(wxTreeEvent& event);
        void OnEndDrag(wxTreeEvent& event);
        void OnConfigureProject(wxCommandEvent& event);

        wxsWidget* Dragged;
        wxsProject* SelectedProject;
        DECLARE_EVENT_TABLE()
};

/** Data used by resource tree */
class WXSCLASS wxsResourceTreeData: public wxTreeItemData
{
    public:

        wxsResourceTreeData(wxsWidget* _Widget):  wxTreeItemData(),   Type(tWidget)   { Widget = _Widget; }
        wxsResourceTreeData(wxsResource* _Resource): wxTreeItemData(), Type(tResource) { Resource = _Resource; }
        wxsResourceTreeData(wxsProject* _Project): wxTreeItemData(), Type(tProject) { Project = _Project; }

        virtual ~wxsResourceTreeData() {}

        enum TypeT
        {
            None,
            tWidget,
            tResource,
            tProject
        };

        TypeT Type;

        union
        {
            wxsWidget* Widget;
            wxsResource* Resource;
            wxsProject* Project;
        };
};





#endif
