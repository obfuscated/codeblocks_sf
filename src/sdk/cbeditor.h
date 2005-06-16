#ifndef EDITOR_H
#define EDITOR_H

#define EDITOR_MODIFIED             "*"

#define BOOKMARK_MARKER					0
#define BOOKMARK_STYLE 					wxSTC_MARK_ARROW
#define BREAKPOINT_MARKER				1
#define BREAKPOINT_STYLE 				wxSTC_MARK_CIRCLE
#define BREAKPOINT_LINE					2
#define ACTIVE_LINE						3
#define ERROR_LINE						4

#include <wx/stc/stc.h>
#include <wx/hashmap.h>
#include <wx/datetime.h>

#include "settings.h"
#include "editorbase.h"
#include "printing_types.h"

// forward decls
struct cbEditorInternalData; // this is the private data struct used by the editor.
class cbEditor;
class cbStyledTextCtrl;
class ProjectFile;
class EditorColorSet;
class wxNotebook;

class cbStyledTextCtrl : public wxStyledTextCtrl
{
	public:
		cbStyledTextCtrl(cbEditor* pParent, int id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
		virtual ~cbStyledTextCtrl();
	protected:
		void OnContextMenu(wxContextMenuEvent& event);
	private:
		cbEditor* m_pParent;
		DECLARE_EVENT_TABLE()
};

/** @brief A file editor
  *
  * This class represents one editor in Code::Blocks. It holds all the necessary
  * information about an editor. When you want to access a Code::Blocks editor,
  * this is the class you want to get at ;)\n
  * To do this, use Manager::Get()->GetEditorManager() functions.
  * @note This class descends from wxPanel, so it provides all wxPanel methods
  * as well...
  */
class DLLIMPORT cbEditor : public EditorBase
{
        DECLARE_EVENT_TABLE()
    	friend class EditorManager;
	public:
		/** cbEditor constructor.
		  * @param parent the parent notebook - you should use EditorManager::Get()
		  * @param filename the filename to open. If filename is empty, it creates a
		  * new, empty, editor.
		  * @param theme the initial color set to use\n
		  * <em>Note: you should not create a cbEditor object directly. Instead
		  * use EditorManager's methods to do it...</em>
		  */
		cbEditor(wxWindow* parent, const wxString& filename, EditorColorSet* theme = 0L);
		/** cbEditor destructor. */
		~cbEditor();

		// properties
		
		/** Returns a pointer to the underlying cbStyledTextCtrl object (which
		  * itself is the wxWindows implementation of Scintilla). If you want
		  * to mess with the actual contents of an editor, this is the object
		  * you want to get.
		  */
        cbStyledTextCtrl* GetControl(){ return m_pControl; }
		/** Returns true if editor is OK, i.e. constructor was called with a filename
		  * parameter and file was opened succesfully. If it returns false, you
		  * should delete the editor...
		  */
		bool IsOK(){ return m_IsOK; }
		/** Sets the editor title. For tabbed interface, it sets the corresponding
		  * tab text, while for MDI interface it sets the MDI window title...
		  */
		void SetEditorTitle(const wxString& title);
		/** Returns true if editor is modified, false otherwise */
		bool GetModified();
		/** Set the editor's modification state to \c modified. */
		void SetModified(bool modified = true);
		/** Set the ProjectFile pointer associated with this editor. All editors
		  * which belong to a project file, should have this set. All others should return NULL.
		  * Optionally you can preserve the "modified" flag of the file.
		  */
		void SetProjectFile(ProjectFile* project_file,bool preserve_modified = false);
		/** Read the ProjectFile pointer associated with this editor. All editors
		  * which belong to a project file, have this set. All others return NULL.
		  */
		ProjectFile* GetProjectFile(){ return m_pProjectFile; }
		/** Updates the associated ProjectFile object with the editor's caret
		  * position, top visible line and its open state. Used in devProject
		  * layout information, so that each time the user opens a project
		  * file in the IDE, it opens exactly in the same state it was when last
		  * closed.
		  */
		void UpdateProjectFile();
		/** Save editor contents. Returns true on success, false otherwise. */
		bool Save();
		/** Save editor contents under a different filename. Returns true on success, false otherwise. */
		bool SaveAs();
		/** Unimplemented */
		bool RenameTo(const wxString& filename, bool deleteOldFromDisk = false);
		/** Fold all editor folds (hides blocks of code). */
		void FoldAll();
		/** Unfold all editor folds (shows blocks of code). */
		void UnfoldAll();
		/** Toggle all editor folds (inverts the show/hide state of blocks of code). */
		void ToggleAllFolds();
		/** Folds the block containing \c line. If \c line is -1, folds the block containing the caret. */
		void FoldBlockFromLine(int line = -1);
		/** Unfolds the block containing \c line. If \c line is -1, unfolds the block containing the caret. */
		void UnfoldBlockFromLine(int line = -1);
		/** Toggles folding of the block containing \c line. If \c line is -1, toggles folding of the block containing the caret. */
		void ToggleFoldBlockFromLine(int line = -1);
		/** Each line in the editor might have one or more markers associated with it.
		  * Think of it as flags for each line. A specific marker can be set for a line,
		  * or not set. Markers used in cbEditor are:
		  * \li breakpoints: set when a line has a breakpoint
		  * \li bookmarks: set when a line has a bookmark\n
		  * More markers may be used in the future...\n
		  * This method, sets a marker on a line.
		  * If \c line is -1, sets a marker on the line containing the caret.\n
		  * Predefined values for \c marker are BREAKPOINT_MARKER and BOOKMARK_MARKER...
		  */
		void MarkLine(int marker, int line = -1);
		/** Toggles \c marker on \c line.
		  * If \c line is -1, toggles \c marker on the line containing the caret.
		  * @see MarkLine() for information on markers.
		  */
		void MarkerToggle(int marker, int line = -1);
		/** Moves the caret to the next line containing marker of type \c marker.
		  * @see MarkLine() for information on markers.
		  */
		void MarkerNext(int marker);
		/** Moves the caret to the previous line containing marker of type \c marker.
		  * @see MarkLine() for information on markers.
		  */
		void MarkerPrevious(int marker);
		/** Sets the breakpoint markers of the editor, by asking the associated ProjectFile (which reads them from the project layout). */
		void SetBreakpoints();
		/** Set the color set to use. */
		void SetColorSet(EditorColorSet* theme);
		/** Highlights the brace pair (one of the braces must be under the cursor) */
		void HighlightBraces();
        /** Returns the specified line's (0-based) indentation (whitespace) in spaces. If line is -1, it uses the current line */
        int GetLineIndentInSpaces(int line = -1);
        /** Returns the specified line's (0-based) indentation (whitespace) string. If line is -1, it uses the current line */
        wxString GetLineIndentString(int line = -1);
        /** Returns the last modification time for the file. Used to detect modifications outside the editor. */
        wxDateTime GetLastModificationTime(){ return m_LastModified; }
        /** Reloads the file from disk. @return True on success, False on failure. */
        bool Reload();
        /** Print the file.
          * @param selectionOnly Should the selected text be printed only?
          * @param pcm The color mode to use when printing
          */
        void Print(bool selectionOnly, PrintColorMode pcm);
        /** Try to auto-complete the current word.
          *
          * This has nothing to do with code-completion plugins. Editor auto-completion
          * is a feature that saves typing common blocks of code, e.g.
          *
          * If you have typed "forb" (no quotes) and select auto-complete, then
          * it will convert "forb" to "for ( ; ; ){ }".
          * If the word up to the caret position is an unknown keyword, nothing happens.
          *
          * These keywords/code pairs can be edited in the editor configuration
          * dialog.
          */
		void AutoComplete();

