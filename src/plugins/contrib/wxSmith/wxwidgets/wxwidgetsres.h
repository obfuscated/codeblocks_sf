/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#ifndef WXWIDGETSRES_H
#define WXWIDGETSRES_H

#include "../wxsresource.h"

/** \brief Base class for all resources used inside wxWidgets */
class wxWidgetsRes: public wxsResource
{
    DECLARE_CLASS(wxWidgetsRes)
    public:

        /** \brief Ctor */
        wxWidgetsRes(wxsProject* Owner,const wxString& ResourceType):
            wxsResource(Owner,ResourceType,_T("wxWidgets"))
        {}

        /** \brief Checking if this resource does use XRC */
        virtual bool OnGetUsingXRC() = 0;

        /** \brief Checking if this resource can be main resource in app */
        virtual bool OnGetCanBeMain() = 0;

};

#endif
