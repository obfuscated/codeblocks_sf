#include "astyleconfigdlg.h"
#include <configmanager.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/font.h>
#include <wx/radiobut.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/spinctrl.h>

BEGIN_EVENT_TABLE(AstyleConfigDlg, wxDialog)
    EVT_RADIOBUTTON(XRCID("rbAnsi"), AstyleConfigDlg::OnStyleChange)
    EVT_RADIOBUTTON(XRCID("rbKr"), AstyleConfigDlg::OnStyleChange)
    EVT_RADIOBUTTON(XRCID("rbLinux"), AstyleConfigDlg::OnStyleChange)
    EVT_RADIOBUTTON(XRCID("rbGNU"), AstyleConfigDlg::OnStyleChange)
    EVT_RADIOBUTTON(XRCID("rbJava"), AstyleConfigDlg::OnStyleChange)
    EVT_RADIOBUTTON(XRCID("rbCustom"), AstyleConfigDlg::OnStyleChange)
END_EVENT_TABLE()

AstyleConfigDlg::AstyleConfigDlg(wxWindow* parent)
{
	//ctor
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgAstyleConfig"));
	wxFont font(10, wxMODERN, wxNORMAL, wxNORMAL);
	XRCCTRL(*this, "txtSample", wxTextCtrl)->SetFont(font);

    LoadSettings();
}

AstyleConfigDlg::~AstyleConfigDlg()
{
	//dtor
}

void AstyleConfigDlg::SetStyle(AStylePredefinedStyle style)
{
    wxString sample;
    switch (style)
    {
        case aspsAnsi:
            sample = _T("namespace foospace\n{\n    int Foo()\n    {\n        if (isBar)\n        {\n            bar();\n            return 1;\n        }\n        else\n            return 0;\n    }\n}");
            XRCCTRL(*this, "rbAnsi", wxRadioButton)->SetValue(true);
            break;
        case aspsKr:
            sample = _T("namespace foospace {\n    int Foo() {\n        if (isBar) {\n            bar();\n            return 1;\n         } else\n            return 0;\n    }\n}");
            XRCCTRL(*this, "rbKr", wxRadioButton)->SetValue(true);
            break;
        case aspsLinux:
            sample = _T("namespace foospace\n{\n        int Foo()\n        {\n                if (isBar) {\n                        bar();\n                        return 1;\n                 }\n                 else\n                        return 0;\n        }\n}");
            XRCCTRL(*this, "rbLinux", wxRadioButton)->SetValue(true);
            break;
        case aspsGnu:
            sample = _T("namespace foospace\n  {\n  int Foo()\n  {\n    if (isBar)\n      {\n        bar();\n        return 1;\n      }\n    else\n      return 0;\n  }\n}");
            XRCCTRL(*this, "rbGNU", wxRadioButton)->SetValue(true);
            break;
        case aspsJava:
            sample = _T("namespace foospace {\n    int Foo() {\n        if (isBar) {\n            bar();\n            return 1;\n         }\n         else\n            return 0;\n    }\n}");
            XRCCTRL(*this, "rbJava", wxRadioButton)->SetValue(true);
            break;
        default:
            XRCCTRL(*this, "rbCustom", wxRadioButton)->SetValue(true);
            break;
    }

    bool en = style != aspsCustom;
    XRCCTRL(*this, "txtSample", wxTextCtrl)->SetValue(sample);
    XRCCTRL(*this, "txtSample", wxTextCtrl)->Enable(en);

    // disable/enable checkboxes based on style
    XRCCTRL(*this, "spnIndentation", wxSpinCtrl)->Enable(!en);
    XRCCTRL(*this, "chkUseTab", wxCheckBox)->Enable(!en);
    XRCCTRL(*this, "chkForceUseTabs", wxCheckBox)->Enable(!en);
    XRCCTRL(*this, "chkConvertTabs", wxCheckBox)->Enable(!en);
    XRCCTRL(*this, "chkFillEmptyLines", wxCheckBox)->Enable(!en);
    XRCCTRL(*this, "chkIndentClasses", wxCheckBox)->Enable(!en);
    XRCCTRL(*this, "chkIndentSwitches", wxCheckBox)->Enable(!en);
    XRCCTRL(*this, "chkIndentCase", wxCheckBox)->Enable(!en);
    XRCCTRL(*this, "chkIndentBrackets", wxCheckBox)->Enable(!en);
    XRCCTRL(*this, "chkIndentBlocks", wxCheckBox)->Enable(!en);
    XRCCTRL(*this, "chkIndentNamespaces", wxCheckBox)->Enable(!en);
    XRCCTRL(*this, "chkIndentLabels", wxCheckBox)->Enable(!en);
    XRCCTRL(*this, "chkIndentPreprocessor", wxCheckBox)->Enable(!en);
    XRCCTRL(*this, "cmbBreakType", wxComboBox)->Enable(!en);
    XRCCTRL(*this, "chkBreakBlocks", wxCheckBox)->Enable(!en);
    XRCCTRL(*this, "chkBreakElseIfs", wxCheckBox)->Enable(!en);
    XRCCTRL(*this, "chkPadOperators", wxCheckBox)->Enable(!en);
    XRCCTRL(*this, "chkPadParens", wxCheckBox)->Enable(!en);
    XRCCTRL(*this, "chkKeepComplex", wxCheckBox)->Enable(!en);
    XRCCTRL(*this, "chkKeepBlocks", wxCheckBox)->Enable(!en);
}