		// Workaround for shift-tab bug in wx2.4.2
		void DoIndent(); /// Indents current line/block
		void DoUnIndent(); /// UnIndents current line/block

        // misc. functions
        virtual wxMenu* CreateContextSubMenu(long id);
        virtual void AddToContextMenu(wxMenu* popup,bool noeditor,bool pluginsdone);

    private:
        // functions
		void DoFoldAll(int fold); // 0=unfold, 1=fold, 2=toggle
		void DoFoldBlockFromLine(int line, int fold); // 0=unfold, 1=fold, 2=toggle
		bool DoFoldLine(int line, int fold); // 0=unfold, 1=fold, 2=toggle
        void CreateEditor();
        void SetEditorStyle();
        bool Open();
        void DoAskForCodeCompletion(); // relevant to code-completion plugins
		bool LineHasMarker(int marker, int line = -1);
		wxColour GetOptionColour(const wxString& option, const wxColour _default);
		void NotifyPlugins(wxEventType type, int intArg = 0, const wxString& strArg = wxEmptyString, int xArg = 0, int yArg = 0);
        
        // events
        void OnMarginClick(wxStyledTextEvent& event);
        void OnEditorUpdateUI(wxStyledTextEvent& event);
        void OnEditorChange(wxStyledTextEvent& event);
        void OnEditorCharAdded(wxStyledTextEvent& event);
		void OnEditorDwellStart(wxStyledTextEvent& event);
		void OnEditorDwellEnd(wxStyledTextEvent& event);
		void OnUserListSelection(wxStyledTextEvent& event);
        void OnTimer(wxTimerEvent& event);
		void OnClose(wxCloseEvent& event);

		// one event handler for all popup menu entries
		void OnContextMenuEntry(wxCommandEvent& event);
		
        // variables
        bool m_IsOK;
        cbStyledTextCtrl* m_pControl;
        int m_ID;
		bool m_Modified;
		int m_Index;
        wxTimer m_timerWait;
		ProjectFile* m_pProjectFile;
		EditorColorSet* m_pTheme;
		short int m_ActiveCalltipsNest;
        wxDateTime m_LastModified; // to check if the file was modified outside the editor

        // DO NOT ADD ANY MORE VARIABLES HERE!
        // ADD THEM IN cbEditorInternalData INSTEAD!

        friend struct cbEditorInternalData; // allow cbEditorInternalData to access cbEditor
        cbEditorInternalData* m_pData;
};

#endif // EDITOR_H
