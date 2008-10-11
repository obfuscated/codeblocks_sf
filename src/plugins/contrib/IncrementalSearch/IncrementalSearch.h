/***************************************************************
 * Name:      IncrementalSearch
 * Purpose:   Code::Blocks plugin
 * Author:    Jens Lody (debian@jenslody.de)
 * Created:   2008-10-01
 * Copyright: Jens Lody
 * License:   GPL
 **************************************************************/

#ifndef INCREMENTAL_SEARCH_H_INCLUDED
#define INCREMENTAL_SEARCH_H_INCLUDED

#include <cbplugin.h>

class cbStyledTextCtrl;

class IncrementalSearch : public cbPlugin
{
    public:
        IncrementalSearch();
        ~IncrementalSearch();

        int GetConfigurationPriority() const { return 50; }
        int GetConfigurationGroup() const { return cgEditor; }
        cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);
        cbConfigurationPanel* GetProjectConfigurationPanel(wxWindow* parent, cbProject* project){ return 0; }
        void BuildMenu(wxMenuBar* menuBar);
        void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0){}
        bool BuildToolBar(wxToolBar* toolBar);

    private:
        void OnAttach();
        void OnRelease(bool appShutDown);
        void OnFocusToolbar(wxCommandEvent& event);
        void OnTextChanged(wxCommandEvent& event);
        void OnSearchPrev(wxCommandEvent& event);
        void OnSearchNext(wxCommandEvent& event);
        void OnToggleToolbar(wxCommandEvent& event);
        void OnEditorEvent(CodeBlocksEvent& event);
        void OnToggleHighlight(wxCommandEvent& event);
        void OnToggleSelectedOnly(wxCommandEvent& event);
        void OnToggleMatchCase(wxCommandEvent& event);
        void HighlightText();
        void SearchText();
        void SearchForward(int fromPos);
        void SearchBackward(int fromPos);
        void VerifyPosition();
        void VerifyRange();
        wxString m_SearchText;
        wxColour m_textCtrlBG_Default;
        wxToolBar* m_pToolbar;
        wxTextCtrl* m_pTextCtrl;
        cbStyledTextCtrl* m_pControl;
        int m_NewPos;
        int m_OldPos;
        int m_SelStart;
        int m_SelEnd;
        int m_MinPos;
        int m_MaxPos;
        int m_flags;
        bool m_Highlight;
        bool m_SelectedOnly;

        DECLARE_EVENT_TABLE();
};

#endif // INCREMENTAL_SEARCH_H_INCLUDED
