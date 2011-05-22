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
    else if (XRCCTRL(*m_dlg, "rbBanner", wxRadioButton)->GetValue())
        formatter.setFormattingStyle(astyle::STYLE_BANNER);
    else if (XRCCTRL(*m_dlg, "rbGNU", wxRadioButton)->GetValue())
        formatter.setFormattingStyle(astyle::STYLE_GNU);
    else if (XRCCTRL(*m_dlg, "rbLinux", wxRadioButton)->GetValue())
        formatter.setFormattingStyle(astyle::STYLE_LINUX);
    else if (XRCCTRL(*m_dlg, "rbHorstmann", wxRadioButton)->GetValue())
        formatter.setFormattingStyle(astyle::STYLE_HORSTMANN);
    else if (XRCCTRL(*m_dlg, "rbCustom", wxRadioButton)->GetValue())
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
            formatter.setPointerAlignment(astyle::PTR_ALIGN_TYPE);
        }
        else if (pointerAlign == _T("Middle"))
        {
            formatter.setPointerAlignment(astyle::PTR_ALIGN_MIDDLE);
        }
        else if (pointerAlign == _T("Name"))
        {
            formatter.setPointerAlignment(astyle::PTR_ALIGN_NAME);
        }
        else
        {
            formatter.setPointerAlignment(astyle::PTR_ALIGN_NONE);
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
        formatter.setAddBracketsMode(XRCCTRL(*m_dlg, "chkAddBrackets", wxCheckBox)->GetValue());
    }
}
