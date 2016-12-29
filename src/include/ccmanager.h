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

/** Code Completion Plugin Manager
 *
 * This class manages all kinds of code completion plugins. For example, the native C/C++ code
 * completion plugin, the Fortran code completion plugin and Python code completion plugin.
 * The ccmanager asks some informations from the managed ccplugins. The major value passes from the
 * ccmanager and the ccplugins are CCCallTip and CCToken.
 *
 * The CCToken is needed when ccmanager needs to fill a code suggestion list, it is the ccplugin's
 * duty to supply a collection of CCTokens.
 *
 * When user is entering some function call statement, CCCallTip is used to show the function
 * information, and especially it can highlight the current function argument.
 *
 * Another kind of tip is the tooltip, this is the tip window shown when mouse hover on a specified
 * token(such as variable token), ccmanager queries this information by asking CCToken information
 * from ccplugin.
 */
class DLLIMPORT CCManager : public Mgr<CCManager>, wxEvtHandler
{
    public:
        friend class Mgr<CCManager>;
        friend class Manager; // give Manager access to our private members

        /** Get the ccPlugin instant for the cbEditor.
         *
         * If the editor opened a c/c++ file, then it returns the native c/c++ ccPlugin.
         * @param ed input editor, uses active editor if one is not passed; has (minimal) cache optimization
         */
        cbCodeCompletionPlugin* GetProviderFor(cbEditor* ed = nullptr);

        /** Register a new set of characters that, when typed, invoke calltip requests.
         *
         * Use this if the default set is not appropriate.
         */
        void RegisterCallTipChars(const wxString& chars, cbCodeCompletionPlugin* registrant);


        /** Register a new set of characters that, when typed, auto-launch codecomplete requests.
         *
         * Use this if the default set is not appropriate.
         */
        void RegisterAutoLaunchChars(const wxString& chars, cbCodeCompletionPlugin* registrant);

        /** Let CCManager know that new results are available from cbCodeCompletionPlugin::GetDocumentation(). */
        void NotifyDocumentation();
        /** Let CCManager know that a change (e.g. active lexer is switched) may invalidate cached active ccPlugin. */
        void NotifyPluginStatus();

        /** If for some reason you *need* to use wxScintilla::AutoCompShow(), call this instead so CCManager does not step on you. */
        void InjectAutoCompShow(int lenEntered, const wxString& itemList);

        /** Used by cbStyledTextCtrl to process ArrowUp and ArrowDown key press.
         *
         * This is not intended to be called by ccPlugins.
         */
        bool ProcessArrow(int key);

        /** Called after env settings have changed, so the changes can be applied. */
        void UpdateEnvSettings();
    private:
        CCManager();
        ~CCManager();

        /** Event handler to list the suggestion, when a user press Ctrl-space (by default). */
        void OnCompleteCode(CodeBlocksEvent& event);

        /** Event handler to avoid tooltips getting stuck active. */
        void OnDeactivateApp(CodeBlocksEvent& event);
        /** Event handler to avoid tooltips getting stuck active. */
        void OnDeactivateEd(CodeBlocksEvent& event);
        /** Event handler to avoid tooltips getting stuck active. */
        void OnEditorOpen(CodeBlocksEvent& event);
        /** Event handler to avoid tooltips getting stuck active. */
        void OnEditorClose(CodeBlocksEvent& event);

        /** Hook to the editor.
         *
         * This is used so that we know some specific chars were entered, then we can decide
         * whether it time to show a calltip or suggestion list.
         */
        void OnEditorHook(cbEditor* ed, wxScintillaEvent& event);

        /** Mouse hover event. */
        void OnEditorTooltip(CodeBlocksEvent& event);

        /** Event handler to show the call tip, when user press Ctrl-Shift-Space. */
        void OnShowCallTip(CodeBlocksEvent& event);

        /** Event handler to show documentation, when user changes autocomplete selection. */
        void OnAutocompleteSelect(wxListEvent& event);
        /** Event handler to tear down documentation, when autocomplete closes. */
        void OnAutocompleteHide(wxShowEvent& event);

