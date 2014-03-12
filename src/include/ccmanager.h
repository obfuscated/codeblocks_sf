/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CCMANAGER_H
#define CCMANAGER_H

#include "manager.h"

class UnfocusablePopupWindow;
class wxHtmlWindow;

class DLLIMPORT CCManager : public Mgr<CCManager>, wxEvtHandler
{
    public:
        friend class Mgr<CCManager>;
        friend class Manager; // give Manager access to our private members

        /** uses active editor if one is not passed; has (minimal) cache optimization */
        cbCodeCompletionPlugin* GetProviderFor(cbEditor* ed = nullptr);

    private:
        CCManager();
        ~CCManager();

        /** event handler to list the suggestion, when a user press Ctrl-space (by default) */
        void OnCompleteCode(CodeBlocksEvent& event);
        void OnDeactivateApp(CodeBlocksEvent& event);
        void OnDeactivateEd(CodeBlocksEvent& event);
        void OnEditorOpen(CodeBlocksEvent& event);
        void OnEditorClose(CodeBlocksEvent& event);
        void OnEditorHook(cbEditor* ed, wxScintillaEvent& event);
        /** mouse hover event */
        void OnEditorTooltip(CodeBlocksEvent& event);
        /** event handler to show the call tip, when user press Ctrl-Shift-Space */
        void OnShowCallTip(CodeBlocksEvent& event);
        /** event handler to show documentation, when user changes autocomplete selection */
        void OnAutocompleteSelect(wxListEvent& event);
        void OnAutocompleteHide(wxShowEvent& event);
#ifdef __WXMSW__
        /** intercept cbStyledTextCtrl scroll events and forward to autocomplete/documentation popups */
        void OnPopupScroll(wxMouseEvent& event);
#endif // __WXMSW__
        void OnHtmlLink(wxHtmlLinkEvent& event);
        void OnTimer(wxTimerEvent& event);

        void DoBufferedCC(cbStyledTextCtrl* stc);
        void DoHidePopup();
        void DoShowDocumentation(cbEditor* ed);
        /** format tips by breaking long lines at (hopefully) logical places */
        void DoShowTips(const wxStringVec& tips, cbStyledTextCtrl* stc, int pos, int argsPos, int hlStart, int hlEnd);

        std::set<wxChar> m_CallTipChars;
        std::set<wxChar> m_AutoLaunchChars;
        int m_EditorHookID;
        int m_AutocompPosition;
        int m_CallTipActive;
        int m_LastAutocompIndex;
        int m_LastTipPos;
        wxTimer m_CallTipTimer;
        wxTimer m_AutoLaunchTimer;
        wxTimer m_AutocompSelectTimer;
        wxSize m_DocSize;
        wxPoint m_DocPos;
#ifdef __WXMSW__
        wxListView* m_pAutocompPopup;
#endif // __WXMSW__
        cbEditor* m_pLastEditor;
        cbCodeCompletionPlugin* m_pLastCCPlugin;
        UnfocusablePopupWindow* m_pPopup;
        wxHtmlWindow* m_pHtml;
        int m_LastACLaunchState[2];
        std::vector<cbCodeCompletionPlugin::CCToken> m_AutocompTokens;
};

#endif // CCMANAGER_H
