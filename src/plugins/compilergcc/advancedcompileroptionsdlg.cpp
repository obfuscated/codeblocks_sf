#include "advancedcompileroptionsdlg.h"
#include <compilerfactory.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/listbox.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>

BEGIN_EVENT_TABLE(AdvancedCompilerOptionsDlg, wxDialog)
    EVT_LISTBOX(XRCID("lstCommands"), AdvancedCompilerOptionsDlg::OnCommandsChange)
END_EVENT_TABLE()

AdvancedCompilerOptionsDlg::AdvancedCompilerOptionsDlg(wxWindow* parent, int compilerIdx)
    : m_CompilerIdx(compilerIdx),
    m_LastCmdIndex(-1)
{
	//ctor
	wxXmlResource::Get()->LoadDialog(this, parent, _("dlgAdvancedCompilerOptions"));
	ReadCompilerOptions();
}

AdvancedCompilerOptionsDlg::~AdvancedCompilerOptionsDlg()
{
	//dtor
}

void AdvancedCompilerOptionsDlg::ReadCompilerOptions()
{
    wxListBox* lst = XRCCTRL(*this, "lstCommands", wxListBox);
    lst->Clear();
    for (int i = 0; i < COMPILER_COMMAND_TYPES_COUNT; ++i)
    {
        m_Commands[i] = CompilerFactory::Compilers[m_CompilerIdx]->GetCommand((CommandType)i);
        m_Commands[i].Replace("\t", "");
        lst->Append(Compiler::CommandTypeDescriptions[i]);
    }
    lst->SetSelection(0);
    DisplayCommand(0);
    
    // switches
    const CompilerSwitches& switches = CompilerFactory::Compilers[m_CompilerIdx]->GetSwitches();
    XRCCTRL(*this, "txtAddIncludePath", wxTextCtrl)->SetValue(switches.includeDirs);
    XRCCTRL(*this, "txtAddLibPath", wxTextCtrl)->SetValue(switches.libDirs);
    XRCCTRL(*this, "txtAddLib", wxTextCtrl)->SetValue(switches.linkLibs);
    XRCCTRL(*this, "txtDefine", wxTextCtrl)->SetValue(switches.defines);
    XRCCTRL(*this, "txtGenericSwitch", wxTextCtrl)->SetValue(switches.genericSwitch);
    XRCCTRL(*this, "txtLinkForGui", wxTextCtrl)->SetValue(switches.linkerSwitchForGui);
    XRCCTRL(*this, "txtObjectExt", wxTextCtrl)->SetValue(switches.objectExtension);
    XRCCTRL(*this, "chkNeedDeps", wxCheckBox)->SetValue(switches.needDependencies);
    XRCCTRL(*this, "chkForceCompilerQuotes", wxCheckBox)->SetValue(switches.forceCompilerUseQuotes);
    XRCCTRL(*this, "chkForceLinkerQuotes", wxCheckBox)->SetValue(switches.forceLinkerUseQuotes);
}

void AdvancedCompilerOptionsDlg::WriteCompilerOptions()
{
    for (int i = 0; i < COMPILER_COMMAND_TYPES_COUNT; ++i)
    {
        m_Commands[i].Replace("\n", "\n\t");
        CompilerFactory::Compilers[m_CompilerIdx]->SetCommand((CommandType)i, m_Commands[i]);
    }

    // switches
    CompilerSwitches switches;
    switches.includeDirs = XRCCTRL(*this, "txtAddIncludePath", wxTextCtrl)->GetValue();
    switches.libDirs = XRCCTRL(*this, "txtAddLibPath", wxTextCtrl)->GetValue();
    switches.linkLibs = XRCCTRL(*this, "txtAddLib", wxTextCtrl)->GetValue();
    switches.defines = XRCCTRL(*this, "txtDefine", wxTextCtrl)->GetValue();
    switches.genericSwitch = XRCCTRL(*this, "txtGenericSwitch", wxTextCtrl)->GetValue();
    switches.linkerSwitchForGui = XRCCTRL(*this, "txtLinkForGui", wxTextCtrl)->GetValue();
    switches.objectExtension = XRCCTRL(*this, "txtObjectExt", wxTextCtrl)->GetValue();
    switches.needDependencies = XRCCTRL(*this, "chkNeedDeps", wxCheckBox)->GetValue();
    switches.forceCompilerUseQuotes = XRCCTRL(*this, "chkForceCompilerQuotes", wxCheckBox)->GetValue();
    switches.forceLinkerUseQuotes = XRCCTRL(*this, "chkForceLinkerQuotes", wxCheckBox)->GetValue();
    CompilerFactory::Compilers[m_CompilerIdx]->SetSwitches(switches);
}

void AdvancedCompilerOptionsDlg::DisplayCommand(int nr)
{
    wxTextCtrl* text = XRCCTRL(*this, "txtCommand", wxTextCtrl);
    if (m_LastCmdIndex != -1)
    {
        if (text->GetValue() != m_Commands[m_LastCmdIndex])
        {
            // last command was changed; save it
            m_Commands[m_LastCmdIndex] = text->GetValue();
        }
    }
    
    text->SetValue(m_Commands[nr]);
    m_LastCmdIndex = nr;
}

void AdvancedCompilerOptionsDlg::OnCommandsChange(wxCommandEvent& event)
{
    DisplayCommand(XRCCTRL(*this, "lstCommands", wxListBox)->GetSelection());
}

void AdvancedCompilerOptionsDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK)
    {
        // make sure we update the first command, if it changed
        DisplayCommand(m_LastCmdIndex);
        // write options
        WriteCompilerOptions();
    }
    wxDialog::EndModal(retCode);
}
