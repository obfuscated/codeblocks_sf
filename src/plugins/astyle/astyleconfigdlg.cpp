#include <sdk.h>
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
#include "dlgformattersettings.h"
#include "asstreamiterator.h"
#include <string>

#ifndef CB_PRECOMP
    #include "globals.h"
#endif

BEGIN_EVENT_TABLE(AstyleConfigDlg, wxPanel)
  EVT_RADIOBUTTON(XRCID("rbAnsi"), AstyleConfigDlg::OnStyleChange)
  EVT_RADIOBUTTON(XRCID("rbKr"), AstyleConfigDlg::OnStyleChange)
  EVT_RADIOBUTTON(XRCID("rbLinux"), AstyleConfigDlg::OnStyleChange)
  EVT_RADIOBUTTON(XRCID("rbGNU"), AstyleConfigDlg::OnStyleChange)
  EVT_RADIOBUTTON(XRCID("rbJava"), AstyleConfigDlg::OnStyleChange)
  EVT_RADIOBUTTON(XRCID("rbCustom"), AstyleConfigDlg::OnStyleChange)
  EVT_BUTTON(XRCID("Preview"), AstyleConfigDlg::OnPreview)
END_EVENT_TABLE()

AstyleConfigDlg::AstyleConfigDlg(wxWindow* parent)
{
	//ctor
	wxXmlResource::Get()->LoadPanel(this, parent, _T("dlgAstyleConfig"));
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
      sample = _T("namespace foospace {\n    int Foo() {\n        if (isBar) {\n            bar();\n            return 1;\n        } else\n            return 0;\n    }\n}");
      XRCCTRL(*this, "rbKr", wxRadioButton)->SetValue(true);
      break;

    case aspsLinux:
      sample = _T("namespace foospace\n{\n        int Foo()\n        {\n                if (isBar) {\n                        bar();\n                        return 1;\n                } else\n                        return 0;\n        }\n}");
      XRCCTRL(*this, "rbLinux", wxRadioButton)->SetValue(true);
      break;

    case aspsGnu:
      sample = _T("namespace foospace\n  {\n  int Foo()\n  {\n    if (isBar)\n      {\n        bar();\n        return 1;\n      }\n    else\n      return 0;\n  }\n}");
      XRCCTRL(*this, "rbGNU", wxRadioButton)->SetValue(true);
      break;

    case aspsJava:
      sample = _T("namespace foospace {\n    int Foo() {\n        if (isBar) {\n            bar();\n            return 1;\n        } else\n            return 0;\n    }\n}");
      XRCCTRL(*this, "rbJava", wxRadioButton)->SetValue(true);
      break;

    default:
      XRCCTRL(*this, "rbCustom", wxRadioButton)->SetValue(true);
      break;
  }

  bool en = style != aspsCustom;

  if (!sample.IsEmpty())
  {
    XRCCTRL(*this, "txtSample", wxTextCtrl)->SetValue(sample);
  }

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
  XRCCTRL(*this, "chkPadParensIn", wxCheckBox)->Enable(!en);
  XRCCTRL(*this, "chkPadParensOut", wxCheckBox)->Enable(!en);
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

void AstyleConfigDlg::OnPreview(wxCommandEvent& WXUNUSED(event))
{
  wxString text(XRCCTRL(*this, "txtSample", wxTextCtrl)->GetValue());
  wxString formattedText;

  astyle::ASFormatter formatter;

  // load settings
  dlgFormatterSettings settings(this);
  settings.ApplyTo(formatter);

  if (text.size() && text.Last() != _T('\r') && text.Last() != _T('\n'))
  {
    text += _T('\n');
  }

  formatter.init(new ASStreamIterator(text, _T('\n')));

  while (formatter.hasMoreLines())
  {
    formattedText << cbC2U(formatter.nextLine().c_str());

    if (formatter.hasMoreLines())
    {
      formattedText << _T('\n');
    }
  }

  XRCCTRL(*this, "txtSample", wxTextCtrl)->SetValue(formattedText);
}

