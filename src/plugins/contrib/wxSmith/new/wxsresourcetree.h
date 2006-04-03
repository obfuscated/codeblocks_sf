#ifndef WXSRESOURCETREE_H
#define WXSRESOURCETREE_H

#include <wx/treectrl.h>

class wxsItem;
class wxsResource;
class wxsProject;

/** \brief Resource tree class */
class wxsResourceTree: public wxTreeCtrl
{
    public:

        /** \brief Ctor */
        wxsResourceTree(wxWindow* Parent);

        /** \brief Dctor */
        virtual ~wxsResourceTree();

        /** \brief Function building new tree item for project
         *
         * This item takes name of associated cbProject and initializes
         * item data to point to project.
         */
        wxTreeItemId NewProjectItem(wxsProject* Project);

        /** \brief Function returning item id for external resources */
        wxTreeItemId ExternalResourcesId();

        /** \brief Function deleting identifier for external resources */
        void DeleteExternalResourcesId();

        /** \brief Getting singleton instance */
        static inline wxsResourceTree* Get() { return Singleton; }

        /** \brief Notifying about selection change */
        void SelectionChanged(wxsItem* RootItem);

    private:

        void OnSelectResource(wxTreeEvent& event);
        void OnRightClick(wxTreeEvent& event);
        void OnBeginDrag(wxTreeEvent& event);
        void OnEndDrag(wxTreeEvent& event);
        void OnConfigureProject(wxCommandEvent& event);

        wxsItem* Dragged;
        wxsProject* SelectedProject;
        wxTreeItemId ExtId;
        bool IsExt;
        bool BlockSelect;

        static wxsResourceTree* Singleton;

        DECLARE_EVENT_TABLE()
};

/** \brief Data used by resource tree */
class wxsResourceTreeData: public wxTreeItemData
{
    public:

        wxsResourceTreeData(wxsItem* _Item):         wxTreeItemData(), Type(tItem)     { Item = _Item; }
        wxsResourceTreeData(wxsResource* _Resource): wxTreeItemData(), Type(tResource) { Resource = _Resource; }
        wxsResourceTreeData(wxsProject* _Project):   wxTreeItemData(), Type(tProject)  { Project = _Project; }

        enum TypeT { None, tItem, tResource, tProject };

        TypeT Type;

        union
        {
            wxsItem* Item;
            wxsResource* Resource;
            wxsProject* Project;
        };
};

#endif
