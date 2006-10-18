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

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/splitter.h>
#include <wx/scrolwin.h>

#include <cbplugin.h>
#include <settings.h>
#include <sdk_events.h>

#include <map>

#include "wxsproject.h"
#include "wxsevent.h"
#include "wxssplitterwindow.h"
#include "wxsresourcetree.h"

class wxsProject;
class wxsCoder;
class wxsExtResManager;
class wxsPropertiesMan;
class wxsWidgetFactory;

/** Main plugin which will handle most of wxSmith's work */
class wxSmith : public cbPlugin
{
	public:
		wxSmith();
		~wxSmith();

		static wxSmith* Get() { return Singleton; }

        int GetConfigurationGroup() const { return cgEditor; }
        cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);
		void BuildMenu(wxMenuBar* menuBar);
		void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0);
		bool BuildToolBar(wxToolBar* toolBar);
		void OnAttach(); // fires when the plugin is attached to the application
		void OnRelease(bool appShutDown); // fires when the plugin is released from the application

        /* Function used while selecting resource in resource browser */
        void OnSelectResource(wxsResourceTreeData* Data);

        /* Getting current resourcec tree */
        wxsResourceTree* GetResourceTree() { return ResourceBrowser; }

        wxsProject* GetSmithProject(cbProject* Proj);
        cbProject* GetCBProject(wxsProject* Proj);

	private:
        wxsResourceTree* ResourceBrowser;
        wxScrolledWindow* PropertiesPanel;
        wxScrolledWindow* EventsPanel;
        wxsSplitterWindowEx* LeftSplitter;
        wxsCoder* Coder;
        wxsExtResManager* ExtResManager;
        wxsPropertiesMan* PropertiesManager;
        wxsWidgetFactory* WidgetFactory;

        void OnProjectHook(cbProject*,TiXmlElement*,bool);
        int HookId;

        /* Here's bridge between current C::B project and wxSmith projects */

        typedef std::map<cbProject*,wxsProject*> ProjectMapT;
        typedef ProjectMapT::iterator ProjectMapI;

        ProjectMapT ProjectMap;

        bool AddSmithToProject(wxsProject* Project);

        /* Event processing functions */

        void OnProjectClose(CodeBlocksEvent& event);
        void OnProjectOpen(CodeBlocksEvent& event);
        void OnProjectActivated(CodeBlocksEvent& event);
        void OnNewWindow(wxCommandEvent& event);
        void OnImportXrc(wxCommandEvent& event);
        void OnConfigure(wxCommandEvent& event);

        /* Internal event-processing functions */
        void OnSpreadEvent(wxsEvent& event);
        void OnSelectRes(wxsEvent& event);
        void OnSelectWidget(wxsEvent& event);

        /** Function checking and adding wxSmith support for current project */
        bool CheckIntegration();

        /* Singleton object */
        static wxSmith* Singleton;

        friend class wxSmithMime;

		DECLARE_EVENT_TABLE()
};

#endif // WXSMITH_H