        /** Defer showing the calltip to avoid a crash issue.
         *
         * Launching this event directly seems to be a candidate for race condition
         * and crash in OnShowCallTip() so we attempt to serialize it.
         */
        void OnDeferredCallTipShow(wxCommandEvent& event);
        /** Defer canceling the calltip to avoid a crash issue. @see CCManager::OnDeferredCallTipShow */
        void OnDeferredCallTipCancel(wxCommandEvent& event);
#ifdef __WXMSW__
        /** Intercept cbStyledTextCtrl scroll events and forward to autocomplete/documentation popups. */
        void OnPopupScroll(wxMouseEvent& event);
#endif // __WXMSW__

        /** A link is clicked in the document window. */
        void OnHtmlLink(wxHtmlLinkEvent& event);

        /** Grouped event handler for several timers. */
        void OnTimer(wxTimerEvent& event);

        /** Handle the CC related menu click. */
        void OnMenuSelect(wxCommandEvent& event);

        /** CC launched in the same state as last run, display via cached data. */
        void DoBufferedCC(cbStyledTextCtrl* stc);

        /** Hide the documentation popup. */
        void DoHidePopup();

        /** Show the documentation popup. */
        void DoShowDocumentation(cbEditor* ed);

        /** Update which tip to show next and where to show it. */
        void DoUpdateCallTip(cbEditor* ed);

        /** User click the next or previous small button of the tip window. */
        enum Direction { Previous, Next };

        /**Switch the tip window to the next item. */
        void AdvanceTip(Direction direction);

        /** Format tips by breaking long lines at (hopefully) logical places. */
        void DoShowTips(const wxStringVec& tips, cbStyledTextCtrl* stc, int pos, int argsPos, int hlStart, int hlEnd);

        /** Code completion as just insert some text in the editor, ask the smart indent plugin to adjust the indent. */
        void CallSmartIndentCCDone(cbEditor* ed);

        typedef std::map< cbCodeCompletionPlugin*, std::set<wxChar> > CCPluginCharMap;
        CCPluginCharMap m_CallTipChars;    //!< Chars each plugin is interested in for calltip state.
        CCPluginCharMap m_AutoLaunchChars; //!< Chars each plugin is interested in for autocomplete.
        int m_EditorHookID;
        int m_AutocompPosition;  //!< Location of caret when autocomplete timer starts, if caret is still there, launch autocomplete.
        int m_CallTipActive;     //!< Is CCManager currently displaying a calltip, and if so, where.
        int m_LastAutocompIndex; //!< Index of currently selected entry in autocomplete popup.
        int m_LastTipPos;        //!< Last location a tool/call tip was displayed.
        int m_WindowBound;       //!< Right-most boundary the documentation popup is allowed to stretch to.
        bool m_OwnsAutocomp;     //!< Do we control the current autocomplete popup?
        typedef std::vector<cbCodeCompletionPlugin::CCCallTip> CallTipVec;
        CallTipVec m_CallTips;   //!< Cached calltips
        CallTipVec::const_iterator m_CurCallTip;   //!< Remember current choice.
        std::map<int, size_t> m_CallTipChoiceDict; //!< Remember past choices.
        std::map<int, size_t> m_CallTipFuzzyChoiceDict; //!< Remember past choices based on prefix.
        wxTimer m_CallTipTimer;
        wxTimer m_AutoLaunchTimer;
        wxTimer m_AutocompSelectTimer;
        wxSize m_DocSize; //!< Size of the documentation popup.
        wxPoint m_DocPos; //!< Location of the documentation popup.
#ifdef __WXMSW__
        wxListView* m_pAutocompPopup;
#endif // __WXMSW__
        cbEditor* m_pLastEditor; //!< Last editor operated on.
        cbCodeCompletionPlugin* m_pLastCCPlugin; //!< The plugin handling m_pLastEditor.
        UnfocusablePopupWindow* m_pPopup; //!< Container for documentation popup.
        wxHtmlWindow* m_pHtml;            //!< Documentation popup.
        int m_LastACLaunchState[2];
        std::vector<cbCodeCompletionPlugin::CCToken> m_AutocompTokens; //!< Cached autocomplete list.
};

#endif // CCMANAGER_H
