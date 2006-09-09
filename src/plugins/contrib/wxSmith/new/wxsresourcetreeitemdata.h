#ifndef WXSRESOURCETREEITEMDATA_H
#define WXSRESOURCETREEITEMDATA_H

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
        void OnSelect() {}

        /** \brief Called when right click was made on item */
        void OnRightClick() {}

    private:

        friend class wxsResourceTree;
};

#endif
