#include "dlgformattersettings.h"
#include <wx/radiobut.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/spinctrl.h>
#include <wx/xrc/xmlres.h>

dlgFormatterSettings::dlgFormatterSettings(wxWindow *dlg)
: m_dlg(dlg)
{
	//ctor
}

dlgFormatterSettings::~dlgFormatterSettings()
{
	//dtor
}

void dlgFormatterSettings::ApplyTo(astyle::ASFormatter& formatter)
{
  int style = 0;

  if (XRCCTRL(*m_dlg, "rbAnsi", wxRadioButton)->GetValue())
    style = 0;
  else if (XRCCTRL(*m_dlg, "rbKr", wxRadioButton)->GetValue())
    style = 1;
  else if (XRCCTRL(*m_dlg, "rbLinux", wxRadioButton)->GetValue())
    style = 2;
  else if (XRCCTRL(*m_dlg, "rbGNU", wxRadioButton)->GetValue())
    style = 3;
  else if (XRCCTRL(*m_dlg, "rbJava", wxRadioButton)->GetValue())
    style = 4;
  else if (XRCCTRL(*m_dlg, "rbCustom", wxRadioButton)->GetValue())
    style = 5;

  switch(style)
  {
    case 0: // ansi
      formatter.setBracketIndent(false);
      formatter.setTabIndentation(4);
      formatter.setSpaceIndentation(4);
      formatter.setBracketFormatMode(astyle::BREAK_MODE);
      formatter.setClassIndent(false);
      formatter.setSwitchIndent(false);
      formatter.setNamespaceIndent(true);
      formatter.setBlockIndent(false);
      formatter.setBreakBlocksMode(false);
      formatter.setBreakElseIfsMode(false);
      formatter.setOperatorPaddingMode(false);
      formatter.setParensOutsidePaddingMode(false);
      formatter.setParensInsidePaddingMode(false);
      formatter.setSingleStatementsMode(true);
      formatter.setBreakOneLineBlocksMode(true);
      break;

    case 1: // K&R
      formatter.setBracketIndent(false);
      formatter.setTabIndentation(4);
      formatter.setSpaceIndentation(4);
      formatter.setBracketFormatMode(astyle::ATTACH_MODE);
      formatter.setClassIndent(false);
      formatter.setSwitchIndent(false);
      formatter.setNamespaceIndent(true);
      formatter.setBlockIndent(false);
      formatter.setBreakBlocksMode(false);
      formatter.setBreakElseIfsMode(false);
      formatter.setOperatorPaddingMode(false);
      formatter.setParensOutsidePaddingMode(false);
      formatter.setParensInsidePaddingMode(false);
      formatter.setSingleStatementsMode(true);
      formatter.setBreakOneLineBlocksMode(true);
      break;

    case 2: // Linux
      formatter.setBracketIndent(false);
      formatter.setTabIndentation(8);
      formatter.setSpaceIndentation(8);
      formatter.setBracketFormatMode(astyle::BDAC_MODE);
      formatter.setClassIndent(false);
      formatter.setSwitchIndent(false);
      formatter.setNamespaceIndent(true);
      formatter.setBlockIndent(false);
      formatter.setBreakBlocksMode(false);
      formatter.setBreakElseIfsMode(false);
      formatter.setOperatorPaddingMode(false);
      formatter.setParensOutsidePaddingMode(false);
      formatter.setParensInsidePaddingMode(false);
      formatter.setSingleStatementsMode(true);
      formatter.setBreakOneLineBlocksMode(true);
      break;

    case 3: // GNU
      formatter.setBracketIndent(false);
      formatter.setTabIndentation(2);
      formatter.setSpaceIndentation(2);
      formatter.setBracketFormatMode(astyle::BREAK_MODE);
      formatter.setClassIndent(false);
      formatter.setSwitchIndent(false);
      formatter.setNamespaceIndent(true);
      formatter.setBlockIndent(true);
      formatter.setBreakBlocksMode(false);
      formatter.setBreakElseIfsMode(false);
      formatter.setOperatorPaddingMode(false);
      formatter.setParensOutsidePaddingMode(false);
      formatter.setParensInsidePaddingMode(false);
      formatter.setSingleStatementsMode(true);
      formatter.setBreakOneLineBlocksMode(true);
      break;

    case 4: // Java
      formatter.setJavaStyle();
      //formatter.modeSetManually = true;
      formatter.setBracketIndent(false);
      formatter.setTabIndentation(4);
      formatter.setSpaceIndentation(4);
      formatter.setBracketFormatMode(astyle::ATTACH_MODE);
      //formatter.setClassIndent(false);
      formatter.setSwitchIndent(false);
      //formatter.setNamespaceIndent(true);
      formatter.setBlockIndent(false);
      formatter.setBreakBlocksMode(false);
      formatter.setBreakElseIfsMode(false);
      formatter.setOperatorPaddingMode(false);
      formatter.setParensInsidePaddingMode(false);
      formatter.setParensOutsidePaddingMode(false);
      formatter.setSingleStatementsMode(true);
      formatter.setBreakOneLineBlocksMode(true);
      break;

    default: // Custom
    {
      bool value = XRCCTRL(*m_dlg, "chkForceUseTabs", wxCheckBox)->GetValue();
      int spaceNum = XRCCTRL(*m_dlg, "spnIndentation", wxSpinCtrl)->GetValue();

      if (XRCCTRL(*m_dlg, "chkUseTab", wxCheckBox)->GetValue())
      {
        formatter.setTabIndentation(spaceNum, value);
      }
      else
      {
        formatter.setSpaceIndentation(spaceNum);
      }

      formatter.setTabSpaceConversionMode(XRCCTRL(*m_dlg, "chkConvertTabs", wxCheckBox)->GetValue());
      formatter.setEmptyLineFill(XRCCTRL(*m_dlg, "chkFillEmptyLines", wxCheckBox)->GetValue());
      formatter.setClassIndent(XRCCTRL(*m_dlg, "chkIndentClasses", wxCheckBox)->GetValue());
      formatter.setSwitchIndent(XRCCTRL(*m_dlg, "chkIndentSwitches", wxCheckBox)->GetValue());
      formatter.setCaseIndent(XRCCTRL(*m_dlg, "chkIndentCase", wxCheckBox)->GetValue());
      formatter.setBracketIndent(XRCCTRL(*m_dlg, "chkIndentBrackets", wxCheckBox)->GetValue());
      formatter.setBlockIndent(XRCCTRL(*m_dlg, "chkIndentBlocks", wxCheckBox)->GetValue());
      formatter.setNamespaceIndent(XRCCTRL(*m_dlg, "chkIndentNamespaces", wxCheckBox)->GetValue());
      formatter.setLabelIndent(XRCCTRL(*m_dlg, "chkIndentLabels", wxCheckBox)->GetValue());
      formatter.setPreprocessorIndent(XRCCTRL(*m_dlg, "chkIndentPreprocessor", wxCheckBox)->GetValue());

      wxString breakType = XRCCTRL(*m_dlg, "cmbBreakType", wxComboBox)->GetValue();

      if (breakType == _T("Break"))
      {
        formatter.setBracketFormatMode(astyle::BREAK_MODE);
      }
      else if (breakType == _T("Attach"))
      {
        formatter.setBracketFormatMode(astyle::ATTACH_MODE);
      }
      else if (breakType == _T("Linux"))
      {
        formatter.setBracketFormatMode(astyle::BDAC_MODE);
      }
      else
      {
        formatter.setBracketFormatMode(astyle::NONE_MODE);
      }

      formatter.setBreakBlocksMode(XRCCTRL(*m_dlg, "chkBreakBlocks", wxCheckBox)->GetValue());
      formatter.setBreakElseIfsMode(XRCCTRL(*m_dlg, "chkBreakElseIfs", wxCheckBox)->GetValue());
      formatter.setOperatorPaddingMode(XRCCTRL(*m_dlg, "chkPadOperators", wxCheckBox)->GetValue());
      formatter.setParensOutsidePaddingMode(XRCCTRL(*m_dlg, "chkPadParensOut", wxCheckBox)->GetValue());
      formatter.setParensInsidePaddingMode(XRCCTRL(*m_dlg, "chkPadParensIn", wxCheckBox)->GetValue());
      formatter.setSingleStatementsMode(!XRCCTRL(*m_dlg, "chkKeepComplex", wxCheckBox)->GetValue());
      formatter.setBreakOneLineBlocksMode(!XRCCTRL(*m_dlg, "chkKeepBlocks", wxCheckBox)->GetValue());
      break;
    }
  }
}
