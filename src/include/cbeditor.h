/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CBEDITOR_H
#define CBEDITOR_H

#include <wx/hashmap.h>
#include <wx/datetime.h>
#include <wx/fontmap.h>
#include <wx/timer.h>

#include "settings.h"
#include "editorbase.h"
#include "printing_types.h"

extern const wxString g_EditorModified;

// forward decls
struct cbEditorInternalData; // this is the private data struct used by the editor.
class cbEditor;
class ProjectFile;
class EditorColourSet;
class wxSplitterWindow;
class LoaderBase;
class cbStyledTextCtrl;
class wxScintillaEvent;
class wxBoxSizer;


/** @brief A file editor
  *
  * This class represents one builtin editor in Code::Blocks. It holds all the necessary
  * information about an editor. When you want to access a Code::Blocks editor,
  * this is the class you want to get at ;)\n
  *
  * To do this, use Manager::Get()->GetEditorManager() functions.
  *
  * The actual editor component used is Scintilla and it can be accessed through
  * the member function GetControl().
  */
class DLLIMPORT cbEditor : public EditorBase
{
        DECLARE_EVENT_TABLE()
        friend class EditorManager;

    protected:
        /** cbEditor constructor.
          * @param parent the parent notebook - you should use EditorManager::Get()
          * @param filename the filename to open. If filename is empty, it creates a
          * new, empty, editor.
          * @param theme the initial colour set to use\n
          * <em>Note: you cannot create a cbEditor object directly. Instead
          * use EditorManager's methods to do it...</em>
          */
        cbEditor(wxWindow* parent, const wxString& filename, EditorColourSet* theme = nullptr);
        cbEditor(wxWindow* parent, LoaderBase* fileLdr, const wxString& filename, EditorColourSet* theme = nullptr);
        /** cbEditor destructor. */
        ~cbEditor();
    public:
        enum SplitType
        {
            stNoSplit = 0,
            stHorizontal,
            stVertical
        };

        /** Don't use this. It throws an exception if you do. */
        void operator=(cb_unused const cbEditor& rhs){ cbThrow(_T("Can't assign an cbEditor* !!!")); }

        // properties

        /** Returns a pointer to the underlying cbStyledTextCtrl object (which
          * itself is the wxWindows implementation of Scintilla). If you want
          * to mess with the actual contents of an editor, this is the object
          * you want to get.
          * @remarks If the editor is split, this function returns the control
          * which currently has the keyboard focus. Don't save this pointer
          * because it might be invalid at any later time...
          */
        cbStyledTextCtrl* GetControl() const;

        /** Returns a pointer to the left (or top) split-view cbStyledTextCtrl.
          * This function always returns a valid pointer.
          */
        cbStyledTextCtrl* GetLeftSplitViewControl() const { return m_pControl; }

        /** Returns a pointer to the right (or bottom) split-view cbStyledTextCtrl.
          * This function may return NULL if the editor is not split.
          */
        cbStyledTextCtrl* GetRightSplitViewControl() const { return m_pControl2; }

        /** Returns the state of split-view for this editor. */
        SplitType GetSplitType() const { return m_SplitType; }

        /** Returns true if editor is OK, i.e. constructor was called with a filename
          * parameter and file was opened successfully. If it returns false, you
          * should delete the editor...
          */
        bool IsOK() const { return m_IsOK; }

        /** Sets the editor title. For tabbed interface, it sets the corresponding
          * tab text, while for MDI interface it sets the MDI window title...
          */
        void SetEditorTitle(const wxString& title);

        /** Returns true if editor is modified, false otherwise */
        bool GetModified() const;

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
        ProjectFile* GetProjectFile() const { return m_pProjectFile; }

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

        /** Save fold states within a new cbStyledTextCtrl. This saves the whole document, thus saving the fold states before the Fold Options Change*/
        bool SaveFoldState();

        /** Fix fold states by comparing foldBackup with m_pControl. This is a temp fix for the Scintilla bug*/
        bool FixFoldState();

        /** Fold all editor folds (hides blocks of code). */
        void FoldAll();

        /** Unfold all editor folds (shows blocks of code). */
        void UnfoldAll();

        /** Toggle all editor folds (inverts the show/hide state of blocks of code). */
        void ToggleAllFolds();

