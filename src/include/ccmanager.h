/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CCMANAGER_H
#define CCMANAGER_H

#include "manager.h"

#ifndef CB_PRECOMP
    #include <wx/timer.h>
    #include "cbplugin.h"
#endif

class UnfocusablePopupWindow;
class wxHtmlLinkEvent;
class wxHtmlWindow;
class wxListEvent;
class wxListView;
class wxScintillaEvent;

/** code completion plugin manager
 *
 *  This class manages all kinds of code completion plugins. For example, the native C/C++ code
 *  completion plugin, the Fortran code completion plugin and Python code completion plugin.
 *  The ccmanager asks some informations from the managed ccplugins. The major value passes from the
 *  ccmanager and the ccplugins are CCCallTip and CCToken.
 *
 *  The CCToken is needed when ccmanager needs to fill a code suggestion list, it is the ccplugin's
 *  duty to supply a collection of CCTokens.
 *
 *  When user is entering some function call statement, CCCallTip is used to show the function
 *  information, and especially it can highlight the current function argument.
 *
 *  Another kind of tip is the tooltip, this is the tip window shown when mouse hover on a specified
 *  token(such as variable token), ccmanager queries this information by asking CCToken information
 *  from ccplugin.
 */
class DLLIMPORT CCManager : public Mgr<CCManager>, wxEvtHandler
{
    public:
        friend class Mgr<CCManager>;
        friend class Manager; // give Manager access to our private members

        /** get the ccPlugin instant for the cbEditor
         *  If the editor opened a c/c++ file, then it returns the native c/c++ ccPlugin.
         *  @param ed input editor, uses active editor if one is not passed; has (minimal) cache optimization
         */
        cbCodeCompletionPlugin* GetProviderFor(cbEditor* ed = nullptr);

        /** if the default set of characters that invoke calltip requests are not appropriate, register a new set */
        void RegisterCallTipChars(const wxString& chars, cbCodeCompletionPlugin* registrant);
        /** if the default set of characters that auto-launch codecomplete requests are not appropriate, register a new set */
        void RegisterAutoLaunchChars(const wxString& chars, cbCodeCompletionPlugin* registrant);

        /** let CCManager know that new results are available from cbCodeCompletionPlugin::GetDocumentation() */
        void NotifyDocumentation();
        /** let CCManager know that a change (e.g. active lexer is switched) may invalidate cached active ccPlugin */
        void NotifyPluginStatus();

        /** if for some reason you *need* to use wxScintilla::AutoCompShow(), call this instead so CCManager does not step on you */
        void InjectAutoCompShow(int lenEntered, const wxString& itemList);

        /** used by cbStyledTextCtrl to process ArrowUp and ArrowDown key press. It is not intended to be called by ccPlugins. */
        bool ProcessArrow(int key);
    private:
        CCManager();
        ~CCManager();

        /** event handler to list the suggestion, when a user press Ctrl-space (by default) */
        void OnCompleteCode(CodeBlocksEvent& event);
        /** event handlers to avoid tooltips getting stuck active */
        void OnDeactivateApp(CodeBlocksEvent& event);
        void OnDeactivateEd(CodeBlocksEvent& event);
        void OnEditorOpen(CodeBlocksEvent& event);
        void OnEditorClose(CodeBlocksEvent& event);

        /** hook to the editor, so that we know some specific chars were entered, then we can decide
         *  whether it time to show a calltip or suggestion list
         */
        void OnEditorHook(cbEditor* ed, wxScintillaEvent& event);

        /** mouse hover event */
        void OnEditorTooltip(CodeBlocksEvent& event);

        /** event handler to show the call tip, when user press Ctrl-Shift-Space */
        void OnShowCallTip(CodeBlocksEvent& event);

        /** event handler to show documentation, when user changes autocomplete selection */
        void OnAutocompleteSelect(wxListEvent& event);
        void OnAutocompleteHide(wxShowEvent& event);

        /** defer showing the calltip to avoid a crash issue, @see CCManager::OnDeferredCallTipShow */
        void OnDeferredCallTipShow(wxCommandEvent& event);
        /** defer canceling the calltip to avoid a crash issue, @see CCManager::OnDeferredCallTipShow */
        void OnDeferredCallTipCancel(wxCommandEvent& event);
#ifdef __WXMSW__
        /** intercept cbStyledTextCtrl scroll events and forward to autocomplete/documentation popups */
        void OnPopupScroll(wxMouseEvent& event);
#endif // __WXMSW__

        /** a link is clicked in the document window */
        void OnHtmlLink(wxHtmlLinkEvent& event);

        /** comment timer event handler for several timers */
        void OnTimer(wxTimerEvent& event);

        /** handle the CC related menu click */
        void OnMenuSelect(wxCommandEvent& event);

        void DoBufferedCC(cbStyledTextCtrl* stc);
        void DoHidePopup();
        void DoShowDocumentation(cbEditor* ed);
        void DoUpdateCallTip(cbEditor* ed);

        /** user click the next or previous small button of the tip window */
        enum Direction { Previous, Next };

        /** switch the tip window to the next item */
        void AdvanceTip(Direction direction);

        /** format tips by breaking long lines at (hopefully) logical places */
        void DoShowTips(const wxStringVec& tips, cbStyledTextCtrl* stc, int pos, int argsPos, int hlStart, int hlEnd);

        /** code completion as just insert some text in the editor, ask the smart indent plugin to adjust the indent */
        void CallSmartIndentCCDone(cbEditor* ed);

        typedef std::map< cbCodeCompletionPlugin*, std::set<wxChar> > CCPluginCharMap;
        CCPluginCharMap m_CallTipChars;    //!< chars each plugin is interested in for calltip state
        CCPluginCharMap m_AutoLaunchChars; //!< chars each plugin is interested in for autocomplete
        int m_EditorHookID;
        int m_AutocompPosition;  //!< location of caret when autocomplete timer starts, if caret is still there, launch autocomplete
        int m_CallTipActive;     //!< is CCManager currently displaying a calltip, and if so, where
        int m_LastAutocompIndex; //!< index of currently selected entry in autocomplete popup
        int m_LastTipPos;        //!< last location a tool/call tip was displayed
        int m_WindowBound;       //!< right-most boundary the documentation popup is allowed to stretch to
        bool m_OwnsAutocomp;     //!< do we control the current autocomplete popup?
        typedef std::vector<cbCodeCompletionPlugin::CCCallTip> CallTipVec;
        CallTipVec m_CallTips; //!< cached calltips
        CallTipVec::const_iterator m_CurCallTip;   //!< remember current choice
        std::map<int, size_t> m_CallTipChoiceDict; //!< remember past choices
        std::map<int, size_t> m_CallTipFuzzyChoiceDict; //!< remember past choices based on prefix
        wxTimer m_CallTipTimer;
        wxTimer m_AutoLaunchTimer;
        wxTimer m_AutocompSelectTimer;
        wxSize m_DocSize; //!< size of the documentation popup
        wxPoint m_DocPos; //!< location of the documentation popup
#ifdef __WXMSW__
        wxListView* m_pAutocompPopup;
#endif // __WXMSW__
        cbEditor* m_pLastEditor; //!< last editor operated on
        cbCodeCompletionPlugin* m_pLastCCPlugin; //!< the plugin handling m_pLastEditor
        UnfocusablePopupWindow* m_pPopup; //!< container for documentation popup
        wxHtmlWindow* m_pHtml;            //!< documentation popup
        int m_LastACLaunchState[2];
        std::vector<cbCodeCompletionPlugin::CCToken> m_AutocompTokens; //!< cached autocomplete list
};

#endif // CCMANAGER_H
