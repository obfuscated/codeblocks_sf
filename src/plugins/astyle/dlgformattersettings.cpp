/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "dlgformattersettings.h"
#include "astylepredefinedstyles.h"
#include <wx/radiobut.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/spinctrl.h>
#include <wx/xrc/xmlres.h>

DlgFormatterSettings::DlgFormatterSettings(wxWindow *dlg) :
  m_dlg(dlg)
{
}

DlgFormatterSettings::~DlgFormatterSettings()
{
}

void DlgFormatterSettings::ApplyTo(astyle::ASFormatter& formatter)
{
  // NOTE: Keep this in sync with FormatterSettings::ApplyTo
  if (XRCCTRL(*m_dlg, "rbAllman", wxRadioButton)->GetValue())
    formatter.setFormattingStyle(astyle::STYLE_ALLMAN);
  else if (XRCCTRL(*m_dlg, "rbJava", wxRadioButton)->GetValue())
    formatter.setFormattingStyle(astyle::STYLE_JAVA);
  else if (XRCCTRL(*m_dlg, "rbKr", wxRadioButton)->GetValue())
    formatter.setFormattingStyle(astyle::STYLE_KR);
  else if (XRCCTRL(*m_dlg, "rbStroustrup", wxRadioButton)->GetValue())
    formatter.setFormattingStyle(astyle::STYLE_STROUSTRUP);
  else if (XRCCTRL(*m_dlg, "rbWhitesmith", wxRadioButton)->GetValue())
    formatter.setFormattingStyle(astyle::STYLE_WHITESMITH);
  else if (XRCCTRL(*m_dlg, "rbVTK", wxRadioButton)->GetValue())
    formatter.setFormattingStyle(astyle::STYLE_VTK);
  else if (XRCCTRL(*m_dlg, "rbBanner", wxRadioButton)->GetValue())
    formatter.setFormattingStyle(astyle::STYLE_BANNER);
  else if (XRCCTRL(*m_dlg, "rbGNU", wxRadioButton)->GetValue())
    formatter.setFormattingStyle(astyle::STYLE_GNU);
  else if (XRCCTRL(*m_dlg, "rbLinux", wxRadioButton)->GetValue())
    formatter.setFormattingStyle(astyle::STYLE_LINUX);
  else if (XRCCTRL(*m_dlg, "rbHorstmann", wxRadioButton)->GetValue())
    formatter.setFormattingStyle(astyle::STYLE_HORSTMANN);
  else if (XRCCTRL(*m_dlg, "rb1TBS", wxRadioButton)->GetValue())
    formatter.setFormattingStyle(astyle::STYLE_1TBS);
  else if (XRCCTRL(*m_dlg, "rbGoogle", wxRadioButton)->GetValue())
    formatter.setFormattingStyle(astyle::STYLE_GOOGLE);
  else if (XRCCTRL(*m_dlg, "rbPico", wxRadioButton)->GetValue())
    formatter.setFormattingStyle(astyle::STYLE_PICO);
  else if (XRCCTRL(*m_dlg, "rbLisp", wxRadioButton)->GetValue())
    formatter.setFormattingStyle(astyle::STYLE_LISP);

  formatter.setAttachClass(XRCCTRL(*m_dlg,     "chkAttachClasses",    wxCheckBox)->GetValue());
  formatter.setAttachExternC(XRCCTRL(*m_dlg,   "chkAttachExternC",    wxCheckBox)->GetValue());
  formatter.setAttachNamespace(XRCCTRL(*m_dlg, "chkAttachNamespaces", wxCheckBox)->GetValue());
  formatter.setAttachInline(XRCCTRL(*m_dlg,    "chkAttachInlines",    wxCheckBox)->GetValue());

  bool value = XRCCTRL(*m_dlg, "chkForceUseTabs", wxCheckBox)->GetValue();
  int spaceNum = XRCCTRL(*m_dlg, "spnIndentation", wxSpinCtrl)->GetValue();

  if (XRCCTRL(*m_dlg, "chkUseTab", wxCheckBox)->GetValue())
    formatter.setTabIndentation(spaceNum, value);
  else
    formatter.setSpaceIndentation(spaceNum);

  formatter.setCaseIndent(XRCCTRL(*m_dlg,               "chkIndentCase",          wxCheckBox)->GetValue());
  formatter.setClassIndent(XRCCTRL(*m_dlg,              "chkIndentClasses",       wxCheckBox)->GetValue());
  formatter.setLabelIndent(XRCCTRL(*m_dlg,              "chkIndentLabels",        wxCheckBox)->GetValue());
  formatter.setModifierIndent(XRCCTRL(*m_dlg,           "chkIndentModifiers",     wxCheckBox)->GetValue());
  formatter.setNamespaceIndent(XRCCTRL(*m_dlg,          "chkIndentNamespaces",    wxCheckBox)->GetValue());
  formatter.setSwitchIndent(XRCCTRL(*m_dlg,             "chkIndentSwitches",      wxCheckBox)->GetValue());
  formatter.setPreprocBlockIndent(XRCCTRL(*m_dlg,       "chkIndentPreprocBlock",  wxCheckBox)->GetValue());
  formatter.setPreprocDefineIndent(XRCCTRL(*m_dlg,      "chkIndentPreprocDefine", wxCheckBox)->GetValue());
  formatter.setPreprocConditionalIndent(XRCCTRL(*m_dlg, "chkIndentPreprocCond",   wxCheckBox)->GetValue());
  formatter.setIndentCol1CommentsMode(XRCCTRL(*m_dlg,   "chkIndentCol1Comments",  wxCheckBox)->GetValue());
  int minConditionalEvent = XRCCTRL(*m_dlg, "spnMinConditionalEvent", wxSpinCtrl)->GetValue();
  formatter.setMinConditionalIndentOption(minConditionalEvent);
  formatter.setMaxInStatementIndentLength( wxAtoi(XRCCTRL(*m_dlg, "txtMaxInStatementIndent", wxTextCtrl)->GetValue()) );

  formatter.setBreakClosingHeaderBracketsMode(XRCCTRL(*m_dlg, "chkBreakClosing", wxCheckBox)->GetValue());
  formatter.setBreakElseIfsMode(XRCCTRL(*m_dlg, "chkBreakElseIfs",               wxCheckBox)->GetValue());
  formatter.setAddBracketsMode(XRCCTRL(*m_dlg, "chkAddBrackets",                 wxCheckBox)->GetValue());
  formatter.setAddOneLineBracketsMode(XRCCTRL(*m_dlg, "chkAddOneLineBrackets",   wxCheckBox)->GetValue());
  formatter.setRemoveBracketsMode(XRCCTRL(*m_dlg, "chkRemoveBrackets",           wxCheckBox)->GetValue());
  formatter.setBreakOneLineBlocksMode(!XRCCTRL(*m_dlg, "chkKeepBlocks",          wxCheckBox)->GetValue());
  formatter.setSingleStatementsMode(!XRCCTRL(*m_dlg, "chkKeepComplex",           wxCheckBox)->GetValue());
  formatter.setTabSpaceConversionMode(XRCCTRL(*m_dlg, "chkConvertTabs",          wxCheckBox)->GetValue());
  formatter.setCloseTemplatesMode(XRCCTRL(*m_dlg, "chkCloseTemplates",           wxCheckBox)->GetValue());
  formatter.setStripCommentPrefix(XRCCTRL(*m_dlg, "chkRemoveCommentPrefix",      wxCheckBox)->GetValue());

  if (XRCCTRL(*m_dlg, "chkBreakLines", wxCheckBox)->GetValue())
  {
    formatter.setMaxCodeLength( wxAtoi(XRCCTRL(*m_dlg, "txtMaxLineLength", wxTextCtrl)->GetValue()) );
    formatter.setBreakAfterMode(XRCCTRL(*m_dlg, "chkBreakAfterLogical", wxCheckBox)->GetValue());
  }
  else
    formatter.setMaxCodeLength(INT_MAX);

  formatter.setBreakBlocksMode(XRCCTRL(*m_dlg, "chkBreakBlocks",                 wxCheckBox)->GetValue());
  formatter.setBreakClosingHeaderBlocksMode(XRCCTRL(*m_dlg, "chkBreakBlocksAll", wxCheckBox)->GetValue());
  formatter.setOperatorPaddingMode(XRCCTRL(*m_dlg, "chkPadOperators",            wxCheckBox)->GetValue());
  formatter.setParensOutsidePaddingMode(XRCCTRL(*m_dlg, "chkPadParensOut",       wxCheckBox)->GetValue());
  formatter.setParensInsidePaddingMode(XRCCTRL(*m_dlg, "chkPadParensIn",         wxCheckBox)->GetValue());
  formatter.setParensHeaderPaddingMode(XRCCTRL(*m_dlg, "chkPadHeader",           wxCheckBox)->GetValue());
  formatter.setParensUnPaddingMode(XRCCTRL(*m_dlg, "chkUnpadParens",             wxCheckBox)->GetValue());
  formatter.setDeleteEmptyLinesMode(XRCCTRL(*m_dlg, "chkDelEmptyLine",           wxCheckBox)->GetValue());
  formatter.setEmptyLineFill(XRCCTRL(*m_dlg, "chkFillEmptyLines",                wxCheckBox)->GetValue());

  wxString pointerAlign = XRCCTRL(*m_dlg, "cmbPointerAlign", wxComboBox)->GetValue();
  if      (pointerAlign == _T("Type"))
    formatter.setPointerAlignment(astyle::PTR_ALIGN_TYPE);
  else if (pointerAlign == _T("Middle"))
    formatter.setPointerAlignment(astyle::PTR_ALIGN_MIDDLE);
  else if (pointerAlign == _T("Name"))
    formatter.setPointerAlignment(astyle::PTR_ALIGN_NAME);
  else
    formatter.setPointerAlignment(astyle::PTR_ALIGN_NONE);

  wxString referenceAlign = XRCCTRL(*m_dlg, "cmbReferenceAlign", wxComboBox)->GetValue();
  if      (referenceAlign == _T("Type"))
    formatter.setReferenceAlignment(astyle::REF_ALIGN_TYPE);
  else if (referenceAlign == _T("Middle"))
    formatter.setReferenceAlignment(astyle::REF_ALIGN_MIDDLE);
  else if (referenceAlign == _T("Name"))
    formatter.setReferenceAlignment(astyle::REF_ALIGN_NAME);
  else
    formatter.setReferenceAlignment(astyle::REF_ALIGN_NONE);
}