        /** Sets the type of folding indicator where id is one of the following: 0->Arrow, 1->Circle, 2->Square, 3->simple */
        void SetFoldingIndicator(int id);

        /** Folds the block containing \c line. If \c line is -1, folds the block containing the caret. */
        void FoldBlockFromLine(int line = -1);

        /** Unfolds the block containing \c line. If \c line is -1, unfolds the block containing the caret. */
        void UnfoldBlockFromLine(int line = -1);

        /** Toggles folding of the block containing \c line. If \c line is -1, toggles folding of the block containing the caret. */
        void ToggleFoldBlockFromLine(int line = -1);

        /** Set the colour set to use. */
        void SetColourSet(EditorColourSet* theme);

        /** Get the colour set in use. */
        EditorColourSet* GetColourSet() const { return m_pTheme; }

        /** Jumps to the matching brace (if there is one). */
        void GotoMatchingBrace();

        /** Highlights the brace pair (one of the braces must be under the cursor) */
        void HighlightBraces();

        /** Returns the specified line's (0-based) indentation (whitespace) in spaces. If line is -1, it uses the current line */
        int GetLineIndentInSpaces(int line = -1) const;

        /** Returns the specified line's (0-based) indentation (whitespace) string. If line is -1, it uses the current line */
        wxString GetLineIndentString(int line = -1) const;

        /** Returns the last modification time for the file. Used to detect modifications outside the editor. */
        wxDateTime GetLastModificationTime() const { return m_LastModified; }

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

        /** This method is obsolete, use the abbreviations plugin instead. */
        void AutoComplete();

        /** Move the caret at the specified line.
          * @param line Line to move caret to.
          * @param centerOnScreen If true (default), tries to bring the specified line to the centre of the editor.*/
        void GotoLine(int line, bool centerOnScreen = true);

        /** Move the caret at the specified line.
          * @param line      Line to move caret to (where the token is).
          * @param tokenName Token name (string) to highlight, if found
          * @return Editor found, position set and token highlighted? */
        bool GotoTokenPosition(int line, const wxString& tokenName);

        /** Add debugger breakpoint at specified line. If @c line is -1, use current line. */
        bool AddBreakpoint(int line = -1, bool notifyDebugger = true);

        /** Remove debugger breakpoint at specified line. If @c line is -1, use current line. */
        bool RemoveBreakpoint(int line = -1, bool notifyDebugger = true);

        /** Toggle debugger breakpoint at specified line. If @c line is -1, use current line. */
        void ToggleBreakpoint(int line = -1, bool notifyDebugger = true);

        /** Does @c line has debugger breakpoint? If @c line is -1, use current line. */
        bool HasBreakpoint(int line) const;

        /** Go to next debugger breakpoint. */
        void GotoNextBreakpoint();

        /** Go to previous debugger breakpoint. */
        void GotoPreviousBreakpoint();

        /** Refresh all markers for the breakpoints (only the markers for the current debugger will be shown) */
        void RefreshBreakpointMarkers();

        /** Clear all bookmarks. */
        virtual void ClearAllBookmarks();

        /** Toggle bookmark at specified line. If @c line is -1, use current line. */
        virtual void ToggleBookmark(int line = -1);

        /** Does @c line has bookmark? */
        virtual bool HasBookmark(int line) const;

        /** Go to next bookmark. */
        virtual void GotoNextBookmark();

        /** Go to previous bookmark. */
        virtual void GotoPreviousBookmark();

        /** Highlight the line the debugger will execute next. */
        void SetDebugLine(int line);

        /** Highlight the specified line as error. */
        void SetErrorLine(int line);

        /** Split the editor window.
          * @param split The type of split: horizontal or vertical. */
        void Split(SplitType split);

        /** Unsplit the editor window. */
        void Unsplit();

        // the following functions, although self-explanatory, are documented
        // in EditorBase.
        void Undo();
        void Redo();
        void ClearHistory();
        void GotoNextChanged();
        void GotoPreviousChanged();
        void SetChangeCollection(bool collectChange);
        void Cut();
        void Copy();
        void Paste();
        bool CanUndo() const;
        bool CanRedo() const;
        bool HasSelection() const;
        bool CanPaste() const;
        bool IsReadOnly() const;
        void SetReadOnly(bool readonly = true);

