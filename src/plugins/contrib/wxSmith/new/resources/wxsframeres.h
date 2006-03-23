#ifndef WXSFRAMERES_H
#define WXSFRAMERES_H

#include "wxswindowres.h"

class wxsFrameRes : public wxsWindowRes
{
    public:

        /** \brief Ctor */
        wxsFrameRes(wxsProject* Project): wxsWindowRes(Project)
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
