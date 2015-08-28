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
 * show tool-tip when the mouse hover over the variables/functions.
 * show call-tip when you hit the ( after the function name
 * automatically auto-completion lists prompted while entering code.
 * navigate the source files, jump between declarations and implementations.
 * find symbol usage, or even rename a symbol(code re-factoring).
 *
 * We later use "CC" as an abbreviation of Code Completion plugin.
 * See the general architecture of code completion plugin on wiki page
 *  http://wiki.codeblocks.org/index.php?title=Code_Completion_Design
 */
class CodeCompletion : public cbCodeCompletionPlugin
{
public:
    /** Identify a function body's position, the underline data structure of the second wxChoice of
     * CC's toolbar
     */
    struct FunctionScope
    {
        FunctionScope() {}

        /** a namespace token can be convert to a FunctionScope type */
        FunctionScope(const NameSpace& ns):
            StartLine(ns.StartLine), EndLine(ns.EndLine), Scope(ns.Name) {}

        int StartLine;      ///< function body (implementation) start line
        int EndLine;        ///< function body (implementation) end line
        wxString ShortName; ///< function's base name (without scope prefix)
        wxString Name;      ///< function's long name (including arguments and return type)
        wxString Scope;     ///< class or namespace
    };

    /** vector containing all the function information of a single source file */
    typedef std::vector<FunctionScope> FunctionsScopeVec;
    /** helper class to support FunctionsScopeVec */
    typedef std::vector<int> ScopeMarksVec;


    struct FunctionsScopePerFile
    {
        FunctionsScopeVec m_FunctionsScope; // all functions in the file
        NameSpaceVec m_NameSpaces;          // all namespaces in the file
        bool parsed;                        // indicates whether this file is parsed or not
    };
    /** filename -> FunctionsScopePerFile map, contains all the opened files scope info */
    typedef std::map<wxString, FunctionsScopePerFile> FunctionsScopeMap;

    /** Constructor */
    CodeCompletion();
    /** Destructor */
    virtual ~CodeCompletion();

    // the function below were virtual functions from the base class
    virtual void OnAttach();
    virtual void OnRelease(bool appShutDown);
    virtual int GetConfigurationGroup() const { return cgEditor; }

    /** CC's config dialog */
    virtual cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);
    /** CC's config dialog which show in the project options panel */
    virtual cbConfigurationPanel* GetProjectConfigurationPanel(wxWindow* parent, cbProject* project);
    /** build menus in the main frame */
    virtual void BuildMenu(wxMenuBar* menuBar);
    /** build context popup menu */
    virtual void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0);
    /** build CC Toolbar */
    virtual bool BuildToolBar(wxToolBar* toolBar);
    /** toolbar priority value */
    virtual int GetToolBarPriority() { return 10; }

    // override virtual functions in cbCodeCompletionPlugin class
    virtual CCProviderStatus GetProviderStatusFor(cbEditor* ed);
    virtual std::vector<CCToken> GetAutocompList(bool isAuto, cbEditor* ed, int& tknStart, int& tknEnd);
    virtual std::vector<CCCallTip> GetCallTips(int pos, int style, cbEditor* ed, int& argsPos);
    virtual wxString GetDocumentation(const CCToken& token);
    virtual std::vector<CCToken> GetTokenAt(int pos, cbEditor* ed, bool& allowCallTip);
    virtual wxString OnDocumentationLink(wxHtmlLinkEvent& event, bool& dismissPopup);
    virtual void DoAutocomplete(const CCToken& token, cbEditor* ed);

    /** get the include paths setting (usually set by user for each C::B project)
     * note that this function is only be called in CodeCompletion::DoCodeCompleteIncludes()
     * if it finds some system level include search dirs which does not been scanned, it will start a
     * a new thread(SystemHeadersThread).
     * @param project project info
     * @param buildTargets target info
     * @return the local include paths
     */
    wxArrayString GetLocalIncludeDirs(cbProject* project, const wxArrayString& buildTargets);

    /** get the whole search dirs except the ones locally belong to the c::b project, note this
     * function is used for auto suggestion for #include directives.
     * @param force if the value is false, just return a static (cached) wxArrayString to optimize
     * the performance, it it is true, we try to update the cache.
     */
    wxArrayString& GetSystemIncludeDirs(cbProject* project, bool force);

    /** search target file names (mostly relative names) under basePath, then return the absolute dirs
     * It just did the calculation below:
     * "c:/ccc/ddd.cpp"(basePath) + "aaa/bbb.h"(target) => "c:/ccc/aaa/bbb.h"(dirs)
     * @param basePath already located file path, this is usually the currently parsing file's location
     * @param targets the relative filename, e.g. When you have #include "aaa/bbb.h", "aaa/bbb.h" is the target location
     * @param dirs result location of the targets in absolute file path format
     */
    void GetAbsolutePath(const wxString& basePath, const wxArrayString& targets, wxArrayString& dirs);

    /** handle all the editor event */
    void EditorEventHook(cbEditor* editor, wxScintillaEvent& event);

    /** read CC's options, mostly happens the user change some setting and press APPLY */
    void RereadOptions(); // called by the configuration panel

