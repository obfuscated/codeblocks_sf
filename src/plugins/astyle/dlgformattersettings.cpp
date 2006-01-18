#include "dlgformattersettings.h"
#include <wx/radiobut.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/spinctrl.h>
#include <wx/xrc/xmlres.h>

dlgFormatterSettings::dlgFormatterSettings(wxDialog *dlg)
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
      formatter.bracketIndent = false;
      formatter.indentLength = 4;
      formatter.indentString = "    ";
      formatter.bracketFormatMode = astyle::BREAK_MODE;
      formatter.classIndent = false;
      formatter.switchIndent = false;
      formatter.namespaceIndent = true;
      formatter.blockIndent = false;
      formatter.breakBlocks = false;
      formatter.breakElseIfs = false;
      formatter.padOperators = false;
      formatter.padParen = false;
      formatter.breakOneLineStatements = true;
      formatter.breakOneLineBlocks = true;
      break;

    case 1: // K&R
      formatter.bracketIndent = false;
      formatter.indentLength = 4;
      formatter.indentString = "    ";
      formatter.bracketFormatMode = astyle::ATTACH_MODE;
      formatter.classIndent = false;
      formatter.switchIndent = false;
      formatter.namespaceIndent = true;
      formatter.blockIndent = false;
      formatter.breakBlocks = false;
      formatter.breakElseIfs = false;
      formatter.padOperators = false;
      formatter.padParen = false;
      formatter.breakOneLineStatements = true;
      formatter.breakOneLineBlocks = true;
      break;

    case 2: // Linux
      formatter.bracketIndent = false;
      formatter.indentLength = 8;
      formatter.indentString = "        ";
      formatter.bracketFormatMode = astyle::BDAC_MODE;
      formatter.classIndent = false;
      formatter.switchIndent = false;
      formatter.namespaceIndent = true;
      formatter.blockIndent = false;
      formatter.breakBlocks = false;
      formatter.breakElseIfs = false;
      formatter.padOperators = false;
      formatter.padParen = false;
      formatter.breakOneLineStatements = true;
      formatter.breakOneLineBlocks = true;
      break;

    case 3: // GNU
      formatter.blockIndent = true;
      formatter.bracketIndent = false;
      formatter.indentLength = 2;
      formatter.indentString = "  ";
      formatter.bracketFormatMode = astyle::BREAK_MODE;
      formatter.classIndent = false;
      formatter.switchIndent = false;
      formatter.namespaceIndent = false;
      formatter.breakBlocks = false;
      formatter.breakElseIfs = false;
      formatter.padOperators = false;
      formatter.padParen = false;
      formatter.breakOneLineStatements = true;
      formatter.breakOneLineBlocks = true;
      break;

    case 4: // Java
      formatter.sourceStyle = astyle::STYLE_JAVA;
      formatter.modeSetManually = true;
      formatter.bracketIndent = false;
      formatter.indentLength = 4;
      formatter.indentString = "    ";
      formatter.bracketFormatMode = astyle::ATTACH_MODE;
      formatter.switchIndent = false;
      formatter.blockIndent = false;
      formatter.breakBlocks = false;
      formatter.breakElseIfs = false;
      formatter.padOperators = false;
      formatter.padParen = false;
      formatter.breakOneLineStatements = true;
      formatter.breakOneLineBlocks = true;
      break;

    default: // Custom
    {
      bool value;
      int spaceNum = XRCCTRL(*m_dlg, "spnIndentation", wxSpinCtrl)->GetValue();

      formatter.modeSetManually = false;
      formatter.indentLength = spaceNum;

      if (XRCCTRL(*m_dlg, "chkUseTab", wxCheckBox)->GetValue())
      {
        formatter.indentString = '\t';
      }
      else
      {
        formatter.indentString = string(spaceNum, ' ');
      }

      value = XRCCTRL(*m_dlg, "chkForceUseTabs", wxCheckBox)->GetValue();

      if (value)
      {
        formatter.indentString = '\t';
        formatter.forceTabIndent = true;
      }
      else
      {
        formatter.forceTabIndent = false;
      }

      formatter.convertTabs2Space = XRCCTRL(*m_dlg, "chkConvertTabs", wxCheckBox)->GetValue();
      formatter.emptyLineIndent = XRCCTRL(*m_dlg, "chkFillEmptyLines", wxCheckBox)->GetValue();
      formatter.classIndent = XRCCTRL(*m_dlg, "chkIndentClasses", wxCheckBox)->GetValue();
      formatter.switchIndent = XRCCTRL(*m_dlg, "chkIndentSwitches", wxCheckBox)->GetValue();
      formatter.caseIndent = XRCCTRL(*m_dlg, "chkIndentCase", wxCheckBox)->GetValue();
      formatter.bracketIndent = XRCCTRL(*m_dlg, "chkIndentBrackets", wxCheckBox)->GetValue();
      formatter.blockIndent = XRCCTRL(*m_dlg, "chkIndentBlocks", wxCheckBox)->GetValue();
      formatter.namespaceIndent = XRCCTRL(*m_dlg, "chkIndentNamespaces", wxCheckBox)->GetValue();
      formatter.labelIndent = XRCCTRL(*m_dlg, "chkIndentLabels", wxCheckBox)->GetValue();
      formatter.preprocessorIndent = XRCCTRL(*m_dlg, "chkIndentPreprocessor", wxCheckBox)->GetValue();

      wxString breakType = XRCCTRL(*m_dlg, "cmbBreakType", wxComboBox)->GetValue();

      if (breakType == _T("Break"))
      {
        formatter.bracketFormatMode = astyle::BREAK_MODE;
      }
      else if (breakType == _T("Attach"))
      {
        formatter.bracketFormatMode = astyle::ATTACH_MODE;
      }
      else if (breakType == _T("Linux"))
      {
        formatter.bracketFormatMode = astyle::BDAC_MODE;
      }
      else
      {
        formatter.bracketFormatMode = astyle::NONE_MODE;
      }

      formatter.breakBlocks = XRCCTRL(*m_dlg, "chkBreakBlocks", wxCheckBox)->GetValue();
      formatter.breakElseIfs = XRCCTRL(*m_dlg, "chkBreakElseIfs", wxCheckBox)->GetValue();
      formatter.padOperators = XRCCTRL(*m_dlg, "chkPadOperators", wxCheckBox)->GetValue();
      formatter.padParen = XRCCTRL(*m_dlg, "chkPadParens", wxCheckBox)->GetValue();
      formatter.breakOneLineStatements = !XRCCTRL(*m_dlg, "chkKeepComplex", wxCheckBox)->GetValue();
      formatter.breakOneLineBlocks = !XRCCTRL(*m_dlg, "chkKeepBlocks", wxCheckBox)->GetValue();
      break;
    }
  }
}
