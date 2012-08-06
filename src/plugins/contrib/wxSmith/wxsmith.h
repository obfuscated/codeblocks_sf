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

#ifndef WXSMITH_H
#define WXSMITH_H

#include <wx/splitter.h>
#include <wx/scrolwin.h>

#include <cbplugin.h>
#include <settings.h>
#include <sdk_events.h>
#include <tinyxml/tinyxml.h>

#include "wxsstoringsplitterwindow.h"
#include "wxsresourcetree.h"

class wxsProject;
class wxsItem;
class wxsResource;

/** \brief Main plugin which will handle most of wxSmith's work
 *
 * This plugin is responsible for managing wxsProject classes - addition to
 * standard cbProject class and is used for global notifications like
 * selecting item.
 *
 * This class is a singleton, is registered in OnAttach and unregistered in
 * OnRelease.
 */
class wxSmith : public cbPlugin
{
    public:

        /** \brief Ctor */
        wxSmith();

        /** \brief Dctor */
        ~wxSmith();

        /** \brief Function returning singleton instance */
        static wxSmith* Get() { return m_Singleton; }

        /** \brief Getting group in configuration dialog */
        int GetConfigurationGroup()  const { return cgEditor; }

        /** \brief Creating configuration panel */
        cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);

        /** \brief Generating extra configuration panel inside project options */
        virtual cbConfigurationPanel* GetProjectConfigurationPanel(wxWindow* parent, cbProject* project);

        /** \brief This function will add entries into menu */
        void BuildMenu(wxMenuBar* menuBar);

        void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0);
        bool BuildToolBar(wxToolBar* toolBar);
        void OnAttach();
        void OnRelease(bool appShutDown);

        /** \brief Registering scripting stuff */
        void RegisterScripting();

        /** \brief unregistering scripting stuff */
        void UnregisterScripting();

        /** \brief Getting wxsProject addition for given cbPrject */
        wxsProject* GetSmithProject(cbProject* Proj);

        /** \brief Getting cbProject class from wxsProject addition */
        cbProject* GetCBProject(wxsProject* Proj);

        /** \brief Helper operator used to convert C::B projects to wxS projects */
        inline wxsProject* operator[](cbProject* Proj) { return GetSmithProject(Proj); }

        /** \brief Helper operator used to convert wxS projects to C::B projects */
        inline cbProject* operator[](wxsProject* Proj) { return GetCBProject(Proj); }

        /** \brief Bringing up the resources tab to the view */
        void ShowResourcesTab();

    private:

        WX_DECLARE_HASH_MAP(cbProject*,wxsProject*,wxPointerHash,wxPointerEqual,ProjectMapT);
        typedef ProjectMapT::iterator ProjectMapI;

        ProjectMapT m_ProjectMap;               ///< \brief Map binding all cbProject classes with wxsProject ones
        wxsStoringSplitterWindow* m_Splitter;   ///< \brief Splitter window used to divide resource browser and property grid
        wxPanel* m_ResourceBrowserParent;       ///< \brief Parent panel for resource browser
        wxPanel* m_PropertyBrowserParent;       ///< \brief Parent for property browser
        int m_HookId;                           ///< \brief Project hook identifier used when deleting hook
        static wxSmith* m_Singleton;            ///< \brief Singleton object

        /** \brief Procedure called when loading/saving project, used to load/save additional configuration from/to .cbp file */
        void OnProjectHook(cbProject*,TiXmlElement*,bool);

        /** \brief PRocedure called when project has been loaded */
        void OnProjectOpened(CodeBlocksEvent& event);

        /** \brief Procedure called when closing project, removes additional stuff associated with project */
        void OnProjectClose(CodeBlocksEvent& event);

        /** \brief Procedure called when some project has been renamed - it will update resource browser */
        void OnProjectRenamed(CodeBlocksEvent& event);

        /** \brief Called when clicked "Configure..." from wxSmith menu */
        void OnConfigure(wxCommandEvent& event);

        /** \brief Called for any menu, used to find possible "Add ...." enteries */
        void OnMenu(wxCommandEvent& event);

        /** \brief Function building resource and properties browser */
        void BuildBrowsers();

        /** \brief Function Generating background panes for resource and property browsers */
        void BuildBrowserParents();

        /** \brief Function allowing to recover invalid wxs file */
        static bool RecoverWxsFile( const wxString& WxsResourceSettings );

        void OnViewBrowsers(wxCommandEvent& event);
        void OnViewResourceBrowser(wxCommandEvent& event);
        void OnViewPropertyBrowser(wxCommandEvent& event);
        void OnUpdateUI(wxUpdateUIEvent& event);

        friend class wxSmithMime;
        DECLARE_EVENT_TABLE()
};

/** \brief Helper function to easily access wxSmith plugin */
inline wxSmith* wxsPlugin() { return wxSmith::Get(); }

/** \brief Helper function to access project converter (wxSmith project <-> C::B project) */
inline wxSmith* wxsProjectConv() { return wxSmith::Get(); }

#endif

