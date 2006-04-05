#ifndef WXSPANELRES_H
#define WXSPANELRES_H

#include "wxswindowres.h"

class wxsPanelRes : public wxsWindowRes
{
    public:

        /** \brief Ctor */
        wxsPanelRes(wxsProject* Project): wxsWindowRes(Project)
        {}

    protected:

        /** \brief Function generating root item */
        virtual wxsItem* BuildRootItem();

        /** \brief Function returning name of root item's class */
        virtual wxString GetRootItemClass();

        /** \brief Function generating preview for this resouce */
        virtual wxWindow* BuildPreview();

        /** \brief Generating xrc loading code */
        virtual wxString BuildXrcLoadingCode();
};

#endif
