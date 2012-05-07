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

#include <wx/arrstr.h>
#include <wx/string.h>
#include <wx/timer.h>

#include <map>
#include <vector>
#include <set>

class cbEditor;
class wxScintillaEvent;
class wxChoice;

/** Code completion plugin can show function call-tip, automatically suggest code-completion
 *  lists while entering code. Also, it supports navigating the source files, jump between declarations
 *  and implementations, find symbol usage, or even rename a symbol(code re-factoring).
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

        int StartLine;
        int EndLine;
        wxString ShortName;
        wxString Name;
        wxString Scope;    // class or namespace
    };

    /** each file contains on such vector, containing all the function information in the file */
    typedef std::vector<FunctionScope> FunctionsScopeVec;
    /** helper class to support FunctionsScopeVec*/
    typedef std::vector<int> ScopeMarksVec;

    struct FunctionsScopePerFile
    {
        FunctionsScopeVec m_FunctionsScope;
        NameSpaceVec m_NameSpaces;
        bool parsed;
    };
    /** filename -> FunctionsScopePerFile map, contains all the opened documents scope info*/
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
    virtual int Configure();
    /** offer for menu space by host */
    virtual void BuildMenu(wxMenuBar* menuBar);
    /** offer for Context menu */
    virtual void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0);
    /** offer for the Toolbar */
    virtual bool BuildToolBar(wxToolBar* toolBar);
    virtual bool BuildToolBar(wxToolBar* toolBar, int &priority) { priority = 10; return BuildToolBar(toolBar); }

    // TODO unused, should be removed probably
    virtual wxArrayString GetCallTips() { return wxArrayString(); }
    virtual int CodeComplete();
    virtual void ShowCallTip();

    /** give auto suggestions on preprocessor directives*/
    void CodeCompletePreprocessor();
    /** give auto suggestions after #include */
    void CodeCompleteIncludes();

    /** get the include paths setting by the project
     * @param project project info
     * @param buildTargets target info
     * @return the local include paths
     */
    wxArrayString GetLocalIncludeDirs(cbProject* project, const wxArrayString& buildTargets);

    /** the default compilers search paths
     * @param force if false, then it just return a static wxArrayString to optimize the performance
     */
    wxArrayString& GetSystemIncludeDirs(cbProject* project, bool force);

    /** search target file names (mostly relative names) under basePath, then return the absolute dirs*/
    void GetAbsolutePath(const wxString& basePath, const wxArrayString& targets, wxArrayString& dirs);

    /** handle all the editor event*/
    void EditorEventHook(cbEditor* editor, wxScintillaEvent& event);

    /** read CC's options, mostly happens the user change some setting and press APPLY*/
    void RereadOptions(); // called by the configuration panel

