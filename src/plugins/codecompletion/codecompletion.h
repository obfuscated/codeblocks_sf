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
#include <wx/string.h>

#include "nativeparser.h"
#include "coderefactoring.h"

#include <wx/timer.h>
#include <map>
#include <vector>
#include <set>

class cbEditor;
class wxScintillaEvent;
class wxChoice;
class SystemHeadersThread;

typedef std::map<wxString, StringSet> SystemHeadersMap;

class CodeCompletion : public cbCodeCompletionPlugin
{
public:
    struct FunctionScope
    {
        FunctionScope() {}
        FunctionScope(const NameSpace& ns): StartLine(ns.StartLine), EndLine(ns.EndLine), Scope(ns.Name) {}

        int StartLine;
        int EndLine;
        wxString Name;
        wxString Scope;    // class or namespace
    };

    typedef std::vector<FunctionScope> FunctionsScopeVec;
    typedef std::vector<int> ScopeMarksVec;

    struct FunctionsScopePerFile
    {
        FunctionsScopeVec m_FunctionsScope;
        NameSpaceVec m_NameSpaces;
        bool parsed;
    };

    typedef std::map<wxString, FunctionsScopePerFile> FunctionsScopeMap;

    CodeCompletion();
    virtual ~CodeCompletion();

    virtual void OnAttach();
    virtual void OnRelease(bool appShutDown);
    virtual int GetConfigurationGroup() const { return cgEditor; }
    virtual cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);
    virtual cbConfigurationPanel* GetProjectConfigurationPanel(wxWindow* parent, cbProject* project);
    virtual int Configure();
    virtual void BuildMenu(wxMenuBar* menuBar); // offer for menu space by host
    virtual void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0);
    virtual bool BuildToolBar(wxToolBar* toolBar);

    // unused, should be removed probably
    virtual wxArrayString GetCallTips() { return wxArrayString(); }
    virtual int CodeComplete();
    virtual void ShowCallTip();

    void CodeCompletePreprocessor();
    void CodeCompleteIncludes();
    wxArrayString GetLocalIncludeDirs(cbProject* project, const wxArrayString& buildTargets);
    wxArrayString& GetSystemIncludeDirs(cbProject* project, bool force);
    void GetAbsolutePath(const wxString& basePath, const wxArrayString& targets, wxArrayString& dirs);

    void EditorEventHook(cbEditor* editor, wxScintillaEvent& event);
    void RereadOptions(); // called by the configuration panel

