#ifndef WXSRESOURCETREE_H
#define WXSRESOURCETREE_H

#include <wx/treectrl.h>

class WXSCLASS wxsWidget;
class WXSCLASS wxsResource;

/** Resource tree class */
class WXSCLASS wxsResourceTree: public wxTreeCtrl
{
    public:

        wxsResourceTree(wxWindow* Parent):
            wxTreeCtrl(Parent,-1)
        {}

    private:

        void OnSelectResource(wxTreeEvent& event);
        void OnBeginDrag(wxTreeEvent& event);
        void OnEndDrag(wxTreeEvent& event);

        wxsWidget* Dragged;
        DECLARE_EVENT_TABLE()
};

/** Data used by resource tree */
class WXSCLASS wxsResourceTreeData: public wxTreeItemData
{
    public:

        wxsResourceTreeData(wxsWidget* _Widget):  wxTreeItemData(),   Type(tWidget)   { Widget = _Widget; }
        wxsResourceTreeData(wxsResource* _Resource): wxTreeItemData(), Type(tResource) { Resource = _Resource; }

        virtual ~wxsResourceTreeData();

        enum TypeT
        {
            None,
            tWidget,
            tResource
        };

        TypeT Type;

        union
        {
            wxsWidget* Widget;
            wxsResource* Resource;
        };
};





#endif
