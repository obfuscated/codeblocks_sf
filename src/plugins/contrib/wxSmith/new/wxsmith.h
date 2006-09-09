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
		static wxSmith* Get() { return m_Singleton; }

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

        /** \brief Getting wxsProject addition for given cbPrject */
        wxsProject* GetSmithProject(cbProject* Proj);

        /** \brief Getting cbProject class from wxsProject addition */
        cbProject* GetCBProject(wxsProject* Proj);

        /** \brief Helper operator used to convert C::B projects to wxS projects */
        inline wxsProject* operator[](cbProject* Proj) { return GetSmithProject(Proj); }

        /** \brief Helper operator used to convert wxS projects to C::B projects */
        inline cbProject* operator[](wxsProject* Proj) { return GetCBProject(Proj); }

	private:

        WX_DECLARE_HASH_MAP(cbProject*,wxsProject*,wxPointerHash,wxPointerEqual,ProjectMapT);
        typedef ProjectMapT::iterator ProjectMapI;

        ProjectMapT m_ProjectMap;               ///< \brief Map binding all cbProject classes with wxsProject ones
        wxsStoringSplitterWindow* m_Splitter;   ///< \brief Splitter window used to divide resource browser and property grid
        int m_HookId;                           ///< \brief Project hook identifier used when deleting hook
        static wxSmith* m_Singleton;            ///< \brief Singleton object

        /** \brief Procedure called when loading/saving project, used to load/save additional configuration from/to .cbp file */
        void OnProjectHook(cbProject*,TiXmlElement*,bool);

        /** \brief Procedure called when closing project, removes additional stuff associated with project */
        void OnProjectClose(CodeBlocksEvent& event);

        /** \brief Called when clicked "Configure..." from wxSmith menu */
        void OnConfigure(wxCommandEvent& event);

        friend class wxSmithMime;
		DECLARE_EVENT_TABLE()
};

/** \brief Helper function to easily access wxSmith plugin */
inline wxSmith* wxsPlugin() { return wxSmith::Get(); }

/** \brief Helper function to access project converter (wxSmith project <-> C::B project) */
inline wxSmith* wxsProjectConv() { return wxSmith::Get(); }

CB_DECLARE_PLUGIN();

#endif

