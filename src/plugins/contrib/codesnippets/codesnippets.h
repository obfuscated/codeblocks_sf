/*
	This file is part of Code Snippets, a plugin for Code::Blocks
	Copyright (C) 2006 Arto Jonsson

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef CODESNIPPETS_H_INCLUDED
#define CODESNIPPETS_H_INCLUDED

#include <wx/dnd.h>

#include "cbplugin.h" // for "class cbPlugin"
//#include "wxaui/manager.h"
#include "snippetsconfig.h"


class CodeSnippetsWindow;

// ----------------------------------------------------------------------------
class CodeSnippets : public cbPlugin
// ----------------------------------------------------------------------------
{
    friend class wxMyFileDropTarget;
    friend class DropTargets;

	public:
		/** Constructor. */
		CodeSnippets();
		/** Destructor. */
		~CodeSnippets();

		/** Invoke configuration dialog. */
		int Configure() { return 0; }

		/** Return the plugin's configuration priority.
		 * This is a number (default is 50) that is used to sort plugins
		 * in configuration dialogs. Lower numbers mean the plugin's
		 * configuration is put higher in the list.
		*/
		int GetConfigurationPriority() const { return 50; }

		/** Return the configuration group for this plugin. Default is cgUnknown.
		 * Notice that you can logically AND more than one configuration groups,
		 * so you could set it, for example, as "cgCompiler | cgContribPlugin".
		*/
		int GetConfigurationGroup() const { return cgContribPlugin; }

		/** Return plugin's configuration panel.
		  * @param parent The parent window.
		  * @return A pointer to the plugin's cbConfigurationPanel. It is deleted by the caller.
		  */
		cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent){ return 0; }

		/** Return plugin's configuration panel for projects.
		 * The panel returned from this function will be added in the project's
		 * configuration dialog.
		 * @param parent The parent window.
		 * @param project The project that is being edited.
		 * @return A pointer to the plugin's cbConfigurationPanel. It is deleted by the caller.
		*/
		cbConfigurationPanel* GetProjectConfigurationPanel(wxWindow* parent, cbProject* project){ return 0; }

		/** This method is called by Code::Blocks and is used by the plugin
		 * to add any menu items it needs on Code::Blocks's menu bar.\n
		 * It is a pure virtual method that needs to be implemented by all
		 * plugins. If the plugin does not need to add items on the menu,
		 * just do nothing ;)
		 * @param menuBar the wxMenuBar to create items in
		*/
		void BuildMenu(wxMenuBar* menuBar);

		/** This method is called by Code::Blocks core modules (EditorManager,
		 * ProjectManager etc) and is used by the plugin to add any menu
		 * items it needs in the module's popup menu. For example, when
		 * the user right-clicks on a project file in the project tree,
		 * ProjectManager prepares a popup menu to display with context
		 * sensitive options for that file. Before it displays this popup
		 * menu, it asks all attached plugins (by asking PluginManager to call
		 * this method), if they need to add any entries
		 * in that menu. This method is called.\n
		 * If the plugin does not need to add items in the menu,
		 * just do nothing ;)
		 * @param type the module that's preparing a popup menu
		 * @param menu pointer to the popup menu
		 * @param data pointer to FileTreeData object (to access/modify the file tree)
		*/
		void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0){}

		/** This method is called by Code::Blocks and is used by the plugin
		 * to add any toolbar items it needs on Code::Blocks's toolbar.\n
		 * It is a pure virtual method that needs to be implemented by all
		 * plugins. If the plugin does not need to add items on the toolbar,
		 * just do nothing ;)
		 * @param toolBar the wxToolBar to create items on
		 * @return The plugin should return true if it needed the toolbar, false if not
		*/
		bool BuildToolBar(wxToolBar* toolBar){ return false; }
	protected:
		/** Any descendent plugin should override this virtual method and
		 * perform any necessary initialization. This method is called by
		 * Code::Blocks (PluginManager actually) when the plugin has been
		 * loaded and should attach in Code::Blocks. When Code::Blocks
		 * starts up, it finds and <em>loads</em> all plugins but <em>does
		 * not</em> activate (attaches) them. It then activates all plugins
		 * that the user has selected to be activated on start-up.\n
		 * This means that a plugin might be loaded but <b>not</b> activated...\n
		 * Think of this method as the actual constructor...
		*/
		void OnAttach();

		/** Any descendent plugin should override this virtual method and
		 * perform any necessary de-initialization. This method is called by
		 * Code::Blocks (PluginManager actually) when the plugin has been
		 * loaded, attached and should de-attach from Code::Blocks.\n
		 * Think of this method as the actual destructor...
		 * @param appShutDown If true, the application is shutting down. In this
		 *         case *don't* use Manager::Get()->Get...() functions or the
		 *         behaviour is undefined...
		*/
		void OnRelease(bool appShutDown);
        void OnDisable(bool appShutDown);

		// ---
		void SetSnippetsWindow(CodeSnippetsWindow* p);
		CodeSnippetsWindow*  GetSnippetsWindow(){return GetConfig()->GetSnippetsWindow();}
        //-void OnTreeDragEvent(wxTreeEvent& event); 2011/01/9
        //-void OnPrjTreeDragEvent(wxMouseEvent& event);

	private:

        void CreateSnippetWindow();
        void SetTreeCtrlHandler(wxWindow *p, WXTYPE eventType);
        void RemoveTreeCtrlHandler(wxWindow *p, WXTYPE eventType);
        bool GetTreeSelectionData(const wxTreeCtrl* pTree, const wxTreeItemId itemID, wxString& selString);
        wxArrayString* TextToFilenames(const wxString& string);
        bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& files);
        wxString FindAppPath(const wxString& argv0, const wxString& cwd, const wxString& appVariableName);
        void CloseDockWindow();
        wxWindow* FindOpenFilesListWindow();

		void OnViewSnippets(wxCommandEvent& event);
		void OnUpdateUI(wxUpdateUIEvent& event);
		void OnActivate(wxActivateEvent& event);
        void OnWindowDestroy(wxEvent& event);
		void OnIdle(wxIdleEvent& event);
		void OnSwitchViewLayout(CodeBlocksLayoutEvent& event);
		void OnSwitchedViewLayout(CodeBlocksLayoutEvent& event);
		void OnDockWindowVisability(CodeBlocksDockEvent& event);
        void OnAppStartupDone(CodeBlocksEvent& event);
        void OnAppStartShutdown(CodeBlocksEvent& event);

        void OnPrjTreeMouseMotionEvent(wxMouseEvent& event);
        void OnPrjTreeMouseLeftDownEvent(wxMouseEvent& event);
        void OnPrjTreeMouseLeftUpEvent(wxMouseEvent& event);
        void DoPrjTreeExternalDrag(wxTreeCtrl* pTree);
        void OnPrjTreeMouseLeaveWindowEvent(wxMouseEvent& event);
        void SendMouseLeftUp(const wxWindow* pWin, const int mouseX, const int mouseY);
        void MSW_MouseMove(int x, int y );

        wxString      GetCBConfigFile();
        wxString      GetCBConfigDir();

		wxWindow*       m_pAppWin;
        ProjectManager* m_pProjectMgr;
        wxTreeCtrl*     m_pMgtTreeBeginDrag;
        //-wxPoint         m_TreeMousePosn;
        wxTreeItemId    m_TreeItemId;
        wxString        m_TreeText;
        int             m_nOnActivateBusy;
        wxFile          m_PidTmpFile;

        bool            m_bMouseCtrlKeyDown;
        bool            m_bMouseLeftKeyDown;
        bool            m_bMouseIsDragging;
        bool            m_bDragCursorOn;
        wxCursor*       m_pDragCursor;
        wxCursor        m_oldCursor;
        int             m_MouseDownX, m_MouseDownY;
        int             m_MouseUpX, m_MouseUpY;
        wxTreeItemId    m_prjTreeItemAtKeyUp, m_prjTreeItemAtKeyDown;
   		bool            m_bMouseExitedWindow;
        bool            m_bBeginInternalDrag;

		DECLARE_EVENT_TABLE();

}; //class CodeSnippets
// ----------------------------------------------------------------------------
//  ::MainFrame Drop Target (taken from ../src/main.cpp)
// ----------------------------------------------------------------------------
class wxMyFileDropTarget : public wxFileDropTarget
// ----------------------------------------------------------------------------
{
    // This class declaration must mirror the one used in ::MainFrame
    // We pass our filename array off to MainFrame using this class

  public:
    wxMyFileDropTarget(CodeSnippets* frame):m_frame(frame){}
    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
    {
        if(!m_frame) return false;
        return m_frame->OnDropFiles(x,y,filenames);
    }
  private:
    CodeSnippets* m_frame;
};

