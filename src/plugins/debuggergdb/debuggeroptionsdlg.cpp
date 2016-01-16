/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include "debuggeroptionsdlg.h"
#ifndef CB_PRECOMP
    #include <wx/checkbox.h>
    #include <wx/choice.h>
    #include <wx/filedlg.h>
    #include <wx/intl.h>
    #include <wx/radiobox.h>
    #include <wx/spinctrl.h>
    #include <wx/textctrl.h>
    #include <wx/xrc/xmlres.h>

    #include <configmanager.h>
    #include <macrosmanager.h>
#endif

#include "debuggergdb.h"

class DebuggerConfigurationPanel : public wxPanel
{
    public:
        void ValidateExecutablePath()
        {
            wxTextCtrl *pathCtrl = XRCCTRL(*this, "txtExecutablePath", wxTextCtrl);
            wxString path = pathCtrl->GetValue();
            Manager::Get()->GetMacrosManager()->ReplaceEnvVars(path);
            if (!wxFileExists(path))
            {
                pathCtrl->SetForegroundColour(*wxWHITE);
                pathCtrl->SetBackgroundColour(*wxRED);
                pathCtrl->SetToolTip(_("Full path to the debugger's executable. Executable can't be found on the filesystem!"));
            }
            else
            {
                pathCtrl->SetForegroundColour(wxNullColour);
                pathCtrl->SetBackgroundColour(wxNullColour);
                pathCtrl->SetToolTip(_("Full path to the debugger's executable."));
            }
            pathCtrl->Refresh();
        }
    private:
        void OnBrowse(cb_unused wxCommandEvent &event)
        {
            wxString oldPath = XRCCTRL(*this, "txtExecutablePath", wxTextCtrl)->GetValue();
            Manager::Get()->GetMacrosManager()->ReplaceEnvVars(oldPath);
            wxFileDialog dlg(this, _("Select executable file"), wxEmptyString, oldPath,
                             wxFileSelectorDefaultWildcardStr, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
            PlaceWindow(&dlg);
            if (dlg.ShowModal() == wxID_OK)
            {
                wxString newPath = dlg.GetPath();
                XRCCTRL(*this, "txtExecutablePath", wxTextCtrl)->ChangeValue(newPath);
            }
        }

        void OnTextChange(cb_unused wxCommandEvent &event)
        {
            ValidateExecutablePath();
        }
    private:
        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(DebuggerConfigurationPanel, wxPanel)
    EVT_BUTTON(XRCID("btnBrowse"), DebuggerConfigurationPanel::OnBrowse)
    EVT_TEXT(XRCID("txtExecutablePath"), DebuggerConfigurationPanel::OnTextChange)
END_EVENT_TABLE()

DebuggerConfiguration::DebuggerConfiguration(const ConfigManagerWrapper &config) : cbDebuggerConfiguration(config)
{
}

cbDebuggerConfiguration* DebuggerConfiguration::Clone() const
{
    return new DebuggerConfiguration(*this);
}

wxPanel* DebuggerConfiguration::MakePanel(wxWindow *parent)
{
    DebuggerConfigurationPanel *panel = new DebuggerConfigurationPanel;
    if (!wxXmlResource::Get()->LoadPanel(panel, parent, wxT("dlgDebuggerOptions")))
        return panel;

    XRCCTRL(*panel, "txtExecutablePath", wxTextCtrl)->ChangeValue(GetDebuggerExecutable(false));
    panel->ValidateExecutablePath();
    XRCCTRL(*panel, "chkDisableInit", wxCheckBox)->SetValue(GetFlag(DisableInit));
    XRCCTRL(*panel, "txtArguments", wxTextCtrl)->ChangeValue(GetUserArguments(false));

    XRCCTRL(*panel, "rbType",            wxRadioBox)->SetSelection(IsGDB() ? 0 : 1);
    XRCCTRL(*panel, "txtInit",           wxTextCtrl)->ChangeValue(GetInitCommands());
    XRCCTRL(*panel, "txtInit",           wxTextCtrl)->SetMinSize(wxSize(-1, 75));;
    XRCCTRL(*panel, "chkWatchArgs",      wxCheckBox)->SetValue(GetFlag(WatchFuncArgs));
    XRCCTRL(*panel, "chkWatchLocals",    wxCheckBox)->SetValue(GetFlag(WatchLocals));
    XRCCTRL(*panel, "chkWatchScriptPrinters", wxCheckBox)->SetValue(GetFlag(WatchScriptPrinters));
    XRCCTRL(*panel, "chkCatchExceptions",wxCheckBox)->SetValue(GetFlag(CatchExceptions));
    XRCCTRL(*panel, "chkTooltipEval",    wxCheckBox)->SetValue(GetFlag(EvalExpression));
    XRCCTRL(*panel, "chkAddForeignDirs", wxCheckBox)->SetValue(GetFlag(AddOtherProjectDirs));
    XRCCTRL(*panel, "chkDoNotRun",       wxCheckBox)->SetValue(GetFlag(DoNotRun));
    XRCCTRL(*panel, "choDisassemblyFlavor", wxChoice)->SetSelection(m_config.ReadInt(wxT("disassembly_flavor"), 0));
    XRCCTRL(*panel, "txtInstructionSet", wxTextCtrl)->ChangeValue(m_config.Read(wxT("instruction_set"), wxEmptyString));
    return panel;
}

bool DebuggerConfiguration::SaveChanges(wxPanel *panel)
{
    m_config.Write(wxT("executable_path"),       XRCCTRL(*panel, "txtExecutablePath", wxTextCtrl)->GetValue());
    m_config.Write(wxT("disable_init"),          XRCCTRL(*panel, "chkDisableInit",    wxCheckBox)->GetValue());
    m_config.Write(wxT("user_arguments"),        XRCCTRL(*panel, "txtArguments",      wxTextCtrl)->GetValue());
    m_config.Write(wxT("type"),                  XRCCTRL(*panel, "rbType",            wxRadioBox)->GetSelection());
    m_config.Write(wxT("init_commands"),         XRCCTRL(*panel, "txtInit",           wxTextCtrl)->GetValue());
    m_config.Write(wxT("watch_args"),            XRCCTRL(*panel, "chkWatchArgs",      wxCheckBox)->GetValue());
    m_config.Write(wxT("watch_locals"),          XRCCTRL(*panel, "chkWatchLocals",    wxCheckBox)->GetValue());
    m_config.Write(wxT("watch_script_printers"), XRCCTRL(*panel, "chkWatchScriptPrinters", wxCheckBox)->GetValue());
    m_config.Write(wxT("catch_exceptions"),      XRCCTRL(*panel, "chkCatchExceptions",wxCheckBox)->GetValue());
    m_config.Write(wxT("eval_tooltip"),          XRCCTRL(*panel, "chkTooltipEval",    wxCheckBox)->GetValue());
    m_config.Write(wxT("add_other_search_dirs"), XRCCTRL(*panel, "chkAddForeignDirs", wxCheckBox)->GetValue());
    m_config.Write(wxT("do_not_run"),            XRCCTRL(*panel, "chkDoNotRun",       wxCheckBox)->GetValue());
    m_config.Write(wxT("disassembly_flavor"),    XRCCTRL(*panel, "choDisassemblyFlavor", wxChoice)->GetSelection());
    m_config.Write(wxT("instruction_set"),       XRCCTRL(*panel, "txtInstructionSet", wxTextCtrl)->GetValue());

    return true;
}

bool DebuggerConfiguration::GetFlag(Flags flag)
{
    switch (flag)
    {
        case DisableInit:
            return m_config.ReadBool(wxT("disable_init"), true);
        case WatchFuncArgs:
            return m_config.ReadBool(wxT("watch_args"), true);
        case WatchLocals:
            return m_config.ReadBool(wxT("watch_locals"), true);
        case WatchScriptPrinters:
            return m_config.ReadBool(wxT("watch_script_printers"), true);
        case CatchExceptions:
            return m_config.ReadBool(wxT("catch_exceptions"), true);
        case EvalExpression:
            return m_config.ReadBool(wxT("eval_tooltip"), false);
        case AddOtherProjectDirs:
            return m_config.ReadBool(wxT("add_other_search_dirs"), false);
        case DoNotRun:
            return m_config.ReadBool(wxT("do_not_run"), false);
        default:
            return false;
    }
}
void DebuggerConfiguration::SetFlag(Flags flag, bool value)
{
    switch (flag)
    {
        case DisableInit:
            m_config.Write(wxT("disable_init"), value);
            break;
        case WatchFuncArgs:
            m_config.Write(wxT("watch_args"), value);
            break;
        case WatchLocals:
            m_config.Write(wxT("watch_locals"), value);
            break;
        case WatchScriptPrinters:
            m_config.Write(wxT("watch_script_printers"), value);
            break;
        case CatchExceptions:
            m_config.Write(wxT("catch_exceptions"), value);
            break;
        case EvalExpression:
            m_config.Write(wxT("eval_tooltip"), value);
            break;
        case AddOtherProjectDirs:
            m_config.Write(wxT("add_other_search_dirs"), value);
            break;
        case DoNotRun:
            m_config.Write(wxT("do_not_run"), value);
            break;
        default:
            ;
    }
}

bool DebuggerConfiguration::IsGDB()
{
    return m_config.ReadInt(wxT("type"), 0) == 0;
}

wxString DebuggerConfiguration::GetDebuggerExecutable(bool expandMacro)
{
    wxString result = m_config.Read(wxT("executable_path"), wxEmptyString);
    if (expandMacro)
        Manager::Get()->GetMacrosManager()->ReplaceEnvVars(result);
    return !result.empty() ? result : cbDetectDebuggerExecutable(wxT("gdb"));
}

wxString DebuggerConfiguration::GetUserArguments(bool expandMacro)
{
    wxString result = m_config.Read(wxT("user_arguments"), wxEmptyString);
    if (expandMacro)
        Manager::Get()->GetMacrosManager()->ReplaceEnvVars(result);
    return result;
}

wxString DebuggerConfiguration::GetDisassemblyFlavorCommand()
{
    int disassembly_flavour = m_config.ReadInt(wxT("disassembly_flavor"), 0);

    wxString flavour = wxT("set disassembly-flavor ");
    switch (disassembly_flavour)
    {
        case 1: // AT & T
        {
            flavour << wxT("att");
            break;
        }
        case 2: // Intel
        {
            flavour << wxT("intel");
            break;
        }
        case 3: // Custom
        {
            wxString instruction_set = m_config.Read(wxT("instruction_set"), wxEmptyString);
            flavour << instruction_set;
            break;
        }
        default: // including case 0: // System default

        if(platform::windows)
            flavour << wxT("att");
        else
            flavour << wxT("intel");
    }// switch
    return flavour;
}

wxString DebuggerConfiguration::GetInitCommands()
{
    return m_config.Read(wxT("init_commands"), wxEmptyString);
}
