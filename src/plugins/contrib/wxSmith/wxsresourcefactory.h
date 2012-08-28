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

#ifndef WXSRESOURCEFACTORY_H
#define WXSRESOURCEFACTORY_H

#include <wx/string.h>
#include <wx/menu.h>

class wxsResource;
class wxsProject;

/** \brief This class is responsible for creating resource classes
 *
 * This class is provided as singleton
 */
class wxsResourceFactory
{
    public:

        /** \brief Function generating resource class */
        static wxsResource* Build(const wxString& ResourceType,wxsProject* Project);

        /** \brief Checking if factory can handle given file as external resource */
        static bool CanHandleExternal(const wxString& FileName);

        /** \brief Building external resource object */
        static wxsResource* BuildExternal(const wxString& FileName);

        /** \brief Building wxSmith menu entries */
        static void BuildSmithMenu(wxMenu* menu);

        /** \brief Processing menu event for creating new resources
         *
         * This function is triggered when there's a chance that "Add ..." menu from
         * wxSmith has been clicked. In that case, wxsResourceFactory will process it
         * and return true. If it's not "Add ..." menu entry, it will return false
         */
        static bool NewResourceMenu(int Id,wxsProject* Project);

        /** \brief Getting tree icon id for given resource type */
        static int ResourceTreeIcon(const wxString &ResourceType);

        /** \brief Calling OnAttach for all factories */
        static void OnAttachAll();

        /** \brief Calling OnRelease for all factories */
        static void OnReleaseAll();

    protected:

        /** \brief Ctor */
        wxsResourceFactory();

        /** \brief Dctor */
        virtual ~wxsResourceFactory();

        /** \brief Called when plugin is being attached */
        virtual void OnAttach() {}

        /** \brief Called when plugin is being released */
        virtual void OnRelease() {}

        /** \brief Getting number of resource types inside this factory */
        virtual int OnGetCount() = 0;

        /** \brief Checking if given resource can be main in application */
        virtual void OnGetInfo(int Number,wxString& Name,wxString& GUI) = 0;

        /** \brief creating resource */
        virtual wxsResource* OnCreate(int Number,wxsProject* Project) = 0;

        /** \brief Checking if factory can handle given file as external resource */
        virtual bool OnCanHandleExternal(const wxString& FileName) { return false; }

        /** \brief Building external resource object */
        virtual wxsResource* OnBuildExternal(const wxString& FileName) { return 0; }

        /** \brief Function creating new resource object
         *
         * This function is responsible for creating new resource, adding it into project,
         * additionally adding new files to project and initialize resource with proper
         * data. Usually it will require some dialog box before creating resource
         * to get needed params.
         */
        virtual bool OnNewWizard(int Number,wxsProject* Project) = 0;

        /** \brief Function getting icon in resource browser for this resource entry */
        virtual int OnResourceTreeIcon(int Number) { return -1; }

    private:

        struct ResourceInfo
        {
            wxsResourceFactory* m_Factory;
            int m_Number;
            wxString m_GUI;
            int m_MenuId;
            ResourceInfo(): m_Factory(0), m_Number(0), m_MenuId(-1) {}
        };

        WX_DECLARE_STRING_HASH_MAP(ResourceInfo,HashT);

        wxsResourceFactory* m_Next;
        bool m_Attached;
        static wxsResourceFactory* m_UpdateQueue;
        static wxsResourceFactory* m_Initialized;
        static HashT m_Hash;
        static wxString m_LastExternalName;
        static wxsResourceFactory* m_LastExternalFactory;
        static bool m_AllAttached;

        static void InitializeFromQueue();
        inline void Initialize();

};

#endif
