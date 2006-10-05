#ifndef CODECOMPLETION_H
#define CODECOMPLETION_H

#include <settings.h> // SDK
#include <cbplugin.h>
#include <cbproject.h>
#include <simpletextlog.h>
#include <sdk_events.h>
#include <wx/string.h>
#include "nativeparser.h"

#include <wx/timer.h>
#include <map>
#include <vector>

class cbEditor;
class wxScintillaEvent;
class wxChoice;

class CodeCompletion : public cbCodeCompletionPlugin
{
    public:
        CodeCompletion();
        ~CodeCompletion();

        virtual void OnAttach();
        virtual void OnRelease(bool appShutDown);
        int GetConfigurationGroup() const { return cgEditor; }
        cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);
        virtual int Configure();
        virtual void BuildMenu(wxMenuBar* menuBar); // offer for menu space by host
        virtual void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0);
        virtual bool BuildToolBar(wxToolBar* toolBar);
        virtual wxArrayString GetCallTips();
        virtual int CodeComplete();
        virtual void ShowCallTip();

        virtual void CodeCompleteIncludes();

        void EditorEventHook(cbEditor* editor, wxScintillaEvent& event);

        struct FunctionScope
        {
            int StartLine;
            int EndLine;
            wxString Name;
            wxString Scope;
        };
        struct NameSpace
        {
            int StartLine;
            int EndLine;
            wxString Name;
        };

    private:


        void OnUpdateUI(wxUpdateUIEvent& event);
        void OnViewClassBrowser(wxCommandEvent& event);
        void OnCodeComplete(wxCommandEvent& event);
        void OnShowCallTip(wxCommandEvent& event);
        void OnGotoFunction(wxCommandEvent& event);
        void OnClassMethod(wxCommandEvent& event);
        void OnUnimplementedClassMethods(wxCommandEvent& event);
        void OnGotoDeclaration(wxCommandEvent& event);
        void OnOpenIncludeFile(wxCommandEvent& event);
        void OnAppDoneStartup(CodeBlocksEvent& event);
        void OnStartParsingProjects(wxTimerEvent& event);
        void OnCodeCompleteTimer(wxTimerEvent& event);
        void OnProjectOpened(CodeBlocksEvent& event);
        void OnProjectActivated(CodeBlocksEvent& event);
        void OnProjectClosed(CodeBlocksEvent& event);
        void OnProjectFileAdded(CodeBlocksEvent& event);
        void OnProjectFileRemoved(CodeBlocksEvent& event);
        void OnUserListSelection(CodeBlocksEvent& event);
        void OnReparseActiveEditor(CodeBlocksEvent& event);
        void OnEditorActivated(CodeBlocksEvent& event);
        void OnParserEnd(wxCommandEvent& event);
        void OnValueTooltip(CodeBlocksEvent& event);
        void DoCodeComplete();
        void DoInsertCodeCompleteToken(wxString tokName);
        int DoClassMethodDeclImpl();
        int DoAllMethodsImpl();
        int FunctionPosition() const;
        int NameSpacePosition() const;
        void OnFunctionsParsingTimer(wxTimerEvent& event);
        void OnFunction(wxCommandEvent& event);
        void ParseFunctionsAndFillToolbar();

        int m_PageIndex;
        bool m_InitDone;

        wxString m_LastIncludeFileFrom;
        wxString m_LastIncludeFile;
        wxString m_LastKeyword;

        wxMenu* m_EditMenu;
        wxMenu* m_SearchMenu;
        wxMenu* m_ViewMenu;
        NativeParser m_NativeParsers;
        wxTimer m_timer;

        int m_EditorHookId;
        int m_LastPosForCodeCompletion;
        wxTimer m_timerCodeCompletion;
        cbEditor* m_pCodeCompletionLastEditor;
        int m_ActiveCalltipsNest;

        bool m_IsAutoPopup;

        wxChoice* m_Function;
        wxChoice* m_Scope;
        std::vector<FunctionScope> m_FunctionsScope;
        std::vector<NameSpace> m_NameSpaces;
        int StartIdxNameSpaceInScope;
        int m_CurrentLine;

        wxTimer m_FunctionsParsingTimer;

        DECLARE_EVENT_TABLE()
};

#endif // CODECOMPLETION_H
