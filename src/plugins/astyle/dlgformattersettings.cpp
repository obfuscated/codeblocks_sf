/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

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

  if (XRCCTRL(*m_dlg, "rbAllman", wxRadioButton)->GetValue())
    style = 0;
  else if (XRCCTRL(*m_dlg, "rbJava", wxRadioButton)->GetValue())
    style = 1;
  else if (XRCCTRL(*m_dlg, "rbKr", wxRadioButton)->GetValue())
    style = 2;
  else if (XRCCTRL(*m_dlg, "rbStroustrup", wxRadioButton)->GetValue())
    style = 3;
  else if (XRCCTRL(*m_dlg, "rbWhitesmith", wxRadioButton)->GetValue())
    style = 4;
  else if (XRCCTRL(*m_dlg, "rbBanner", wxRadioButton)->GetValue())
    style = 5;
  else if (XRCCTRL(*m_dlg, "rbLinux", wxRadioButton)->GetValue())
    style = 6;
  else if (XRCCTRL(*m_dlg, "rbGNU", wxRadioButton)->GetValue())
    style = 7;
  else if (XRCCTRL(*m_dlg, "rbCustom", wxRadioButton)->GetValue())
    style = 8;

  switch (style)
  {
    case 0: // Allman (ANSI)
      formatter.setFormattingStyle(astyle::STYLE_ALLMAN);
      break;

    case 1: // Java
      formatter.setFormattingStyle(astyle::STYLE_JAVA);
      break;

    case 2: // K&R
      formatter.setFormattingStyle(astyle::STYLE_KandR);
      break;

    case 3: // Stroustrup
      formatter.setFormattingStyle(astyle::STYLE_STROUSTRUP);
      break;

    case 4: // Whitesmith
      formatter.setFormattingStyle(astyle::STYLE_WHITESMITH);
      break;

    case 5: // Banner
      formatter.setFormattingStyle(astyle::STYLE_BANNER);
      break;

    case 6: // Linux
      formatter.setFormattingStyle(astyle::STYLE_LINUX);
      break;

    case 7: // GNU
      formatter.setFormattingStyle(astyle::STYLE_GNU);
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

      formatter.setClassIndent(XRCCTRL(*m_dlg, "chkIndentClasses", wxCheckBox)->GetValue());
      formatter.setSwitchIndent(XRCCTRL(*m_dlg, "chkIndentSwitches", wxCheckBox)->GetValue());
      formatter.setCaseIndent(XRCCTRL(*m_dlg, "chkIndentCase", wxCheckBox)->GetValue());
      formatter.setBracketIndent(XRCCTRL(*m_dlg, "chkIndentBrackets", wxCheckBox)->GetValue());
      formatter.setBlockIndent(XRCCTRL(*m_dlg, "chkIndentBlocks", wxCheckBox)->GetValue());
      formatter.setNamespaceIndent(XRCCTRL(*m_dlg, "chkIndentNamespaces", wxCheckBox)->GetValue());
      formatter.setLabelIndent(XRCCTRL(*m_dlg, "chkIndentLabels", wxCheckBox)->GetValue());
      formatter.setPreprocessorIndent(XRCCTRL(*m_dlg, "chkIndentPreprocessor", wxCheckBox)->GetValue());

      wxString brackedFormatMode = XRCCTRL(*m_dlg, "cmbBracketFormatMode", wxComboBox)->GetValue();

      if (brackedFormatMode == _T("Attach"))
      {
        formatter.setBracketFormatMode(astyle::ATTACH_MODE);
      }
      else if (brackedFormatMode == _T("Break"))
      {
        formatter.setBracketFormatMode(astyle::BREAK_MODE);
      }
      else if (brackedFormatMode == _T("Linux"))
      {
        formatter.setBracketFormatMode(astyle::LINUX_MODE);
      }
      else if (brackedFormatMode == _T("Stroustrup"))
      {
        formatter.setBracketFormatMode(astyle::STROUSTRUP_MODE);
      }
      else
      {
        formatter.setBracketFormatMode(astyle::NONE_MODE);
      }

      wxString pointerAlign = XRCCTRL(*m_dlg, "cmbPointerAlign", wxComboBox)->GetValue();

      if (pointerAlign == _T("Type"))
      {
        formatter.setPointerAlignment(astyle::ALIGN_TYPE);
      }
      else if (pointerAlign == _T("Middle"))
      {
        formatter.setPointerAlignment(astyle::ALIGN_MIDDLE);
      }
      else if (pointerAlign == _T("Name"))
      {
        formatter.setPointerAlignment(astyle::ALIGN_NAME);
      }
      else
      {
        formatter.setPointerAlignment(astyle::ALIGN_NONE);
      }

      formatter.setBreakClosingHeaderBracketsMode(XRCCTRL(*m_dlg, "chkBreakClosing", wxCheckBox)->GetValue());
      formatter.setBreakBlocksMode(XRCCTRL(*m_dlg, "chkBreakBlocks", wxCheckBox)->GetValue());
      formatter.setBreakElseIfsMode(XRCCTRL(*m_dlg, "chkBreakElseIfs", wxCheckBox)->GetValue());
      formatter.setOperatorPaddingMode(XRCCTRL(*m_dlg, "chkPadOperators", wxCheckBox)->GetValue());
      formatter.setParensOutsidePaddingMode(XRCCTRL(*m_dlg, "chkPadParensOut", wxCheckBox)->GetValue());
      formatter.setParensInsidePaddingMode(XRCCTRL(*m_dlg, "chkPadParensIn", wxCheckBox)->GetValue());
      formatter.setParensUnPaddingMode(XRCCTRL(*m_dlg, "chkUnpadParens", wxCheckBox)->GetValue());
      formatter.setSingleStatementsMode(!XRCCTRL(*m_dlg, "chkKeepComplex", wxCheckBox)->GetValue());
      formatter.setBreakOneLineBlocksMode(!XRCCTRL(*m_dlg, "chkKeepBlocks", wxCheckBox)->GetValue());
      formatter.setTabSpaceConversionMode(XRCCTRL(*m_dlg, "chkConvertTabs", wxCheckBox)->GetValue());
      formatter.setEmptyLineFill(XRCCTRL(*m_dlg, "chkFillEmptyLines", wxCheckBox)->GetValue());
      break;
    }
  }
}
