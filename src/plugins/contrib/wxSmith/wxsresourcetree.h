#ifndef WXSRESOURCETREE_H
#define WXSRESOURCETREE_H

#include <wx/treectrl.h>

#include "wxsglobals.h"

class wxsWidget;
class wxsResource;
class wxsProject;

/** Resource tree class */
class wxsResourceTree: public wxTreeCtrl
{
    public:

        wxsResourceTree(wxWindow* Parent):
            wxTreeCtrl(Parent,-1),
            SkipSelBit(false)
        {}

        bool SkipSelectionChange(bool Skip=true) { bool Old = SkipSelBit; SkipSelBit = Skip; return Old; }

    private:

        void OnSelectResource(wxTreeEvent& event);
        void OnRightClick(wxTreeEvent& event);
        void OnBeginDrag(wxTreeEvent& event);
        void OnEndDrag(wxTreeEvent& event);
        void OnConfigureProject(wxCommandEvent& event);

        wxsWidget* Dragged;
        wxsProject* SelectedProject;
        bool SkipSelBit;
        DECLARE_EVENT_TABLE()
};

/** Data used by resource tree */
class wxsResourceTreeData: public wxTreeItemData
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
