#ifndef WXSRESOURCETREEITEMDATA_H
#define WXSRESOURCETREEITEMDATA_H

#include <wx/treectrl.h>
#include <wx/menu.h>

/** \brief Data used by resource tree */
class wxsResourceTreeItemData: public wxTreeItemData
{
    public:

        /** \brief Ctor */
        wxsResourceTreeItemData();

        /** \brief Dctor */
        virtual ~wxsResourceTreeItemData();

    protected:

        /** \brief Called when corresponding tree item has been selected */
        virtual void OnSelect() {}

        /** \brief Called when right click was made on item */
        virtual void OnRightClick() {}

        /** \brief Called when popup menu generated event with given id
         *  \return true when event was processed, false otherwise
         */
        virtual bool OnPopup(long Id) { return false; }


        /** \brief Function to properly display popup menu */
        void PopupMenu(wxMenu* Menu);

    private:

        friend class wxsResourceTree;
};

#endif