private:
    /** update CC's ToolBar, the user may disable the first wxChoice, so we need to recreate the
     * wxChoice and measure the best fit size
     */
    void UpdateToolBar();

    /** load the token replacement map (macro definitions) from configuration file*/
    void LoadTokenReplacements();
    /** write the Token replacement map to the configure file */
    void SaveTokenReplacements();

    /** event handler for updating UI e.g. menu statues*/
    void OnUpdateUI(wxUpdateUIEvent& event);

    /** event handler when user click Menu->View->Symbols browser */
    void OnViewClassBrowser(wxCommandEvent& event);

    /** event handler when user click Menu->Search->Goto function */
    void OnGotoFunction(wxCommandEvent& event);

    /** navigate to the previous function body */
    void OnGotoPrevFunction(wxCommandEvent& event);

    /** navigate to the next function body */
    void OnGotoNextFunction(wxCommandEvent& event);

    /** handle CC's context menu->insert "Class method declaration/implementation..." */
    void OnClassMethod(wxCommandEvent& event);

    /** handle CC's context menu->insert "All class methods without implementation..." */
    void OnUnimplementedClassMethods(wxCommandEvent& event);

    /** handle both goto declaration and implementation event */
    void OnGotoDeclaration(wxCommandEvent& event);

    /** CC's re-factoring function, find all the reference place */
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
    void OnEditorSave(CodeBlocksEvent& event);
    void OnEditorOpen(CodeBlocksEvent& event);
    void OnEditorActivated(CodeBlocksEvent& event);
    void OnEditorClosed(CodeBlocksEvent& event);

    /** CC's own logger, to handle log events sent from other worker threads or itself(the main GUI
     * thread), the log messages will be printed in the "Code::Blocks" log panel.
     */
    void OnCCLogger(CodeBlocksThreadEvent& event);
    /** CC's own debug logger, to handle log event sent from other worker threads or itself(the main
     * GUI thread), the log messages will be printed in the "Code::Blocks Debug" log panel.
     */
    void OnCCDebugLogger(CodeBlocksThreadEvent& event);

    /** batch parsing start event
     * this event usually be fired when an Parser object try to start parsing tasks in the thread
     * pool
     */
    void OnParserStart(wxCommandEvent& event);

    /** batch parsing end event
     * this event usually be fired when the task pool becomes empty
     */
    void OnParserEnd(wxCommandEvent& event);

    /** receive event from SystemHeadersThread */
    void OnSystemHeadersThreadUpdate(CodeBlocksThreadEvent& event);
    void OnSystemHeadersThreadFinish(CodeBlocksThreadEvent& event);
    void OnSystemHeadersThreadError(CodeBlocksThreadEvent& event);

    /** fill the tokens with correct code complete words
     * @param caretPos the location of caret
     * @param ed the context editor where we locates
     * @param tokens the stored token results
     * @param preprocessorOnly if true, we only collect macro definition tokens
     */
    void DoCodeComplete(int caretPos, cbEditor* ed, std::vector<CCToken>& tokens, bool preprocessorOnly = false);

    /** fill the tokens with correct preprocessor directives, such as #i will prompt "if", "include"
     * @param tknStart the start of the completed word
     * @param tknEnd current caret location
     * @param ed current active editor
     * @param tokens results storing all the suggesting texts
     */
    void DoCodeCompletePreprocessor(int tknStart, int tknEnd, cbEditor* ed, std::vector<CCToken>& tokens);

    /** fill the tokens with correct include file names
     * @param tknStart the start of the completed word
     * @param tknEnd current caret location
     * @param ed current active editor
     * @param tokens results storing all the suggested include files
     */
    void DoCodeCompleteIncludes(cbEditor* ed, int& tknStart, int tknEnd, std::vector<CCToken>& tokens);

    /** ContextMenu->Insert-> declaration/implementation */
    int DoClassMethodDeclImpl();

    /** ContextMenu->Insert-> All class methods */
    int DoAllMethodsImpl();

    /** modify the string content to follow the current editor's code style
     * The code style includes the EOL, TAB and indent
     * @param[in,out] str the input string, but also the modified string
     * @param eolStyle an int value to indicate the EOL style
     * @param indent a wxString containing the whole intent text
     * @param useTabs whether TAB is used
     * @param tabSize how long is the TAB
     */
    void MatchCodeStyle(wxString& str, int eolStyle = wxSCI_EOL_LF, const wxString& indent = wxEmptyString, bool useTabs = false, int tabSize = 4);

    // CC's toolbar related functions
    /** helper method in finding the function position in the vector for the function containing the current line */
    void FunctionPosition(int &scopeItem, int &functionItem) const;

    /** navigate between function bodies */
    void GotoFunctionPrevNext(bool next = false);

    /** find the namespace whose scope covers the current line
     * the m_CurrentLine is used
     * @return  the found namespace index
     */
    int NameSpacePosition() const;

    /** Toolbar select event */
    void OnScope(wxCommandEvent& event);

    /** Toolbar select event */
    void OnFunction(wxCommandEvent& event);

    /** normally the editor has changed, then CC need to parse the document again, and (re)construct
     * the internal database, and refresh the toolbar(wxChoice's content)
     */
    void ParseFunctionsAndFillToolbar();

    /** the caret has changed, so the wxChoice need to be updated to indicates which scope and
     * function in which the caret locates.
     */
    void FindFunctionAndUpdate(int currentLine);

    /** the scope item has changed or becomes invalid, so the associated function wxChoice should
     * be updated.
     * @param scopeItem the new item in scope wxChoice.
     */
    void UpdateFunctions(unsigned int scopeItem);

    /** enable the two wxChoices */
    void EnableToolbarTools(bool enable = true);

    /** if C::B starts up with some projects opened, this function will be called to parse the
     * already opened projects
     */
    void DoParseOpenedProjectAndActiveEditor();

    /** highlight member variables */
    void UpdateEditorSyntax(cbEditor* ed = NULL);

    /** delayed for toolbar update */
    void OnToolbarTimer(wxTimerEvent& event);

    /** event fired from the edit event hook function to indicate parsing while editing */
    void OnRealtimeParsingTimer(wxTimerEvent& event);

    /** delayed running after saving project, while many projects' saving */
    void OnProjectSavedTimer(wxTimerEvent& event);

    /** delayed for re-parsing */
    void OnReparsingTimer(wxTimerEvent& event);

    /** delayed running of editor activated event, only the last activated editor should be considered */
    void OnEditorActivatedTimer(wxTimerEvent& event);

    /** Indicates CC's initialization is done */
    bool                    m_InitDone;

    /** menu pointers to the frame's main menu */
    wxMenu*                 m_EditMenu;
    wxMenu*                 m_SearchMenu;
    wxMenu*                 m_ViewMenu;
    wxMenu*                 m_ProjectMenu;

    /** this member will actually manage all the Parser instances */
    NativeParser            m_NativeParser;

    /** code re-factoring tool */
    CodeRefactoring         m_CodeRefactoring;

    int                     m_EditorHookId;

    /** timer triggered by editor hook function to delay the real-time parse */
    wxTimer                 m_TimerRealtimeParsing;

    /** timer for toolbar
     *  we only show an updated item in CC's toolbar's item list when caret position is stable for
     *  a period of time.
     */
    wxTimer                 m_TimerToolbar;

    /* FIXME (ollydbg#1#03/20/15): This timer is added by rev 6510, but I don't know what is the
     * exact reason to delay the reparsing by a timer
     * Note that we also save the project pointer in the m_TimerProjectSaved, so that we can
     * see which project is saved in the timer event handler, it looks like a bug here, which means
     * if two Project Saved event happens continuously, the first one will be overwritten because we
     * save the project pointer member of m_TimerProjectSaved.
     */

    /** delay after project saved event */
    wxTimer                 m_TimerProjectSaved;

    /** delay after receive a project save/modified event */
    wxTimer                 m_TimerReparsing;

    /** delay after receive editor activated event
     *  the reason we need a timer is that we want to get a stable editor activate information
     *  thus we will only handle the last editor activated editor
     *  The timer will be restart when an editor activated event happens.
     */
    wxTimer                 m_TimerEditorActivated;

    /** the last valid editor
     *  it is saved in editor activated event handler, and will be verified in editor activated timer
     *  event handler
     */
    cbEditor*               m_LastEditor;

    // The variables below were related to CC's toolbar
    /** the CC's toolbar */
    wxToolBar*              m_ToolBar;
    /** function choice control of CC's toolbar, it is the second choice */
    wxChoice*               m_Function;
    /** namespace/scope choice control, it is the first choice control */
    wxChoice*               m_Scope;
    /** current active file's function body info */
    FunctionsScopeVec       m_FunctionsScope;
    /** current active file's namespace/scope info */
    NameSpaceVec            m_NameSpaces;
    /** current active file's line info, helper member to access function scopes */
    ScopeMarksVec           m_ScopeMarks;
    /** this is a "filename->info" map containing all the opening files choice info */
    FunctionsScopeMap       m_AllFunctionsScopes;

    /** indicate whether the CC's toolbar need a refresh, this means the toolbar list will be
     *  reconstructed
     */
    bool                    m_ToolbarNeedRefresh;

    /** force to re-collect the CC toolbar's item information
     *  this means we will parse the buffer to collect the scope information
     *  and then rebuild the toolbar items
     */
    bool                    m_ToolbarNeedReparse;

    /** current caret line, this is actually the saved caret line */
    int                     m_CurrentLine;

    /** the file updating the toolbar info */
    wxString                m_LastFile;

    /** indicate whether the predefined keywords set should be added in the suggestion list */
    bool                    m_LexerKeywordsToInclude[9];

    /** indicate the editor has modified by the user and a real-time parse should be start */
    bool                    m_NeedReparse;

    /** remember the number of bytes in the current editor/document
     *  this is actually the saved editor or file's size
     */
    int                     m_CurrentLength;

    /** batch run UpdateEditorSyntax() after first parsing */
    bool                    m_NeedsBatchColour;

    //options on code completion (auto suggestion list) feature

    /** maximum allowed code-completion list entries */
    size_t                  m_CCMaxMatches;

    /** whether add parentheses after user selects a function name in the code-completion suggestion list */
    bool                    m_CCAutoAddParentheses;

    /** add function arguments' types and names when autocompleted outside function. The default
     * value is false.
     */
    bool                    m_CCDetectImplementation;

    /** user defined characters that work like Tab (empty by Default). They will be inserted
     *  with the selected item.
     */
    wxString                m_CCFillupChars;

    /** give code completion list for header files, it happens after the #include directive */
    bool                    m_CCEnableHeaders;

    /** do not allow code completion to add include files of projects/targets
     *  to the parser that are not supported by the current platform
     */
    bool                    m_CCEnablePlatformCheck;

    /* dir to files map, for example, there are two dirs c:/a and c:/b
     * so the map looks like: (usually the relative file path is stored
     * c:/a  ---> {c:/a/a1.h, c:/a/a2.h} ---> {a1.h, a2.h}
     * c:/b  ---> {c:/b/b1.h, c:/b/b2.h} ---> {b1.h, b2.h}
     */
    SystemHeadersMap        m_SystemHeadersMap;

    /** thread to collect header file names, these header file names can be prompt for auto
     * suggestion after #include <  or #include " directives.
     */
    std::list<SystemHeadersThread*> m_SystemHeadersThreads;

    /**  critical section to protect accessing m_SystemHeadersMap */
    wxCriticalSection               m_SystemHeadersThreadCS;

    /** map to record all re-parsing files
     *
     * Here is an example how the ReparsingMap is used. Suppose you have two cbp opened:
     * a.cbp, which contains a1.cpp, a2.cpp and a3.cpp
     * b.cbp, which contains b1,cpp, b2,cpp and b3.cpp
     * now, if a1,cpp and b2.cpp b3.cpp are modified, and the user press the Save all button
     * Then CC receives event about project saved, then we store such information.
     * ReparsingMap contains such two elements
     * (a.cbp, (a1,cpp))
     * (b.cbp, (b2.cpp, b3.cpp))
     * there two elements will be passed to m_NativeParser, and m_NativeParser will distribute
     * to each Parser objects
     */
    typedef std::map<cbProject*, wxArrayString> ReparsingMap;

    /** all the files need to be parsed and their associated projects */
    ReparsingMap m_ReparsingMap;

    /** Provider of documentation for the popup window */
    DocumentationHelper     m_DocHelper;

    // requires access to: m_NativeParser.GetParser().GetTokenTree()
    friend wxString DocumentationHelper::OnDocumentationLink(wxHtmlLinkEvent&, bool&);

    DECLARE_EVENT_TABLE()
};

#endif // CODECOMPLETION_H
