#ifndef WXSITEMRESTREEDATA_H
#define WXSITEMRESTREEDATA_H

#include "../wxsresourcetreeitemdata.h"

class wxsItem;

/** \brief Class responsible for operations on wxWidgets items inside resource tree */
class wxsItemResTreeData: public wxsResourceTreeItemData
{
    public:

        /** \brief Ctor */
        wxsItemResTreeData(wxsItem* Item);

        /** \brief Dctor */
        virtual ~wxsItemResTreeData();

    private:

        virtual void OnSelect();
        virtual void OnRightClick();

        wxsItem* m_Item;
};


#endif
