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

//#if defined(__GNUG__) && !defined(__APPLE__)
//	#pragma interface "wxsmith.h"
//#endif
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

class WXSCLASS wxsProject;

/** Main plugin which will handle most of wxSmith's work */
class WXSCLASS wxSmith : public cbPlugin
{
	public:
		wxSmith();
		~wxSmith();

		static wxSmith* Get() { return Singleton; }

		int Configure();
		void BuildMenu(wxMenuBar* menuBar);
		void BuildModuleMenu(const ModuleType type, wxMenu* menu, const wxString& arg);
		bool BuildToolBar(wxToolBar* toolBar);
		void OnAttach(); // fires when the plugin is attached to the application
		void OnRelease(bool appShutDown); // fires when the plugin is released from the application

        /* Function used while selecting resource in resource browser */
        void OnSelectResource(wxsResourceTreeData* Data);

        /* Getting current resourcec tree */
        wxTreeCtrl* GetResourceTree() { return ResourceBrowser; }

        wxsProject* GetSmithProject(cbProject* Proj);
        cbProject* GetCBProject(wxsProject* Proj);

	private:
        wxTreeCtrl* ResourceBrowser;
        wxScrolledWindow* PropertiesPanel;
        wxScrolledWindow* EventsPanel;
        wxsSplitterWindowEx* LeftSplitter;

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

/** Helper plugin dealing with mime types */
class WXSCLASS wxSmithMime : public cbMimePlugin
{
    public:
    
        wxSmithMime();
    
        /** Returning true if can handle this file
         *
         * This function will handle two file types:
         *  WXS file (will be opened only when project file is also opened)
         *  XRC files (currently not implemented)
         */
        virtual bool CanHandleFile(const wxString& filename) const;
        
        /** Opening file - if this is wxs file and it's project is opened,
         *                 proper editor will be opened / selected
         * for xrc files, new editor withour project will be used */
        virtual int OpenFile(const wxString& filename);
        
        /** We do not handle everything */
        virtual bool HandlesEverything() const  { return false; }
        
        /** And we do not configure anything */
        virtual int Configure() { return -1; }
};

CB_DECLARE_PLUGIN();

#endif // WXSMITH_H