void AstyleConfigDlg::LoadSettings()
{
  ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("astyle"));
  int style = cfg->ReadInt(_T("/style"), 0);

  XRCCTRL(*this, "spnIndentation", wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/indentation"), 4));
  XRCCTRL(*this, "chkUseTab", wxCheckBox)->SetValue(cfg->ReadBool(_T("/use_tabs"), false));
  XRCCTRL(*this, "chkForceUseTabs", wxCheckBox)->SetValue(cfg->ReadBool(_T("/force_tabs"), false));
  XRCCTRL(*this, "chkConvertTabs", wxCheckBox)->SetValue(cfg->ReadBool(_T("/convert_tabs"), false));
  XRCCTRL(*this, "chkFillEmptyLines", wxCheckBox)->SetValue(cfg->ReadBool(_T("/fill_empty_lines"), false));
  XRCCTRL(*this, "chkIndentClasses", wxCheckBox)->SetValue(cfg->ReadBool(_T("/indent_classes"), false));
  XRCCTRL(*this, "chkIndentSwitches", wxCheckBox)->SetValue(cfg->ReadBool(_T("/indent_switches"), false));
  XRCCTRL(*this, "chkIndentCase", wxCheckBox)->SetValue(cfg->ReadBool(_T("/indent_case"), false));
  XRCCTRL(*this, "chkIndentBrackets", wxCheckBox)->SetValue(cfg->ReadBool(_T("/indent_brackets"), false));
  XRCCTRL(*this, "chkIndentBlocks", wxCheckBox)->SetValue(cfg->ReadBool(_T("/indent_blocks"), false));
  XRCCTRL(*this, "chkIndentNamespaces", wxCheckBox)->SetValue(cfg->ReadBool(_T("/indent_namespaces"), false));
  XRCCTRL(*this, "chkIndentLabels", wxCheckBox)->SetValue(cfg->ReadBool(_T("/indent_labels"), false));
  XRCCTRL(*this, "chkIndentPreprocessor", wxCheckBox)->SetValue(cfg->ReadBool(_T("/indent_preprocessor"), false));
  XRCCTRL(*this, "cmbBreakType", wxComboBox)->SetValue(cfg->Read(_T("/break_type"), _T("None")));
  XRCCTRL(*this, "chkBreakBlocks", wxCheckBox)->SetValue(cfg->ReadBool(_T("/break_blocks"), false));
  XRCCTRL(*this, "chkBreakElseIfs", wxCheckBox)->SetValue(cfg->ReadBool(_T("/break_elseifs"), false));
  XRCCTRL(*this, "chkPadOperators", wxCheckBox)->SetValue(cfg->ReadBool(_T("/pad_operators"), false));
  XRCCTRL(*this, "chkPadParensIn", wxCheckBox)->SetValue(cfg->ReadBool(_T("/pad_parentheses_in"), false));
  XRCCTRL(*this, "chkPadParensOut", wxCheckBox)->SetValue(cfg->ReadBool(_T("/pad_parentheses_out"), false));
  XRCCTRL(*this, "chkKeepComplex", wxCheckBox)->SetValue(cfg->ReadBool(_T("/keep_complex"), false));
  XRCCTRL(*this, "chkKeepBlocks", wxCheckBox)->SetValue(cfg->ReadBool(_T("/keep_blocks"), false));

  SetStyle((AStylePredefinedStyle)style);
}

void AstyleConfigDlg::SaveSettings()
{
  ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("astyle"));
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

  cfg->Write(_T("/style"), style);
  cfg->Write(_T("/indentation"), XRCCTRL(*this, "spnIndentation", wxSpinCtrl)->GetValue());
  cfg->Write(_T("/use_tabs"), XRCCTRL(*this, "chkUseTab", wxCheckBox)->GetValue());
  cfg->Write(_T("/force_tabs"), XRCCTRL(*this, "chkForceUseTabs", wxCheckBox)->GetValue());
  cfg->Write(_T("/convert_tabs"), XRCCTRL(*this, "chkConvertTabs", wxCheckBox)->GetValue());
  cfg->Write(_T("/fill_empty_lines"), XRCCTRL(*this, "chkFillEmptyLines", wxCheckBox)->GetValue());
  cfg->Write(_T("/indent_classes"), XRCCTRL(*this, "chkIndentClasses", wxCheckBox)->GetValue());
  cfg->Write(_T("/indent_switches"), XRCCTRL(*this, "chkIndentSwitches", wxCheckBox)->GetValue());
  cfg->Write(_T("/indent_case"), XRCCTRL(*this, "chkIndentCase", wxCheckBox)->GetValue());
  cfg->Write(_T("/indent_brackets"), XRCCTRL(*this, "chkIndentBrackets", wxCheckBox)->GetValue());
  cfg->Write(_T("/indent_blocks"), XRCCTRL(*this, "chkIndentBlocks", wxCheckBox)->GetValue());
  cfg->Write(_T("/indent_namespaces"), XRCCTRL(*this, "chkIndentNamespaces", wxCheckBox)->GetValue());
  cfg->Write(_T("/indent_labels"), XRCCTRL(*this, "chkIndentLabels", wxCheckBox)->GetValue());
  cfg->Write(_T("/indent_preprocessor"), XRCCTRL(*this, "chkIndentPreprocessor", wxCheckBox)->GetValue());
  cfg->Write(_T("/break_type"), XRCCTRL(*this, "cmbBreakType", wxComboBox)->GetValue());
  cfg->Write(_T("/break_blocks"), XRCCTRL(*this, "chkBreakBlocks", wxCheckBox)->GetValue());
  cfg->Write(_T("/break_elseifs"), XRCCTRL(*this, "chkBreakElseIfs", wxCheckBox)->GetValue());
  cfg->Write(_T("/pad_operators"), XRCCTRL(*this, "chkPadOperators", wxCheckBox)->GetValue());
  cfg->Write(_T("/pad_parentheses_in"), XRCCTRL(*this, "chkPadParensIn", wxCheckBox)->GetValue());
  cfg->Write(_T("/pad_parentheses_out"), XRCCTRL(*this, "chkPadParensOut", wxCheckBox)->GetValue());
  cfg->Write(_T("/keep_complex"), XRCCTRL(*this, "chkKeepComplex", wxCheckBox)->GetValue());
  cfg->Write(_T("/keep_blocks"), XRCCTRL(*this, "chkKeepBlocks", wxCheckBox)->GetValue());
}
