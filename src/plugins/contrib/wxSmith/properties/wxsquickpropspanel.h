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

#ifndef WXSQUICKPROPSPANEL_H
#define WXSQUICKPROPSPANEL_H

#include <wx/panel.h>

class wxsPropertyContainer;

/** \brief Panel used for building quick properties
 *
 * This panel is used as parent window for quick properties.
 * Quick properties is additional panel created inside editor with set
 * of most used properties and with ability to easily change them.
 * It was introduced since standard property grid can sometimes slow things
 * down and base editor area has limited size (and usually can not be anything
 * else than text editor or combo box).
 *
 * Important thing is that when reading / writing properties to panel,
 * GetPropertiesContaier method should be used. And it must always be compared
 * to 0. If it's null, it does mean that property container has already been
 * deleted but quick properties window still remains.
 *
 * Note that NotifyChange() must be called any time value of property changes.
 */
class wxsQuickPropsPanel: public wxPanel
{
    public:

        /** \brief ctor
         *
         * This ctor requires pointer to property container only. When using
         * it, wxPanel class must be initialized using Create() method.
         */
        wxsQuickPropsPanel(wxsPropertyContainer* PC);

        /** \brief ctor
         *
         * Set of parameters is almost identical to wxPanel ones. Added
         * wxsPropertyContainer param.
         */
        wxsQuickPropsPanel(
            wxWindow* parent,
            wxsPropertyContainer* PC,
            wxWindowID id = -1,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxTAB_TRAVERSAL,
            const wxString& name = _T("wxsQuickPropsPanel"));

        /** \brief Dctor */
        virtual ~wxsQuickPropsPanel();

        /** \brief Getting associated property container */
        inline wxsPropertyContainer* GetPropertyContainer() { return Container; }

    protected:

        /** \brief Function notifying about change of properties in container
         *
         * This function is called whenever user changed value of any property
         * in property grid or elsewhere. It should reread all values and
         * update content of quick properties panel.
         */
        virtual void Update() {}

        /** \brief Function notifying that value of any property has changed inside quick properties.
         *
         * This function causes property grid and other things using properties
         * to recreate it's content.
         */
        void NotifyChange();

    private:

        wxsPropertyContainer* Container;        ///< Container associated with this panel or 0

        friend class wxsPropertyContainer;
};

#endif