void AstyleConfigDlg::OnStyleChange(wxCommandEvent& event)
{
    if (event.GetId() == XRCID("rbAnsi"))
        SetStyle(aspsAnsi);
    else if (event.GetId() == XRCID("rbKr"))
        SetStyle(aspsKr);
    else if (event.GetId() == XRCID("rbLinux"))
        SetStyle(aspsLinux);
    else if (event.GetId() == XRCID("rbGNU"))
        SetStyle(aspsGnu);
    else if (event.GetId() == XRCID("rbJava"))
        SetStyle(aspsJava);
    else if (event.GetId() == XRCID("rbCustom"))
        SetStyle(aspsCustom);
}

void AstyleConfigDlg::LoadSettings()
{
    int style = ConfigManager::Get()->Read(_T("/astyle/style"), 0L);
    XRCCTRL(*this, "spnIndentation", wxSpinCtrl)->SetValue(ConfigManager::Get()->Read(_T("/astyle/indentation"), 4));
    XRCCTRL(*this, "chkUseTab", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/astyle/use_tabs"), 0L));
    XRCCTRL(*this, "chkForceUseTabs", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/astyle/force_tabs"), 0L));
    XRCCTRL(*this, "chkConvertTabs", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/astyle/convert_tabs"), 0L));
    XRCCTRL(*this, "chkFillEmptyLines", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/astyle/fill_empty_lines"), 0L));
    XRCCTRL(*this, "chkIndentClasses", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/astyle/indent_classes"), 0L));
    XRCCTRL(*this, "chkIndentSwitches", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/astyle/indent_switches"), 0L));
    XRCCTRL(*this, "chkIndentCase", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/astyle/indent_case"), 0L));
    XRCCTRL(*this, "chkIndentBrackets", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/astyle/indent_brackets"), 0L));
    XRCCTRL(*this, "chkIndentBlocks", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/astyle/indent_blocks"), 0L));
    XRCCTRL(*this, "chkIndentNamespaces", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/astyle/indent_namespaces"), 0L));
    XRCCTRL(*this, "chkIndentLabels", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/astyle/indent_labels"), 0L));
    XRCCTRL(*this, "chkIndentPreprocessor", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/astyle/indent_preprocessor"), 0L));
    XRCCTRL(*this, "cmbBreakType", wxComboBox)->SetValue(ConfigManager::Get()->Read(_T("/astyle/break_type"), _T("None")));
    XRCCTRL(*this, "chkBreakBlocks", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/astyle/break_blocks"), 0L));
    XRCCTRL(*this, "chkBreakElseIfs", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/astyle/break_elseifs"), 0L));
    XRCCTRL(*this, "chkPadOperators", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/astyle/pad_operators"), 0L));
    XRCCTRL(*this, "chkPadParens", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/astyle/pad_parentheses"), 0L));
    XRCCTRL(*this, "chkKeepComplex", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/astyle/keep_complex"), 0L));
    XRCCTRL(*this, "chkKeepBlocks", wxCheckBox)->SetValue(ConfigManager::Get()->Read(_T("/astyle/keep_blocks"), 0L));

    SetStyle((AStylePredefinedStyle)style);
}

void AstyleConfigDlg::SaveSettings()
{
    int style = 0;
    if (XRCCTRL(*this, "rbAnsi", wxRadioButton)->GetValue())
        style = 0;
    else if (XRCCTRL(*this, "rbKr", wxRadioButton)->GetValue())
        style = 1;
    else if (XRCCTRL(*this, "rbLinux", wxRadioButton)->GetValue())
        style = 2;
    else if (XRCCTRL(*this, "rbGNU", wxRadioButton)->GetValue())
        style = 3;
    else if (XRCCTRL(*this, "rbJava", wxRadioButton)->GetValue())
        style = 4;
    else if (XRCCTRL(*this, "rbCustom", wxRadioButton)->GetValue())
        style = 5;
    ConfigManager::Get()->Write(_T("/astyle/style"), style);
    ConfigManager::Get()->Write(_T("/astyle/indentation"), XRCCTRL(*this, "spnIndentation", wxSpinCtrl)->GetValue());
    ConfigManager::Get()->Write(_T("/astyle/use_tabs"), XRCCTRL(*this, "chkUseTab", wxCheckBox)->GetValue());
    ConfigManager::Get()->Write(_T("/astyle/force_tabs"), XRCCTRL(*this, "chkForceUseTabs", wxCheckBox)->GetValue());
    ConfigManager::Get()->Write(_T("/astyle/convert_tabs"), XRCCTRL(*this, "chkConvertTabs", wxCheckBox)->GetValue());
    ConfigManager::Get()->Write(_T("/astyle/fill_empty_lines"), XRCCTRL(*this, "chkFillEmptyLines", wxCheckBox)->GetValue());
    ConfigManager::Get()->Write(_T("/astyle/indent_classes"), XRCCTRL(*this, "chkIndentClasses", wxCheckBox)->GetValue());
    ConfigManager::Get()->Write(_T("/astyle/indent_switches"), XRCCTRL(*this, "chkIndentSwitches", wxCheckBox)->GetValue());
    ConfigManager::Get()->Write(_T("/astyle/indent_case"), XRCCTRL(*this, "chkIndentCase", wxCheckBox)->GetValue());
    ConfigManager::Get()->Write(_T("/astyle/indent_brackets"), XRCCTRL(*this, "chkIndentBrackets", wxCheckBox)->GetValue());
    ConfigManager::Get()->Write(_T("/astyle/indent_blocks"), XRCCTRL(*this, "chkIndentBlocks", wxCheckBox)->GetValue());
    ConfigManager::Get()->Write(_T("/astyle/indent_namespaces"), XRCCTRL(*this, "chkIndentNamespaces", wxCheckBox)->GetValue());
    ConfigManager::Get()->Write(_T("/astyle/indent_labels"), XRCCTRL(*this, "chkIndentLabels", wxCheckBox)->GetValue());
    ConfigManager::Get()->Write(_T("/astyle/indent_preprocessor"), XRCCTRL(*this, "chkIndentPreprocessor", wxCheckBox)->GetValue());
    ConfigManager::Get()->Write(_T("/astyle/break_type"), XRCCTRL(*this, "cmbBreakType", wxComboBox)->GetValue());
    ConfigManager::Get()->Write(_T("/astyle/break_blocks"), XRCCTRL(*this, "chkBreakBlocks", wxCheckBox)->GetValue());
    ConfigManager::Get()->Write(_T("/astyle/break_elseifs"), XRCCTRL(*this, "chkBreakElseIfs", wxCheckBox)->GetValue());
    ConfigManager::Get()->Write(_T("/astyle/pad_operators"), XRCCTRL(*this, "chkPadOperators", wxCheckBox)->GetValue());
    ConfigManager::Get()->Write(_T("/astyle/pad_parentheses"), XRCCTRL(*this, "chkPadParens", wxCheckBox)->GetValue());
    ConfigManager::Get()->Write(_T("/astyle/keep_complex"), XRCCTRL(*this, "chkKeepComplex", wxCheckBox)->GetValue());
    ConfigManager::Get()->Write(_T("/astyle/keep_blocks"), XRCCTRL(*this, "chkKeepBlocks", wxCheckBox)->GetValue());
}

void AstyleConfigDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK)
    {
        // user pressed OK; save settings
        SaveSettings();
    }

    wxDialog::EndModal(retCode);
}
