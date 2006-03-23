#ifndef WXSDIALOGRES_H
#define WXSDIALOGRES_H

#include "wxswindowres.h"

class wxsDialogRes : public wxsWindowRes
{
    public:

        /** \brief Ctor */
        wxsDialogRes(wxsProject* Project): wxsWindowRes(Project)
        {}

    protected:

        /** \brief Function generating root item */
        virtual wxsItem* BuildRootItem();

        /** \brief Function returning name of root item's class */
        virtual wxString GetRootItemClass();

        /** \brief Function generating preview for this resouce */
        virtual wxWindow* BuildPreview();
};

#endif
