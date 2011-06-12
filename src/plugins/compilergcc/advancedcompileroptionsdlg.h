/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef ADVANCEDCOMPILEROPTIONSDLG_H
#define ADVANCEDCOMPILEROPTIONSDLG_H

#include "scrollingdialog.h"
#include <wx/string.h>
#include "compiler.h" // RegExArray, COMPILER_COMMAND_TYPES_COUNT

class wxCommandEvent;
class wxSpinEvent;

class AdvancedCompilerOptionsDlg : public wxScrollingDialog
{
    public:
        AdvancedCompilerOptionsDlg(wxWindow* parent, const wxString& compilerId);
        virtual ~AdvancedCompilerOptionsDlg();
        bool IsDirty() const {return m_bDirty;}
    private:
        void OnCommandsChange(wxCommandEvent& event);
        void OnExtChange(wxCommandEvent& event);
        void OnAddExt(wxCommandEvent& event);
        void OnDelExt(wxCommandEvent& event);
        void OnRegexChange(wxCommandEvent& event);
        void OnRegexTest(wxCommandEvent& event);
        void OnRegexAdd(wxCommandEvent& event);
        void OnRegexDelete(wxCommandEvent& event);
        void OnRegexDefaults(wxCommandEvent& event);
        void OnRegexUp(wxSpinEvent& event);
        void OnRegexDown(wxSpinEvent& event);

        void EndModal(int retCode);
        void ReadCompilerOptions();
        void WriteCompilerOptions();
        void ReadExtensions(int nr);
        void DisplayCommand(int cmd, int ext);
        void CheckForChanges();

        CompilerTool* GetCompilerTool(int cmd, int ext);
        void SaveCommands(int cmd, int ext);

        void FillRegexes();
        void FillRegexDetails(int index);
        void SaveRegexDetails(int index);

        wxString m_CompilerId;
        int m_LastCmdIndex;
        int m_LastExtIndex;
        RegExArray m_Regexes;
        int m_SelectedRegex;
        bool m_bDirty; //!< Have changes being made to settings in the dialog
        CompilerToolsVector m_Commands[ctCount];
        DECLARE_EVENT_TABLE();
};

#endif // ADVANCEDCOMPILEROPTIONSDLG_H

