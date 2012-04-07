/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef DEBUGGEROPTIONSDLG_H
#define DEBUGGEROPTIONSDLG_H

#include <debuggermanager.h>

class ConfigManagerWrapper;

class DebuggerConfiguration : public cbDebuggerConfiguration
{
    public:
        explicit DebuggerConfiguration(const ConfigManagerWrapper &config);

        virtual cbDebuggerConfiguration* Clone() const;
        virtual wxPanel* MakePanel(wxWindow *parent);
        virtual bool SaveChanges(wxPanel *panel);
    public:
        enum Flags
        {
            WatchFuncArgs,
            WatchLocals,
            WatchScriptPrinters,
            CatchExceptions,
            EvalExpression,
            AddOtherProjectDirs,
            DoNotRun
        };

        bool GetFlag(Flags flag);
        void SetFlag(Flags flag, bool value);
        bool IsGDB();
        wxString GetDebuggerExecutable(bool expandMacro = true);
        wxString GetUserArguments(bool expandMacro = true);
        wxString GetDisassemblyFlavorCommand();
        wxString GetInitCommands();

};

#endif // DEBUGGEROPTIONSDLG_H
