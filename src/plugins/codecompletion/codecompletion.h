/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef CODECOMPLETION_H
#define CODECOMPLETION_H

#include <settings.h> // SDK
#include <cbplugin.h>
#include <cbproject.h>
#include <sdk_events.h>

#include "coderefactoring.h"
#include "nativeparser.h"
#include "systemheadersthread.h"
#include "doxygen_parser.h"

#include <wx/arrstr.h>
#include <wx/listctrl.h>
#include <wx/string.h>
#include <wx/timer.h>

#include <map>
#include <vector>
#include <set>

class cbEditor;
class wxScintillaEvent;
class wxChoice;
class DocumentationHelper;

/** Code completion plugin has those features:
 * show function call-tip when the mouse hover over the variables/functions.
 * automatically auto-completion lists prompted while entering code.
 * navigate the source files, jump between declarations and implementations.
 * find symbol usage, or even rename a symbol(code re-factoring).
 *
 *  We later use "CC" as an abbreviation of Code Completion plugin.
 * See the general architecture of code completion plugin on wiki page
 *  http://wiki.codeblocks.org/index.php?title=Code_Completion_Design
 */
class CodeCompletion : public cbCodeCompletionPlugin
{
public:
    /** the underline data structure of the second wxChoice of CC's toolbar*/
    struct FunctionScope
    {
        FunctionScope() {}
        FunctionScope(const NameSpace& ns): StartLine(ns.StartLine), EndLine(ns.EndLine), Scope(ns.Name) {}

        int StartLine;      // function body (implementation) start line
        int EndLine;        // function body (implementation) end line
        wxString ShortName; // function's base name (without scope prefix)
        wxString Name;      // function's long name (including arguments and return type)
        wxString Scope;     // class or namespace
    };

    /** each file has one such vector instance, containing all the function information in the file */
    typedef std::vector<FunctionScope> FunctionsScopeVec;
    /** helper class to support FunctionsScopeVec*/
    typedef std::vector<int> ScopeMarksVec;

    /** class to store token name and it's ID */
    typedef std::vector<std::pair<wxString,int> > AutocompNameIdxVec;

    struct FunctionsScopePerFile
    {
        FunctionsScopeVec m_FunctionsScope; // all functions in the file
        NameSpaceVec m_NameSpaces;          // all namespaces in the file
        bool parsed;                        // indicates whether this file is parsed or not
    };
    /** filename -> FunctionsScopePerFile map, contains all the opened files scope info*/
    typedef std::map<wxString, FunctionsScopePerFile> FunctionsScopeMap;

    /** Constructor */
    CodeCompletion();
    /** Destructor */
    virtual ~CodeCompletion();

    // the function below were virtual functions from the base class
    virtual void OnAttach();
    virtual void OnRelease(bool appShutDown);
    virtual int GetConfigurationGroup() const { return cgEditor; }
    virtual cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);
    virtual cbConfigurationPanel* GetProjectConfigurationPanel(wxWindow* parent, cbProject* project);
    /** build menus in the main frame */
    virtual void BuildMenu(wxMenuBar* menuBar);
    /** build context popup menu */
    virtual void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0);
    /** build CC Toolbar */
    virtual bool BuildToolBar(wxToolBar* toolBar);
    /** toolbar priority value */
    virtual int GetToolBarPriority() { return 10; }

    // TODO unused, should be removed probably
    virtual wxArrayString GetCallTips() { return wxArrayString(); }
    virtual int CodeComplete();
    virtual void ShowCallTip();
    virtual bool IsProviderFor(cbEditor* ed);

    virtual wxStringVec GetToolTips(int pos, int style, cbEditor* ed);

    /** show auto-completion list for preprocessor directives*/
    void CodeCompletePreprocessor();
    /** show auto-completion after #include */
    void CodeCompleteIncludes();

    /** get the include paths setting (usually set by user for each C::B project)
     * @param project project info
     * @param buildTargets target info
     * @return the local include paths
     */
    wxArrayString GetLocalIncludeDirs(cbProject* project, const wxArrayString& buildTargets);

    /** the default compiler's search paths
     * @param force if the value is false, just return a static wxArrayString to optimize the performance
     */
    wxArrayString& GetSystemIncludeDirs(cbProject* project, bool force);

    /** search target file names (mostly relative names) under basePath, then return the absolute dirs
     * It just did the caculation below:
     * "c:/ccc/ddd.cpp"(basePath) + "aaa/bbb.h"(target) => "c:/ccc/aaa/bbb.h"(dirs)
     * @param basePath already located file path, this is usually the currently parsing file's location
     * @param targets the relative filename, e.g. When you have #include "aaa/bbb.h", "aaa/bbb.h" is the target location
     * @param dirs result location of the targets in absolute file path format
     */
    void GetAbsolutePath(const wxString& basePath, const wxArrayString& targets, wxArrayString& dirs);

    /** handle all the editor event*/
    void EditorEventHook(cbEditor* editor, wxScintillaEvent& event);

    /** read CC's options, mostly happens the user change some setting and press APPLY*/
    void RereadOptions(); // called by the configuration panel

