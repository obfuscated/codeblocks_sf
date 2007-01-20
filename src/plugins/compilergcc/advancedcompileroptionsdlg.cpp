#include "sdk.h"
#ifndef CB_PRECOMP
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/intl.h>
#include <wx/listbox.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include <wx/xrc/xmlres.h>
#include "compilerfactory.h"
#include "globals.h"
#endif
#include "advancedcompileroptionsdlg.h"

// TODO : implement full IsDirty mechanism, now always forced to true when ok is pressed
// NOTE : when OK pressed settings are already applied to the compiler, although in the compiler settings
//		that the parent dialog that spawned this dialog the user can still click on cancel there,
//		meaning also the changes here should not be applied, but as it is now : too late, applied (TO FIX)


wxString ControlCharsToString(const wxString& src)
{
    wxString ret = src;
    ret.Replace(_T("\t"), _T("\\t"));
    ret.Replace(_T("\n"), _T("\\n"));
    ret.Replace(_T("\r"), _T("\\r"));
    ret.Replace(_T("\a"), _T("\\a"));
    ret.Replace(_T("\b"), _T("\\b"));
    return ret;
} // end of ControlCharsToString

wxString StringToControlChars(const wxString& src)
{
    wxString ret = src;
    ret.Replace(_T("\\t"), _T("\t"));
    ret.Replace(_T("\\n"), _T("\n"));
    ret.Replace(_T("\\r"), _T("\r"));
    ret.Replace(_T("\\a"), _T("\a"));
    ret.Replace(_T("\\b"), _T("\b"));
    return ret;
} // end of StringToControlChars

BEGIN_EVENT_TABLE(AdvancedCompilerOptionsDlg, wxDialog)
    EVT_LISTBOX(XRCID("lstCommands"),       AdvancedCompilerOptionsDlg::OnCommandsChange)
    EVT_LISTBOX(XRCID("lstRegex"),          AdvancedCompilerOptionsDlg::OnRegexChange)
    EVT_BUTTON(XRCID("btnRegexTest"),       AdvancedCompilerOptionsDlg::OnRegexTest)
    EVT_BUTTON(XRCID("btnRegexAdd"),        AdvancedCompilerOptionsDlg::OnRegexAdd)
    EVT_BUTTON(XRCID("btnRegexDelete"),     AdvancedCompilerOptionsDlg::OnRegexDelete)
    EVT_BUTTON(XRCID("btnRegexDefaults"),   AdvancedCompilerOptionsDlg::OnRegexDefaults)
    EVT_SPIN_UP(XRCID("spnRegexOrder"),     AdvancedCompilerOptionsDlg::OnRegexUp)
    EVT_SPIN_DOWN(XRCID("spnRegexOrder"),   AdvancedCompilerOptionsDlg::OnRegexDown)
END_EVENT_TABLE()

AdvancedCompilerOptionsDlg::AdvancedCompilerOptionsDlg(wxWindow* parent, const wxString& compilerId)
    : m_CompilerId(compilerId),
    m_LastCmdIndex(-1)
{
	//ctor
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgAdvancedCompilerOptions"));
	ReadCompilerOptions();
	m_bDirty = false;
} // end of constructor

AdvancedCompilerOptionsDlg::~AdvancedCompilerOptionsDlg()
{
	//dtor
}

