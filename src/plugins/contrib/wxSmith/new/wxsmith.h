/***************************************************************
 * Name:      wxsmith.h
 * Purpose:   Code::Blocks plugin
 * Author:    BYO<byo@o2.pl>
 * Created:   04/10/05 01:05:08
 * Copyright: (c) BYO
 * License:   GPL
 **************************************************************/

#ifndef WXSMITH_H
#define WXSMITH_H

#include <wx/splitter.h>
#include <wx/scrolwin.h>

#include <cbplugin.h>
#include <settings.h>
#include <sdk_events.h>

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

        /** \brief Function returing singleton instance */
		static wxSmith* Get() { return Singleton; }

        /** \brief Getting group in configuration dialog */
        int GetConfigurationGroup()  const { return cgEditor; }

        /** \brief Creating configuration panel */
        cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);

        /** \brief This funciton will add enteries into menu */
		void BuildMenu(wxMenuBar* menuBar);

		void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0);
		bool BuildToolBar(wxToolBar* toolBar);
		void OnAttach();
		void OnRelease(bool appShutDown);

        /** \brief Global function notifying that given resource has been selected */
        void SelectResource(wxsResource* Resource);

        /** \brief Getting wxsProject addition for given cbPrject */
        wxsProject* GetSmithProject(cbProject* Proj);

        /** \brief Getting cbProject class from wxsProject addition */
        cbProject* GetCBProject(wxsProject* Proj);

	private:
        wxsStoringSplitterWindow* Splitter;

        WX_DECLARE_HASH_MAP(cbProject*,wxsProject*,wxPointerHash,wxPointerEqual,ProjectMapT);
        typedef ProjectMapT::iterator ProjectMapI;

        /** \brief Map binding all cbProject classes with wxsProject ones */
        ProjectMapT ProjectMap;

        /* Event processing functions */
        void OnProjectClose(CodeBlocksEvent& event);
        void OnProjectOpen(CodeBlocksEvent& event);
        void OnProjectActivated(CodeBlocksEvent& event);
        void OnNewWindow(wxCommandEvent& event);
        void OnImportXrc(wxCommandEvent& event);
        void OnConfigure(wxCommandEvent& event);

        /** \brief Function checking and adding wxSmith support for current project */
        bool CheckIntegration();

        /** \brief Singleton object */
        static wxSmith* Singleton;

        friend class wxSmithMime;
		DECLARE_EVENT_TABLE()
};

CB_DECLARE_PLUGIN();

#endif