        bool CanSelectAll() const;
        void SelectAll();

        // Workaround for shift-tab bug in wx2.4.2
        void DoIndent();   //!< Indents current line/block
        void DoUnIndent(); //!< UnIndents current line/block

        // misc. functions
        virtual wxMenu* CreateContextSubMenu(long id);
        virtual void AddToContextMenu(wxMenu* popup,ModuleType type,bool pluginsdone);  //pecan 2006/03/22

        HighlightLanguage GetLanguage( ) const { return m_lang; }
        void SetLanguage( HighlightLanguage lang = HL_AUTO );

        wxFontEncoding GetEncoding( ) const;
        wxString GetEncodingName( ) const;
        void SetEncoding( wxFontEncoding encoding );

        bool GetUseBom() const;
        void SetUseBom( bool bom );

        void SetZoom(int zoom, bool both = true);

        /// Apply the editor defaults to any (possibly foreign) cbStyledTextCtrl.
        static void ApplyStyles(cbStyledTextCtrl* control);

        void AutoIndentDone();
    private:
        cbEditor(cb_unused const cbEditor& rhs); // prevent copy construction

        // functions
        bool LineHasMarker(int marker, int line = -1) const;
        void MarkerToggle(int marker, int line = -1);
        void MarkerNext(int marker);
        void MarkerPrevious(int marker);
        void MarkLine(int marker, int line);

        void DoFoldAll(int fold); // 0=unfold, 1=fold, 2=toggle
        void DoFoldBlockFromLine(int line, int fold); // 0=unfold, 1=fold, 2=toggle
        bool DoFoldLine(int line, int fold); // 0=unfold, 1=fold, 2=toggle
        void SetMarkerStyle(int marker, int markerType, wxColor fore, wxColor back);
        void UnderlineFoldedLines(bool underline);
        cbStyledTextCtrl* CreateEditor();
        void ConnectEvents(cbStyledTextCtrl* stc);
        void SetEditorStyle();
        void SetEditorStyleBeforeFileOpen();
        void SetEditorStyleAfterFileOpen();
        static void InternalSetEditorStyleBeforeFileOpen(cbStyledTextCtrl* control);
        static void InternalSetEditorStyleAfterFileOpen(cbStyledTextCtrl* control);
        bool Open(bool detectEncoding = true);
        void DoAskForCodeCompletion(); // relevant to code-completion plugins
        static wxColour GetOptionColour(const wxString& option, const wxColour _default);
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
        void OnZoom(wxScintillaEvent& event);
        /** notify all the registered EditorHook functions
         * @param event indicates which event is received by the cbEditor
         * You should bind OnScintillaEvent to every wxScintillaEvent events, either directly or
         * indirectly, see cbEditor::ConnectEvents() for more details.
         */
        void OnScintillaEvent(wxScintillaEvent& event);
        void OnClose(wxCloseEvent& event);

        // one event handler for all popup menu entries
        void OnContextMenuEntry(wxCommandEvent& event);
        bool OnBeforeBuildContextMenu(const wxPoint& position, ModuleType type);    //pecan 2006/03/22
        void OnAfterBuildContextMenu(ModuleType type);                              //pecan 2006/03/22

        void DestroySplitView();

        void DoInitializations(const wxString& filename, LoaderBase* fileLdr = nullptr);

        void BreakpointMarkerToggle(int line);

        // variables
        bool m_IsOK;
        wxSplitterWindow* m_pSplitter;
        wxBoxSizer* m_pSizer;
        cbStyledTextCtrl* m_pControl;
        cbStyledTextCtrl* m_pControl2;
        cbStyledTextCtrl* m_foldBackup;
        SplitType m_SplitType;
        bool m_Modified;
        int m_Index;
        wxTimer m_timerWait;
        ProjectFile* m_pProjectFile;
        EditorColourSet* m_pTheme;
        HighlightLanguage m_lang;
        wxDateTime m_LastModified; // to check if the file was modified outside the editor
        bool m_autoIndentDone;

        // DO NOT ADD ANY MORE VARIABLES HERE!
        // ADD THEM IN cbEditorInternalData INSTEAD!

        friend struct cbEditorInternalData; // allow cbEditorInternalData to access cbEditor
        cbEditorInternalData* m_pData;
};

#endif // CBEDITOR_H
