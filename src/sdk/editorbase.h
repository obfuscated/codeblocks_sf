#ifndef EDITORBASE_H
#define EDITORBASE_H

#include <wx/hashmap.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include "globals.h"
#include "settings.h"

class wxMenu;
class EditorBase;

WX_DECLARE_HASH_MAP(int, EditorBase*, wxIntegerHash, wxIntegerEqual, SwitchToMap);

/** @brief Base class that all "editors" should inherit from. */
class DLLIMPORT EditorBase : public wxPanel
{
    DECLARE_EVENT_TABLE()
    public:
        EditorBase(wxWindow* parent, const wxString& filename);
        virtual ~EditorBase();

		virtual const wxString& GetFilename(){ return m_Filename; }
		/** Sets the editor's filename */
		virtual void SetFilename(const wxString& x){ m_Filename = x; }

		/** Returns the editor's short name. It is the name displayed on the
		  * editor's tab...
		  */
		virtual const wxString& GetShortName(){ return m_Shortname; }
		/** Returns true if editor is modified, false otherwise */
		virtual bool GetModified() const { return false; }
		/** Set the editor's modification state to \c modified. */
		virtual void SetModified(bool modified = true) {}

		virtual const wxString& GetTitle();
		virtual void SetTitle(const wxString& newTitle);

		virtual void Activate();

		/** @return true if this editor can be closed */
		virtual bool QueryClose(){ return true; }
		/** default implementation, calls Destroy() and returns true
		  *  @return true if editor closed succesfully
		  */
		virtual bool Close();

		/** Save editor contents. Returns true on success, false otherwise. */
		virtual bool Save() { return true; }

        /** deliberately non-virtual */
        virtual bool IsBuiltinEditor();
        /** Are there other editors besides this? */
        virtual bool ThereAreOthers();

		/** Displays the editor's context menu (usually invoked by the user right-clicking in the editor) */
		//virtual void DisplayContextMenu(const wxPoint& position,bool noeditor = false);
		virtual void DisplayContextMenu(const wxPoint& position,ModuleType type = mtUnknown);

        /** Should this kind of editor be visible from the open files tree? */
        virtual bool VisibleToTree() { return true; }

        /** Move the caret at the specified line. */
        virtual void GotoLine(int line, bool centreOnScreen = true){}

        /** Toggle breakpoint at specified line. If @c line is -1, use current line. */
        virtual void ToggleBreakpoint(int line = -1, bool notifyDebugger = true){}
        /** Does @c line has breakpoint? */
        virtual bool HasBreakpoint(int line){ return false; }
        /** Go to next breakpoint. */
        virtual void GotoNextBreakpoint(){}
        /** Go to previous breakpoint. */
        virtual void GotoPreviousBreakpoint(){}

        /** Toggle bookmark at specified line. If @c line is -1, use current line. */
        virtual void ToggleBookmark(int line = -1){}
        /** Does @c line has bookmark? */
        virtual bool HasBookmark(int line){ return false; }
        /** Go to next bookmark. */
        virtual void GotoNextBookmark(){}
        /** Go to previous bookmark. */
        virtual void GotoPreviousBookmark(){}

        /** Highlight the line the debugger will execute next. */
        virtual void SetDebugLine(int line){}

        /** Highlight the specified line as error. */
        virtual void SetErrorLine(int line){}

        /** Undo changes. */
        virtual void Undo(){}

        /** Redo changes. */
        virtual void Redo(){}

        /** Cut selected text/object to clipboard. */
        virtual void Cut(){}

        /** Copy selected text/object to clipboard. */
        virtual void Copy(){}

        /** Paste selected text/object from clipboard. */
        virtual void Paste(){}

        /** @return True if there is something to undo, false if not. */
        virtual bool CanUndo(){ return false; }

        /** @return True if there is something to redo, false if not. */
        virtual bool CanRedo(){ return false; }

        /** @return True if there is text/object selected, false if not. */
        virtual bool HasSelection(){ return false; }

        /** @return True if there is something to paste, false if not. */
        virtual bool CanPaste(){ return false; }
    protected:
        /** Initializes filename data */
        virtual void InitFilename(const wxString& filename);
        /** Creates context submenus. See cbEditor code for details */
        virtual wxMenu* CreateContextSubMenu(int id); // For context menus

        /** Creates context menu items, both before and after creating plugins menu items */
        virtual void AddToContextMenu(wxMenu* popup, ModuleType type,bool pluginsdone) {}   //pecan 2006/03/22
        /** Creates unique filename when asking to save the file */
        virtual wxString CreateUniqueFilename();
        /** Informs the editor we 're just about to create a context menu.
          * Default implementation, just returns true.
          * @param position specifies the position of the popup menu.
          * @param type specifies the "ModuleType" popup menu.
          * @return If the editor returns false, the context menu creation is aborted.
          */
        virtual bool OnBeforeBuildContextMenu(const wxPoint& position, ModuleType type){ return true; } //pecan 2006/03/22
        /** Informs the editor we 're done creating the context menu (just about to display it).
          * Default implementation does nothing.
          * @param type specifies the "ModuleType" context popup menu.
          */
        virtual void OnAfterBuildContextMenu(ModuleType type){}     //pecan 2006/03/22

        bool m_IsBuiltinEditor; // do not mess with it!
		wxString m_Shortname;
		wxString m_Filename;
    private:

        /** one event handler for all popup menu entries */
        void OnContextMenuEntry(wxCommandEvent& event);
        void BasicAddToContextMenu(wxMenu* popup, ModuleType type); //pecan 2006/03/22
        SwitchToMap m_SwitchTo;
        wxString m_WinTitle;
        wxString lastWord;
};

#endif // EDITORBASE_H
