/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CCMANAGER_H
#define CCMANAGER_H

#include "manager.h"

class DLLIMPORT CCManager : public Mgr<CCManager>, wxEvtHandler
{
    public:
        friend class Mgr<CCManager>;
        friend class Manager; // give Manager access to our private members

        cbCodeCompletionPlugin* GetProviderFor(cbEditor* ed = nullptr);

    private:
        CCManager();
        ~CCManager();

        /** event handler to list the suggestion, when a user press Ctrl-space (by default) */
        void OnCompleteCode(CodeBlocksEvent& event);
        void OnDeactivateApp(CodeBlocksEvent& event);
        void OnDeactivateEd(CodeBlocksEvent& event);
        void OnEditorHook(cbEditor* ed, wxScintillaEvent& event);
        /** mouse hover event */
        void OnEditorTooltip(CodeBlocksEvent& event);
        /** event handler to show the call tip, when user press Ctrl-Shift-Space */
        void OnShowCallTip(CodeBlocksEvent& event);
        void OnTimer(wxTimerEvent& event);

        void DoShowTips(const wxStringVec& tips, cbStyledTextCtrl* stc, int pos, int argsPos, int hlStart, int hlEnd);

        std::set<wxChar> m_CallTipChars;
        std::set<wxChar> m_AutoLaunchChars;
        int m_EditorHookID;
        int m_AutocompPosition;
        int m_CallTipActive;
        wxTimer m_CallTipTimer;
        wxTimer m_AutoLaunchTimer;
        cbEditor* m_pLastEditor;
        cbCodeCompletionPlugin* m_pLastCCPlugin;
};

#endif // CCMANAGER_H