void AdvancedCompilerOptionsDlg::ReadCompilerOptions()
{
    Compiler* compiler = CompilerFactory::GetCompiler(m_CompilerId);

    wxListBox* lst = XRCCTRL(*this, "lstCommands", wxListBox);
    lst->Clear();
    for (int i = 0; i < COMPILER_COMMAND_TYPES_COUNT; ++i)
    {
        m_Commands[i] = compiler->GetCommand((CommandType)i);
        m_Commands[i].Replace(_T("\t"), _T(""));
        lst->Append(Compiler::CommandTypeDescriptions[i]);
    }
    lst->SetSelection(0);
    DisplayCommand(0);

    // switches
    const CompilerSwitches& switches = compiler->GetSwitches();
    XRCCTRL(*this, "txtAddIncludePath", wxTextCtrl)->SetValue(switches.includeDirs);
    XRCCTRL(*this, "txtAddLibPath", wxTextCtrl)->SetValue(switches.libDirs);
    XRCCTRL(*this, "txtAddLib", wxTextCtrl)->SetValue(switches.linkLibs);
    XRCCTRL(*this, "txtLibPrefix", wxTextCtrl)->SetValue(switches.libPrefix);
    XRCCTRL(*this, "txtLibExt", wxTextCtrl)->SetValue(switches.libExtension);
    XRCCTRL(*this, "txtDefine", wxTextCtrl)->SetValue(switches.defines);
    XRCCTRL(*this, "txtGenericSwitch", wxTextCtrl)->SetValue(switches.genericSwitch);
    XRCCTRL(*this, "txtObjectExt", wxTextCtrl)->SetValue(switches.objectExtension);
    XRCCTRL(*this, "chkFwdSlashes", wxCheckBox)->SetValue(switches.forceFwdSlashes);
    XRCCTRL(*this, "chkLinkerNeedsLibPrefix", wxCheckBox)->SetValue(switches.linkerNeedsLibPrefix);
    XRCCTRL(*this, "chkLinkerNeedsLibExt", wxCheckBox)->SetValue(switches.linkerNeedsLibExtension);
    XRCCTRL(*this, "chkNeedDeps", wxCheckBox)->SetValue(switches.needDependencies);
    XRCCTRL(*this, "chkForceCompilerQuotes", wxCheckBox)->SetValue(switches.forceCompilerUseQuotes);
    XRCCTRL(*this, "chkForceLinkerQuotes", wxCheckBox)->SetValue(switches.forceLinkerUseQuotes);
    XRCCTRL(*this, "chkSupportsPCH", wxCheckBox)->SetValue(switches.supportsPCH);
    XRCCTRL(*this, "txtPCHExt", wxTextCtrl)->SetValue(switches.PCHExtension);
    XRCCTRL(*this, "chkUseFlatObjects", wxCheckBox)->SetValue(switches.UseFlatObjects);
    XRCCTRL(*this, "chkUseFullSourcePaths", wxCheckBox)->SetValue(switches.UseFullSourcePaths);

    m_Regexes = compiler->GetRegExArray();
    m_SelectedRegex = m_Regexes.Count() > 0 ? 0 : -1;
    FillRegexes();
} // end of ReadCompilerOptions

void AdvancedCompilerOptionsDlg::WriteCompilerOptions()
{
    Compiler* compiler = CompilerFactory::GetCompiler(m_CompilerId);

    for (int i = 0; i < COMPILER_COMMAND_TYPES_COUNT; ++i)
    {
        m_Commands[i].Replace(_T("\n"), _T("\n\t"));
        compiler->SetCommand((CommandType)i, m_Commands[i]);
    }

    // switches
    CompilerSwitches switches;
    switches.includeDirs = XRCCTRL(*this, "txtAddIncludePath", wxTextCtrl)->GetValue();
    switches.libDirs = XRCCTRL(*this, "txtAddLibPath", wxTextCtrl)->GetValue();
    switches.linkLibs = XRCCTRL(*this, "txtAddLib", wxTextCtrl)->GetValue();
    switches.libPrefix = XRCCTRL(*this, "txtLibPrefix", wxTextCtrl)->GetValue();
    switches.libExtension = XRCCTRL(*this, "txtLibExt", wxTextCtrl)->GetValue();
    switches.defines = XRCCTRL(*this, "txtDefine", wxTextCtrl)->GetValue();
    switches.genericSwitch = XRCCTRL(*this, "txtGenericSwitch", wxTextCtrl)->GetValue();
    switches.objectExtension = XRCCTRL(*this, "txtObjectExt", wxTextCtrl)->GetValue();
    switches.forceFwdSlashes = XRCCTRL(*this, "chkFwdSlashes", wxCheckBox)->GetValue();
    switches.linkerNeedsLibPrefix = XRCCTRL(*this, "chkLinkerNeedsLibPrefix", wxCheckBox)->GetValue();
    switches.linkerNeedsLibExtension = XRCCTRL(*this, "chkLinkerNeedsLibExt", wxCheckBox)->GetValue();
    switches.needDependencies = XRCCTRL(*this, "chkNeedDeps", wxCheckBox)->GetValue();
    switches.forceCompilerUseQuotes = XRCCTRL(*this, "chkForceCompilerQuotes", wxCheckBox)->GetValue();
    switches.forceLinkerUseQuotes = XRCCTRL(*this, "chkForceLinkerQuotes", wxCheckBox)->GetValue();
    switches.supportsPCH = XRCCTRL(*this, "chkSupportsPCH", wxCheckBox)->GetValue();
    switches.PCHExtension = XRCCTRL(*this, "txtPCHExt", wxTextCtrl)->GetValue();
    switches.UseFlatObjects = XRCCTRL(*this, "chkUseFlatObjects", wxCheckBox)->GetValue();
    switches.UseFullSourcePaths = XRCCTRL(*this, "chkUseFullSourcePaths", wxCheckBox)->GetValue();
    compiler->SetSwitches(switches);
} // end of WriteCompilerOptions

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

