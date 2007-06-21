#ifndef WXSADVQPPCHILD_H
#define WXSADVQPPCHILD_H

#include <wx/panel.h>
#include "wxsadvqpp.h"

/** \brief Child of advanced quick props panel */
class wxsAdvQPPChild : public wxPanel
{
    public:

        /** \brief Ctor */
        wxsAdvQPPChild(wxsAdvQPP* Parent,const wxString Title);

        /** \brief Dctor */
        virtual ~wxsAdvQPPChild();

        /** \brief Getting container class
         *  \note You should always compare returned value to 0 since
         *        container may be accidentially unbinded from quick properties
         *        panel.
         */
        inline wxsPropertyContainer* GetPropertyContainer()
        {
            return Parent->GetPropertyContainer();
        }

    protected:

        /** \brief Function which notifies that content of panel should be reloaded */
        virtual void Update() = 0;

        /** \brief Function which can be called to notify that user changed value of any property in this panel */
        inline void NotifyChange()
        {
            Parent->NotifyChange();
        }

    private:

        wxsAdvQPP* Parent;

        friend class wxsAdvQPP;
};

#endif
