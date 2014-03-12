/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef CCMANAGER_H
#define CCMANAGER_H

#include "manager.h"

class DLLIMPORT CCManager : public Mgr<CCManager>
{
    public:
        friend class Mgr<CCManager>;
        friend class Manager; // give Manager access to our private members

        cbCodeCompletionPlugin* GetProviderFor(cbEditor* ed = nullptr);

    private:
        CCManager();
        ~CCManager();

        void OnDeactivateApp(CodeBlocksEvent& event);
        void OnDeactivateEd(CodeBlocksEvent& event);
        /** mouse hover event */
        void OnEditorTooltip(CodeBlocksEvent& event);
        /** event handler to show the call tip, when user press Ctrl-Shift-Space */
        void OnShowCallTip(CodeBlocksEvent& event);

        void DoShowTips(const wxStringVec& tips, cbStyledTextCtrl* stc, int pos, int argsPos, int hlStart, int hlEnd);

        cbEditor* lastEditor;
        cbCodeCompletionPlugin* lastCCPlugin;
};

#endif // CCMANAGER_H
