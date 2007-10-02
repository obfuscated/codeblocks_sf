/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision$
* $Id$
* $HeadURL$
*/

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
