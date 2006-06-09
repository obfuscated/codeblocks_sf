#ifndef CBEDITOR_H
#define CBEDITOR_H

#include <wx/wxscintilla.h>
#include <wx/hashmap.h>
#include <wx/datetime.h>
#include <wx/fontmap.h>

#include "settings.h"
#include "editorbase.h"
#include "printing_types.h"
#include "editorcolourset.h"

extern const wxString g_EditorModified;

// forward decls
struct cbEditorInternalData; // this is the private data struct used by the editor.
class cbEditor;
class ProjectFile;
class EditorColourSet;

class cbStyledTextCtrl : public wxScintilla
{
	public:
		cbStyledTextCtrl(wxWindow* pParent, int id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
		virtual ~cbStyledTextCtrl();
	protected:
		void OnContextMenu(wxContextMenuEvent& event);
	private:
		wxWindow* m_pParent;
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
		  * @param theme the initial colour set to use\n
		  * <em>Note: you should not create a cbEditor object directly. Instead
		  * use EditorManager's methods to do it...</em>
		  */
		cbEditor(wxWindow* parent, const wxString& filename, EditorColourSet* theme = 0L);
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
		/** Set the colour set to use. */
		void SetColourSet(EditorColourSet* theme);
		/** Get the colour set in use. */
		EditorColourSet* GetColourSet(){ return m_pTheme; }
		/** Jumps to the matching brace (if there is one). */
		void GotoMatchingBrace();
		/** Highlights the brace pair (one of the braces must be under the cursor) */
		void HighlightBraces();
        /** Returns the specified line's (0-based) indentation (whitespace) in spaces. If line is -1, it uses the current line */
        int GetLineIndentInSpaces(int line = -1);
        /** Returns the specified line's (0-based) indentation (whitespace) string. If line is -1, it uses the current line */
        wxString GetLineIndentString(int line = -1);
        /** Returns the last modification time for the file. Used to detect modifications outside the editor. */
        wxDateTime GetLastModificationTime(){ return m_LastModified; }
        /** Sets the last modification time for the file to 'now'. Used to detect modifications outside the editor. */
        void Touch();
        /** Reloads the file from disk. @return True on success, False on failure. */
        bool Reload(bool detectEncoding = true);
        /** Print the file.
          * @param selectionOnly Should the selected text be printed only?
          * @param pcm The colour mode to use when printing
          * @param line_numbers Print the line numbers of file, too.
          */
        void Print(bool selectionOnly, PrintColourMode pcm, bool line_numbers);
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
        /** Move the caret at the specified line.
          * @param line Line to move caret to.
          * @param centreOnScreen If true (default), tries to bring the specified line to the centre of the editor.*/
        void GotoLine(int line, bool centreOnScreen = true);
        /** Add debugger breakpoint at specified line. If @c line is -1, use current line. */
        bool AddBreakpoint(int line = -1, bool notifyDebugger = true);
        /** Remove debugger breakpoint at specified line. If @c line is -1, use current line. */
        bool RemoveBreakpoint(int line = -1, bool notifyDebugger = true);
        /** Toggle debugger breakpoint at specified line. If @c line is -1, use current line. */
        void ToggleBreakpoint(int line = -1, bool notifyDebugger = true);
        /** Does @c line has debugger breakpoint? If @c line is -1, use current line. */
        bool HasBreakpoint(int line);
        /** Go to next debugger breakpoint. */
        void GotoNextBreakpoint();
        /** Go to previous debugger breakpoint. */
        void GotoPreviousBreakpoint();
        /** Toggle bookmark at specified line. If @c line is -1, use current line. */
        void ToggleBookmark(int line = -1);
        /** Does @c line has bookmark? */
        bool HasBookmark(int line);
        /** Go to next bookmark. */
        void GotoNextBookmark();
        /** Go to previous bookmark. */
        void GotoPreviousBookmark();
        /** Highlight the line the debugger will execute next. */
        void SetDebugLine(int line);
        /** Highlight the specified line as error. */
        void SetErrorLine(int line);

        // the following functions, although self-explanatory, are documented
        // in EditorBase.
        void Undo();
        void Redo();
        void Cut();
        void Copy();
        void Paste();
        bool CanUndo();
        bool CanRedo();
        bool HasSelection();
        bool CanPaste();

		// Workaround for shift-tab bug in wx2.4.2
		void DoIndent(); /// Indents current line/block
		void DoUnIndent(); /// UnIndents current line/block

        // misc. functions
        virtual wxMenu* CreateContextSubMenu(long id);
        virtual void AddToContextMenu(wxMenu* popup,ModuleType type,bool pluginsdone);  //pecan 2006/03/22

        HighlightLanguage GetLanguage( ) { return m_lang; }
        void SetLanguage( HighlightLanguage lang = HL_AUTO );

        wxFontEncoding GetEncoding( );
        wxString GetEncodingName( );
        void SetEncoding( wxFontEncoding encoding );

        bool GetUseBom( );
        void SetUseBom( bool bom );
    private:
        // functions
        bool LineHasMarker(int marker, int line = -1);
        void MarkerToggle(int marker, int line = -1);
        void MarkerNext(int marker);
        void MarkerPrevious(int marker);
        void MarkLine(int marker, int line);

		void DoFoldAll(int fold); // 0=unfold, 1=fold, 2=toggle
		void DoFoldBlockFromLine(int line, int fold); // 0=unfold, 1=fold, 2=toggle
		bool DoFoldLine(int line, int fold); // 0=unfold, 1=fold, 2=toggle
        void CreateEditor();
        void SetEditorStyle();
        void SetEditorStyleBeforeFileOpen();
        void SetEditorStyleAfterFileOpen();
        void DetectEncoding();
        bool Open(bool detectEncoding = true);
        void DoAskForCodeCompletion(); // relevant to code-completion plugins
		wxColour GetOptionColour(const wxString& option, const wxColour _default);
		void NotifyPlugins(wxEventType type, int intArg = 0, const wxString& strArg = wxEmptyString, int xArg = 0, int yArg = 0);

        // events
        void OnMarginClick(wxScintillaEvent& event);
        void OnEditorUpdateUI(wxScintillaEvent& event);
        void OnEditorChange(wxScintillaEvent& event);
        void OnEditorCharAdded(wxScintillaEvent& event);
		void OnEditorDwellStart(wxScintillaEvent& event);
		void OnEditorDwellEnd(wxScintillaEvent& event);
		void OnEditorModified(wxScintillaEvent& event);
		void OnUserListSelection(wxScintillaEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnZoom(wxScintillaEvent& event);

		// one event handler for all popup menu entries
		void OnContextMenuEntry(wxCommandEvent& event);
        bool OnBeforeBuildContextMenu(const wxPoint& position, ModuleType type);    //pecan 2006/03/22
        void OnAfterBuildContextMenu(ModuleType type);                              //pecan 2006/03/22

        // variables
        bool m_IsOK;
        cbStyledTextCtrl* m_pControl;
        int m_ID;
		bool m_Modified;
		int m_Index;
        wxTimer m_timerWait;
		ProjectFile* m_pProjectFile;
		EditorColourSet* m_pTheme;
		HighlightLanguage m_lang;
        wxDateTime m_LastModified; // to check if the file was modified outside the editor

        // DO NOT ADD ANY MORE VARIABLES HERE!
        // ADD THEM IN cbEditorInternalData INSTEAD!

        friend struct cbEditorInternalData; // allow cbEditorInternalData to access cbEditor
        cbEditorInternalData* m_pData;
};

#endif // CBEDITOR_H