void AdvancedCompilerOptionsDlg::FillRegexes()
{
    wxListBox* list = XRCCTRL(*this, "lstRegex", wxListBox);
    list->Clear();

    for (size_t i = 0; i < m_Regexes.Count(); ++i)
    {
        RegExStruct& rs = m_Regexes[i];
        list->Append(rs.desc);
    }
    list->SetSelection(m_SelectedRegex);
    FillRegexDetails(m_SelectedRegex);
}

void AdvancedCompilerOptionsDlg::FillRegexDetails(int index)
{
    if (index == -1)
    {
        XRCCTRL(*this, "txtRegexDesc", wxTextCtrl)->SetValue(_T(""));
        XRCCTRL(*this, "cmbRegexType", wxComboBox)->SetSelection(-1);
        XRCCTRL(*this, "txtRegex", wxTextCtrl)->SetValue(_T(""));
        XRCCTRL(*this, "spnRegexMsg1", wxSpinCtrl)->SetValue(0);
        XRCCTRL(*this, "spnRegexMsg2", wxSpinCtrl)->SetValue(0);
        XRCCTRL(*this, "spnRegexMsg3", wxSpinCtrl)->SetValue(0);
        XRCCTRL(*this, "spnRegexFilename", wxSpinCtrl)->SetValue(0);
        XRCCTRL(*this, "spnRegexLine", wxSpinCtrl)->SetValue(0);
        return;
    }

    RegExStruct& rs = m_Regexes[index];
    XRCCTRL(*this, "txtRegexDesc", wxTextCtrl)->SetValue(rs.desc);
    XRCCTRL(*this, "cmbRegexType", wxComboBox)->SetSelection((int)rs.lt);
    XRCCTRL(*this, "txtRegex", wxTextCtrl)->SetValue(ControlCharsToString(rs.regex));
    XRCCTRL(*this, "spnRegexMsg1", wxSpinCtrl)->SetValue(rs.msg[0]);
    XRCCTRL(*this, "spnRegexMsg2", wxSpinCtrl)->SetValue(rs.msg[1]);
    XRCCTRL(*this, "spnRegexMsg3", wxSpinCtrl)->SetValue(rs.msg[2]);
    XRCCTRL(*this, "spnRegexFilename", wxSpinCtrl)->SetValue(rs.filename);
    XRCCTRL(*this, "spnRegexLine", wxSpinCtrl)->SetValue(rs.line);
}

void AdvancedCompilerOptionsDlg::SaveRegexDetails(int index)
{
    if (index == -1)
        return;

    RegExStruct& rs = m_Regexes[index];
    rs.desc = XRCCTRL(*this, "txtRegexDesc", wxTextCtrl)->GetValue();
    rs.lt = (CompilerLineType)XRCCTRL(*this, "cmbRegexType", wxComboBox)->GetSelection();
    rs.regex = StringToControlChars(XRCCTRL(*this, "txtRegex", wxTextCtrl)->GetValue());
    rs.msg[0] = XRCCTRL(*this, "spnRegexMsg1", wxSpinCtrl)->GetValue();
    rs.msg[1] = XRCCTRL(*this, "spnRegexMsg2", wxSpinCtrl)->GetValue();
    rs.msg[2] = XRCCTRL(*this, "spnRegexMsg3", wxSpinCtrl)->GetValue();
    rs.filename = XRCCTRL(*this, "spnRegexFilename", wxSpinCtrl)->GetValue();
    rs.line = XRCCTRL(*this, "spnRegexLine", wxSpinCtrl)->GetValue();
}

void AdvancedCompilerOptionsDlg::OnCommandsChange(wxCommandEvent& event)
{
    DisplayCommand(XRCCTRL(*this, "lstCommands", wxListBox)->GetSelection());
}

void AdvancedCompilerOptionsDlg::OnRegexChange(wxCommandEvent& event)
{
    SaveRegexDetails(m_SelectedRegex);

    // update regex list, in case desc was changed
    XRCCTRL(*this, "lstRegex", wxListBox)->SetString(m_SelectedRegex, XRCCTRL(*this, "txtRegexDesc", wxTextCtrl)->GetValue());

    m_SelectedRegex = XRCCTRL(*this, "lstRegex", wxListBox)->GetSelection();
    FillRegexDetails(m_SelectedRegex);
}