// ----------------------------------------------------------------------------
// Local drop targets
// ----------------------------------------------------------------------------
class DropTargets: public wxDropTarget
// ----------------------------------------------------------------------------
{
  public:

    // constructor
    DropTargets (CodeSnippets* pcbDndExtn);

    virtual wxDragResult OnData (wxCoord x, wxCoord y, wxDragResult def);
    bool OnDataText (wxCoord x, wxCoord y, const wxString& data);
    bool OnDataFiles (wxCoord x, wxCoord y, const wxArrayString& filenames);

    wxDragResult OnDragOver (wxCoord x, wxCoord y, wxDragResult def);
    wxDragResult OnEnter (wxCoord x, wxCoord y, wxDragResult def);
    virtual bool OnDrop(wxCoord x, wxCoord y)
    {
        //wxDropTarget::OnDrop
        //virtual bool OnDrop(wxCoord x, wxCoord y)
        //Called when the user drops a data object on the target.
        //Return false to veto the operation.
        #ifdef LOGGING
         LOGIT( wxT("DropTargets:OnDrop") );
        #endif //LOGGING
        return true;
    }

    void OnLeave();

  private:

    CodeSnippets* m_pcbDndExtn;

    wxFileDataObject *m_file;
    wxTextDataObject *m_text;

};
//----------------------------------------------------------------------------
// drop targets composite
// ----------------------------------------------------------------------------
class DropTargetsComposite: public wxDataObjectComposite
// ----------------------------------------------------------------------------
{
  public:
    // constructor
    DropTargetsComposite () { m_dataObjectLast = NULL; };

    bool SetData (const wxDataFormat& format, size_t len, const void *buf)
    {
        m_dataObjectLast = GetObject (format);
        wxCHECK_MSG ( m_dataObjectLast, FALSE, wxT("unsupported format in wxDataObjectComposite"));
        return m_dataObjectLast->SetData (len, buf);
    }

    wxDataObjectSimple *GetLastDataObject() { return m_dataObjectLast; }

  private:
    wxDataObjectSimple *m_dataObjectLast;

};

#endif // CODESNIPPETS_H_INCLUDED