private:
    /** Update CC's ToolBar*/
    void UpdateToolBar();

    /** Token replacement map is used for Tokenizer, this function load it from configure file*/
    void LoadTokenReplacements();
    /** write the Token replacement map to the configure file */
    void SaveTokenReplacements();

    /** event handler for updating UI*/
    void OnUpdateUI(wxUpdateUIEvent& event);
    void OnViewClassBrowser(wxCommandEvent& event);
    /** event handler to list the suggestion, when a user press CTRL+space(by default)*/
    void OnCodeComplete(wxCommandEvent& event);
    /** event handler to show the call tip, when user press Ctrl-Shift-Space */
    void OnShowCallTip(wxCommandEvent& event);

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

    void OnCurrentProjectReparse(wxCommandEvent& event);
    void OnSelectedProjectReparse(wxCommandEvent& event);
    void OnSelectedFileReparse(wxCommandEvent& event);
    void OnAppDoneStartup(CodeBlocksEvent& event);

    /** event handlers for the standard events sent from sdk core*/
    void OnWorkspaceChanged(CodeBlocksEvent& event);
    void OnProjectActivated(CodeBlocksEvent& event);
    void OnProjectClosed(CodeBlocksEvent& event);
    void OnProjectSaved(CodeBlocksEvent& event);
    void OnProjectFileAdded(CodeBlocksEvent& event);
    void OnProjectFileRemoved(CodeBlocksEvent& event);
    void OnProjectFileChanged(CodeBlocksEvent& event);
    /** handle the save/modify event from the sdk*/
    void OnEditorSaveOrModified(CodeBlocksEvent& event);
    /** handle editor open event from sdk*/
    void OnEditorOpen(CodeBlocksEvent& event);
    void OnEditorActivated(CodeBlocksEvent& event);
    void OnEditorClosed(CodeBlocksEvent& event);

    /** CC's own logger, to handle event sent from other thread or itself*/
    void OnCCLogger(wxCommandEvent& event);
    /** CC's own debug logger, to handle event sent from other thread ot itself*/
    void OnCCDebugLogger(wxCommandEvent& event);

    /** batch parsing end event*/
    void OnParserEnd(wxCommandEvent& event);
    /** batch parsing start event*/
    void OnParserStart(wxCommandEvent& event);

    /** mouse hover event*/
    void OnEditorTooltip(CodeBlocksEvent& event);

    /** receive event from SystemHeadersThread */
    void OnSystemHeadersThreadUpdate(wxCommandEvent& event);
    void OnSystemHeadersThreadCompletion(wxCommandEvent& event);
    void OnSystemHeadersThreadError(wxCommandEvent& event);

    /** show code suggestion list*/
    void DoCodeComplete();

    /** ContextMenu->Insert-> declaration/implementation*/
    int DoClassMethodDeclImpl();
    /** ContextMenu->Insert-> All class methods*/
    int DoAllMethodsImpl();

    //CC's toolbar related functions
    /** help method in finding the function position in the vector for the function containing the current line*/
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

    /** Not used*/
    int                     m_PageIndex;
    /** Indicates CC's initialization is done*/
    bool                    m_InitDone;

    /** menu pointers to the frame's main menu*/
    wxMenu*                 m_EditMenu;
    wxMenu*                 m_SearchMenu;
    wxMenu*                 m_ViewMenu;
    wxMenu*                 m_ProjectMenu;

    /** this member will actually manage the many Parser objects*/
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


    cbEditor*               m_LastEditor;
    int                     m_ActiveCalltipsNest;

    bool                    m_IsAutoPopup;
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
    /** TODO */
    std::map<wxString, int> m_SearchItem;

    /** the file updating the toolbar info*/
    wxString                m_LastFile;

    /** indicate whether the predefined keywords set should be added in the suggestion list*/
    bool                    m_LexerKeywordsToInclude[9];

    /** indicate the editor has modified by the user and a real-time parse should be start*/
    bool                    m_NeedReparse;

    /** remember the number of bytes in the current editor/document */
    int                     m_CurrentLength;
    /** collected header file names to support auto-completion after #include*/
    SystemHeadersMap        m_SystemHeadersMap;

    //options on code completion plugins
    bool                    m_UseCodeCompletion;
    int                     m_CCAutoLaunchChars;
    bool                    m_CCAutoLaunch;
    int                     m_CCLaunchDelay;
    size_t                  m_CCMaxMatches;
    bool                    m_CCAutoAddParentheses;
    wxString                m_CCFillupChars;
    bool                    m_CCAutoSelectOne;
    bool                    m_CCEnableHeaders;

    /** thread to collect header file names */
    std::list<SystemHeadersThread*> m_SystemHeadersThreads;
    wxCriticalSection               m_SystemHeadersThreadCS;

    /** map to collect all re-parsing files */
    typedef std::map<cbProject*, wxArrayString> ReparsingMap;
    ReparsingMap m_ReparsingMap;

    DECLARE_EVENT_TABLE()
};

#endif // CODECOMPLETION_H