void AdvancedCompilerOptionsDlg::OnRegexAdd(wxCommandEvent& event)
{
    SaveRegexDetails(m_SelectedRegex);
    m_Regexes.Add(RegExStruct(_("New regular expression"), cltError, _T(""), 0));
    m_SelectedRegex = m_Regexes.Count() - 1;
    FillRegexes();
}

void AdvancedCompilerOptionsDlg::OnRegexDelete(wxCommandEvent& event)
{
    if (cbMessageBox(_("Are you sure you want to delete this regular expression?"), _("Confirmation"), wxICON_QUESTION | wxYES_NO | wxNO_DEFAULT) == wxID_YES)
    {
        m_Regexes.RemoveAt(m_SelectedRegex);
        if (m_SelectedRegex >= (int)m_Regexes.Count())
            --m_SelectedRegex;
        FillRegexes();
    }
}

void AdvancedCompilerOptionsDlg::OnRegexDefaults(wxCommandEvent& event)
{
    if (cbMessageBox(_("Are you sure you want to load the default regular expressions "
                    "for this compiler?\n"
                    "ALL regular expressions will be erased and replaced with their default "
                    "counterparts!\n\n"
                    "Are you REALLY sure?"), _("Confirmation"), wxICON_QUESTION | wxYES_NO | wxNO_DEFAULT) == wxID_YES)
    {
        Compiler* compiler = CompilerFactory::GetCompiler(m_CompilerId);
        compiler->LoadDefaultRegExArray();
        m_Regexes = compiler->GetRegExArray();
        while (m_SelectedRegex >= (int)m_Regexes.Count())
            --m_SelectedRegex;
        FillRegexes();
    }
}

void AdvancedCompilerOptionsDlg::OnRegexUp(wxSpinEvent& event)
{
    if (m_SelectedRegex <= 0)
        return;

    RegExStruct rs = m_Regexes[m_SelectedRegex];
    m_Regexes.RemoveAt(m_SelectedRegex);
    m_Regexes.Insert(rs, m_SelectedRegex - 1);
    --m_SelectedRegex;
    FillRegexes();
}

void AdvancedCompilerOptionsDlg::OnRegexDown(wxSpinEvent& event)
{
    if (m_SelectedRegex >= (int)m_Regexes.Count() - 1)
        return;

    RegExStruct rs = m_Regexes[m_SelectedRegex];
    m_Regexes.RemoveAt(m_SelectedRegex);
    m_Regexes.Insert(rs, m_SelectedRegex + 1);
    ++m_SelectedRegex;
    FillRegexes();
}

void AdvancedCompilerOptionsDlg::OnRegexTest(wxCommandEvent& event)
{
    if (m_SelectedRegex == -1)
        return;
    wxString text = XRCCTRL(*this, "txtRegexTest", wxTextCtrl)->GetValue();
    if (text.IsEmpty())
    {
        cbMessageBox(_("Please enter a compiler line in the \"Compiler output\" text box..."), _("Error"), wxICON_ERROR);
        return;
    }

    Compiler* compiler = CompilerFactory::GetCompiler(m_CompilerId);

    // backup regexes
    RegExArray regex_copy = m_Regexes;
    SaveRegexDetails(m_SelectedRegex);

    // test-run
    compiler->SetRegExArray(m_Regexes);
    CompilerLineType clt = compiler->CheckForWarningsAndErrors(text);

    // restore regexes
    compiler->SetRegExArray(regex_copy);
    m_Regexes = regex_copy;

    wxString msg;
    msg.Printf(_("Regular expression analyzed as follows:\n\n"
                "Type: %s message\n"
                "Filename: %s\n"
                "Line number: %s\n"
                "Message: %s"),
                clt == cltNormal ? _("Info") : (clt == cltError ? _("Error") : _("Warning")),
                compiler->GetLastErrorFilename().c_str(),
                compiler->GetLastErrorLine().c_str(),
                compiler->GetLastError().c_str()
                );

    cbMessageBox(msg, _("Test results"), wxICON_INFORMATION);
}

void AdvancedCompilerOptionsDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK)
    {
    	m_bDirty = true;
        Compiler* compiler = CompilerFactory::GetCompiler(m_CompilerId);

        // make sure we update the first command, if it changed
        DisplayCommand(m_LastCmdIndex);
        // write options
        WriteCompilerOptions();
        // save regexes
        SaveRegexDetails(m_SelectedRegex);
        compiler->SetRegExArray(m_Regexes);
    }
    wxDialog::EndModal(retCode);
}