private:
    /** update CC's ToolBar*/
    void UpdateToolBar();

    /** load the token replacement map (macro definitions) from configuration file*/
    void LoadTokenReplacements();
    /** write the Token replacement map to the configure file */
    void SaveTokenReplacements();

    /** event handler for updating UI e.g. menu statues*/
    void OnUpdateUI(wxUpdateUIEvent& event);

    /** event handler when user click Menu->View->Symbols browser */
    void OnViewClassBrowser(wxCommandEvent& event);

    /** event handler to list the suggestion, when a user press CTRL+space(by default)*/
    void OnCodeComplete(wxCommandEvent& event);

    /** event handler to show the call tip, when user press Ctrl-Shift-Space */
    void OnShowCallTip(wxCommandEvent& event);

    /** event handler when user click Menu->Search->Goto function*/
    void OnGotoFunction(wxCommandEvent& event);

    /** navigate to the previous function body*/
    void OnGotoPrevFunction(wxCommandEvent& event);

    /** navigate to the next function body*/
    void OnGotoNextFunction(wxCommandEvent& event);

    /** handle CC's context menu->insert */
    void OnClassMethod(wxCommandEvent& event);

    /** handle CC's context menu->insert */
    void OnUnimplementedClassMethods(wxCommandEvent& event);

    /** handle both goto declaration and implementation event */
    void OnGotoDeclaration(wxCommandEvent& event);

    /** CC's re-factoring function, find all the reference place*/
    void OnFindReferences(wxCommandEvent& event);

    /** CC's re-factoring function, rename a symbol */
    void OnRenameSymbols(wxCommandEvent& event);

    /** open the include file under the caret position */
    void OnOpenIncludeFile(wxCommandEvent& event);

    /** event handler when user select context menu->reparse file/projects */
    void OnCurrentProjectReparse(wxCommandEvent& event);
    void OnSelectedProjectReparse(wxCommandEvent& event);
    void OnSelectedFileReparse(wxCommandEvent& event);

    // event handlers for the standard events sent from sdk core
    /** SDK event when application has started up */
    void OnAppDoneStartup(CodeBlocksEvent& event);
    /** SDK workspace related events */
    void OnWorkspaceChanged(CodeBlocksEvent& event);
    /** SDK project related events */
    void OnProjectActivated(CodeBlocksEvent& event);
    void OnProjectClosed(CodeBlocksEvent& event);
    void OnProjectSaved(CodeBlocksEvent& event);
    void OnProjectFileAdded(CodeBlocksEvent& event);
    void OnProjectFileRemoved(CodeBlocksEvent& event);
    void OnProjectFileChanged(CodeBlocksEvent& event);
    /** SDK editor related events */
    void OnEditorSaveOrModified(CodeBlocksEvent& event);
    void OnEditorOpen(CodeBlocksEvent& event);
    void OnEditorActivated(CodeBlocksEvent& event);
    void OnEditorClosed(CodeBlocksEvent& event);

    /** CC's own logger, to handle event sent from other thread or itself*/
    void OnCCLogger(CodeBlocksThreadEvent& event);
    /** CC's own debug logger, to handle event sent from other thread or itself*/
    void OnCCDebugLogger(CodeBlocksThreadEvent& event);

    /** batch parsing start event*/
    void OnParserStart(wxCommandEvent& event);
    /** batch parsing end event*/
    void OnParserEnd(wxCommandEvent& event);

    /** receive event from SystemHeadersThread */
    void OnSystemHeadersThreadUpdate(CodeBlocksThreadEvent& event);
    void OnSystemHeadersThreadFinish(CodeBlocksThreadEvent& event);
    void OnSystemHeadersThreadError(CodeBlocksThreadEvent& event);

    /** show code suggestion list*/
    void DoCodeComplete();
    void DoShowCallTip(int caretPos = wxNOT_FOUND);

    /** ContextMenu->Insert-> declaration/implementation*/
    int DoClassMethodDeclImpl();
    /** ContextMenu->Insert-> All class methods*/
    int DoAllMethodsImpl();

    void MatchCodeStyle(wxString& str, int eolStyle = wxSCI_EOL_LF, const wxString& indent = wxEmptyString, bool useTabs = false, int tabSize = 4);

    // CC's toolbar related functions
    /** helper method in finding the function position in the vector for the function containing the current line*/
    void FunctionPosition(int &scopeItem, int &functionItem) const;
    /** navigate between function bodies*/
    void GotoFunctionPrevNext(bool next = false);
    /** help method in finding the namespace position in the vector for the namespace containing the current line*/
    int NameSpacePosition() const;

    /** Toolbar select event */
    void OnScope(wxCommandEvent& event);
    /** Toolbar select event */
    void OnFunction(wxCommandEvent& event);

    void ParseFunctionsAndFillToolbar();
    void FindFunctionAndUpdate(int currentLine);
    void UpdateFunctions(unsigned int scopeItem);
    void EnableToolbarTools(bool enable = true);
    void DoParseOpenedProjectAndActiveEditor();

    /** Returns token index of autocomplete item; might be -1
     *  @param itemIndex index to m_AutocompNameIdx; when itemIndex is < 0 then AutoCompGetCurrent will be used
     */
    int GetAutocompTokenIdx(int itemIndex = -1);

    /** highlight member variables */
    void UpdateEditorSyntax(cbEditor* ed = NULL);

    /** delayed for code completion */
    void OnCodeCompleteTimer(wxTimerEvent& event);

    /** delayed for toolbar update */
    void OnToolbarTimer(wxTimerEvent& event);

    /** event fired from the edit event hook function to indicate parsing while editing*/
    void OnRealtimeParsingTimer(wxTimerEvent& event);

    /** delayed running after saving project, while many projects' saving */
    void OnProjectSavedTimer(wxTimerEvent& event);

    /** delayed for re-parsing */
    void OnReparsingTimer(wxTimerEvent& event);

    /** delayed running of editor activated event, only the last activated editor should be considered*/
    void OnEditorActivatedTimer(wxTimerEvent& event);

    /** */
    void OnAutocompleteSelect(wxListEvent& event);

    /** delayed show of documentation to prevent scroll stuttering */
    void OnAutocompSelectTimer(wxTimerEvent& event);

    /** Not used*/
    int                     m_PageIndex;
    /** Indicates CC's initialization is done*/
    bool                    m_InitDone;

    /** menu pointers to the frame's main menu*/
    wxMenu*                 m_EditMenu;
    wxMenu*                 m_SearchMenu;
    wxMenu*                 m_ViewMenu;
    wxMenu*                 m_ProjectMenu;

    /** this member will actually manage all the Parser instances*/
    NativeParser            m_NativeParser;
    /** code re-factoring tool*/
    CodeRefactoring         m_CodeRefactoring;

    int                     m_EditorHookId;
    int                     m_LastPosForCodeCompletion;

    /** delay for showing the suggesting list*/
    wxTimer                 m_TimerCodeCompletion;
    /** timer triggered by editor hook function to delay the real-time parse*/
    wxTimer                 m_TimerRealtimeParsing;
    /** timer for toolbar*/
    wxTimer                 m_TimerToolbar;
    /** delay after project saved event*/
    wxTimer                 m_TimerProjectSaved;
    /** delay after receive a project save/modified event*/
    wxTimer                 m_TimerReparsing;
    /** delay after receive editor activated event*/
    wxTimer                 m_TimerEditorActivated;

    wxTimer                 m_TimerAutocompSelect;
    wxListEvent*            m_LastSelectEvent;

    cbEditor*               m_LastEditor;
    int                     m_ActiveCalltipsNest;

    bool                    m_IsAutoPopup;
    /** this indicates only show pre-processor directives in the auto-completion list */
    bool                    m_CompletePPOnly;

    // The variables below were related to CC's toolbar
    /** the CC's toolbar */
    wxToolBar*              m_ToolBar;
    /** function choice control of CC's toolbar, it is the second choice */
    wxChoice*               m_Function;
    /** namespace/scope choice control, it is the first choice control*/
    wxChoice*               m_Scope;
    /** current active file's function body info*/
    FunctionsScopeVec       m_FunctionsScope;
    /** current active file's namespace/scope info */
    NameSpaceVec            m_NameSpaces;
    /** current active file's line info, helper member to access function scopes*/
    ScopeMarksVec           m_ScopeMarks;
    /** this is a "filename->info" map containing all the opening files choice info*/
    FunctionsScopeMap       m_AllFunctionsScopes;
    /** indicate whether the CC's toolbar need a refresh */
    bool                    m_ToolbarNeedRefresh;
    /** force update toolbar */
    bool                    m_ToolbarNeedReparse;

    /** current caret line */
    int                     m_CurrentLine;

    /** the file updating the toolbar info*/
    wxString                m_LastFile;

    /** indicate whether the predefined keywords set should be added in the suggestion list*/
    bool                    m_LexerKeywordsToInclude[9];

    /** indicate the editor has modified by the user and a real-time parse should be start*/
    bool                    m_NeedReparse;

    /** remember the number of bytes in the current editor/document */
    int                     m_CurrentLength;

    /** batch run UpdateEditorSyntax() after first parsing */
    bool                    m_NeedsBatchColour;

    /** header file names used for auto-completion after #include*/
    SystemHeadersMap        m_SystemHeadersMap;

    //options on code completion (auto suggestion list) feature
    /** disable the code-completion while editing*/
    bool                    m_UseCodeCompletion;
    /** how many characters will the user entered to trigger the code-completion*/
    int                     m_CCAutoLaunchChars;
    /** will the code-completion list pop-up automatically or not*/
    bool                    m_CCAutoLaunch;
    /** how long will the code-completion list delayed(in microseconds) after you hit the key, if
        the value is zero, then the list will show immediately
     */
    int                     m_CCLaunchDelay;
    /** maximum allowed code-completion list entries*/
    size_t                  m_CCMaxMatches;
    /** whether add parentheses after user select a function name in the code-completion suggestion list*/
    bool                    m_CCAutoAddParentheses;
    bool                    m_CCDetectImplementation;
    /** defines characters that work like Tab (empty by Default) but are also inserted*/
    wxString                m_CCFillupChars;
    /** Should a single item auto-completion list automatically choose the item */
    bool                    m_CCAutoSelectOne;
    /** give code completion list for header files, it happens after the #include directive */
    bool                    m_CCEnableHeaders;

    /** thread to collect header file names */
    std::list<SystemHeadersThread*> m_SystemHeadersThreads;
    wxCriticalSection               m_SystemHeadersThreadCS;

    /** map to record all re-parsing files */
    typedef std::map<cbProject*, wxArrayString> ReparsingMap;
    ReparsingMap m_ReparsingMap;

    /** names and token's ids of autocomplete items; filled up in CodeComplete */
    AutocompNameIdxVec      m_AutocompNameIdx;

    /** which position in autocomplete list was selected? */
    int                     m_LastAutocompIndex;

    /** Popup window to display documentation */
    DocumentationHelper     m_DocHelper;

    friend void DocumentationHelper::OnLink(wxHtmlLinkEvent&);
    friend void DocumentationHelper::OnSelectionChange(wxListEvent&);

    DECLARE_EVENT_TABLE()
};

#endif // CODECOMPLETION_H