private:
    void UpdateToolBar();

    void LoadTokenReplacements();
    void SaveTokenReplacements();

    void OnUpdateUI(wxUpdateUIEvent& event);
    void OnViewClassBrowser(wxCommandEvent& event);
    void OnCodeComplete(wxCommandEvent& event);
    void OnShowCallTip(wxCommandEvent& event);
    void OnGotoFunction(wxCommandEvent& event);
    void OnGotoPrevFunction(wxCommandEvent& event);
    void OnGotoNextFunction(wxCommandEvent& event);
    void OnClassMethod(wxCommandEvent& event);
    void OnUnimplementedClassMethods(wxCommandEvent& event);
    void OnGotoDeclaration(wxCommandEvent& event);
    void OnFindReferences(wxCommandEvent& event);
    void OnRenameSymbols(wxCommandEvent& event);
    void OnOpenIncludeFile(wxCommandEvent& event);
    void OnCurrentProjectReparse(wxCommandEvent& event);
    void OnSelectedProjectReparse(wxCommandEvent& event);
    void OnSelectedFileReparse(wxCommandEvent& event);
    void OnAppDoneStartup(CodeBlocksEvent& event);
    void OnCodeCompleteTimer(wxTimerEvent& event);
    void OnWorkspaceChanged(CodeBlocksEvent& event);
    void OnProjectActivated(CodeBlocksEvent& event);
    void OnProjectClosed(CodeBlocksEvent& event);
    void OnProjectSaved(CodeBlocksEvent& event);
    void OnProjectFileAdded(CodeBlocksEvent& event);
    void OnProjectFileRemoved(CodeBlocksEvent& event);
    void OnProjectFileChanged(CodeBlocksEvent& event);
    void OnUserListSelection(CodeBlocksEvent& event);
    void OnReparseActiveEditor(CodeBlocksEvent& event);
    void OnEditorOpen(CodeBlocksEvent& event);
    void OnEditorActivated(CodeBlocksEvent& event);
    void OnEditorClosed(CodeBlocksEvent& event);
    void OnEditorModified(CodeBlocksEvent& event);
    void OnParserEnd(wxCommandEvent& event);
    void OnParserStart(wxCommandEvent& event);
    void OnValueTooltip(CodeBlocksEvent& event);
    void OnThreadUpdate(wxCommandEvent& event);
    void OnThreadCompletion(wxCommandEvent& event);

    void DoCodeComplete();
    void DoInsertCodeCompleteToken(wxString tokName);
    int DoClassMethodDeclImpl();
    int DoAllMethodsImpl();
    void FunctionPosition(int &scopeItem, int &functionItem) const;
    void GotoFunctionPrevNext(bool next = false);
    int NameSpacePosition() const;
    void OnStartParsingFunctions(wxTimerEvent& event);
    void OnFindFunctionAndUpdate(wxTimerEvent& event);
    void OnScope(wxCommandEvent& event);
    void OnFunction(wxCommandEvent& event);
    void ParseFunctionsAndFillToolbar(bool force = false);
    void FindFunctionAndUpdate(int currentLine);
    void UpdateFunctions(unsigned int scopeItem);
    void EnableToolbarTools(bool enable = true);
    void OnRealtimeParsing(wxTimerEvent& event);
    void OnProjectSavedTimer(wxTimerEvent& event);
    void GotoTokenPosition(cbEditor* editor, const wxString& target, size_t line);

    int                     m_PageIndex;
    bool                    m_InitDone;

    wxMenu*                 m_EditMenu;
    wxMenu*                 m_SearchMenu;
    wxMenu*                 m_ViewMenu;
    wxMenu*                 m_ProjectMenu;
    NativeParser            m_NativeParser;
    CodeRefactoring         m_CodeRefactoring;
    int                     m_EditorHookId;
    int                     m_LastPosForCodeCompletion;
    wxTimer                 m_TimerCodeCompletion;
    wxTimer                 m_TimerFunctionsParsing;
    wxTimer                 m_TimerRealtimeParsing;
    wxTimer                 m_TimerToolbar;
    wxTimer                 m_TimerProjectSaved;
    cbEditor*               m_LastEditor;
    int                     m_ActiveCalltipsNest;

    bool                    m_IsAutoPopup;

    wxToolBar*              m_ToolBar;
    wxChoice*               m_Function;
    wxChoice*               m_Scope;
    FunctionsScopeVec       m_FunctionsScope;
    NameSpaceVec            m_NameSpaces;
    ScopeMarksVec           m_ScopeMarks;
    FunctionsScopeMap       m_AllFunctionsScopes;
    bool                    m_ToolbarChanged;

    int                     m_CurrentLine;
    std::map<wxString, int> m_SearchItem;
    wxString                m_LastFile;
    bool                    m_LexerKeywordsToInclude[9];
    bool                    m_NeedReparse;
    int                     m_CurrentLength;
    SystemHeadersMap        m_SystemHeadersMap;

    bool                    m_UseCodeCompletion;
    int                     m_CCAutoLaunchChars;
    bool                    m_CCAutoLaunch;
    int                     m_CCLaunchDelay;
    size_t                  m_CCMaxMatches;
    bool                    m_CCAutoAddParentheses;
    wxString                m_CCFillupChars;
    bool                    m_CCAutoSelectOne;

    std::list<SystemHeadersThread*> m_SystemHeadersThread;

    DECLARE_EVENT_TABLE()
};

#endif // CODECOMPLETION_H
