/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

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
  EVT_RADIOBUTTON(XRCID("rbAllman"),     AstyleConfigDlg::OnStyleChange)
  EVT_RADIOBUTTON(XRCID("rbJava"),       AstyleConfigDlg::OnStyleChange)
  EVT_RADIOBUTTON(XRCID("rbKr"),         AstyleConfigDlg::OnStyleChange)
  EVT_RADIOBUTTON(XRCID("rbStroustrup"), AstyleConfigDlg::OnStyleChange)
  EVT_RADIOBUTTON(XRCID("rbWhitesmith"), AstyleConfigDlg::OnStyleChange)
  EVT_RADIOBUTTON(XRCID("rbBanner"),     AstyleConfigDlg::OnStyleChange)
  EVT_RADIOBUTTON(XRCID("rbGNU"),        AstyleConfigDlg::OnStyleChange)
  EVT_RADIOBUTTON(XRCID("rbLinux"),      AstyleConfigDlg::OnStyleChange)
  EVT_RADIOBUTTON(XRCID("rbHorstmann"),  AstyleConfigDlg::OnStyleChange)
  EVT_RADIOBUTTON(XRCID("rb1TBS"),       AstyleConfigDlg::OnStyleChange)
  EVT_RADIOBUTTON(XRCID("rbPico"),       AstyleConfigDlg::OnStyleChange)
  EVT_RADIOBUTTON(XRCID("rbLisp"),       AstyleConfigDlg::OnStyleChange)
  EVT_RADIOBUTTON(XRCID("rbCustom"),     AstyleConfigDlg::OnStyleChange)
  EVT_BUTTON(XRCID("Preview"),           AstyleConfigDlg::OnPreview    )
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
#define AS_ALLMAN "\
int Foo(bool isBar)\n\
{\n\
    if (isBar)\n\
    {\n\
        bar();\n\
        return 1;\n\
    }\n\
    else\n\
        return 0;\n\
}"
    case aspsAllman:
      sample = _T(AS_ALLMAN);
      XRCCTRL(*this, "rbAllman", wxRadioButton)->SetValue(true);
      break;
#undef AS_ALLMAN
#define AS_JAVA "\
int Foo(bool isBar) {\n\
    if (isBar) {\n\
        bar();\n\
        return 1;\n\
    } else\n\
        return 0;\n\
}"
    case aspsJava:
      sample = _T(AS_JAVA);
      XRCCTRL(*this, "rbJava", wxRadioButton)->SetValue(true);
      break;
#undef AS_JAVA
#define AS_KR "\
int Foo(bool isBar)\n\
{\n\
    if (isBar) {\n\
        bar();\n\
        return 1;\n\
    } else\n\
        return 0;\n\
}"
    case aspsKr:
      sample = _T(AS_KR);
      XRCCTRL(*this, "rbKr", wxRadioButton)->SetValue(true);
      break;
#undef AS_KR
#define AS_STROUSTRUP "\
int Foo(bool isBar)\n\
{\n\
     if (isBar) {\n\
          bar();\n\
          return 1;\n\
     } else\n\
          return 0;\n\
}"
    case aspsStroustrup:
      sample = _T(AS_STROUSTRUP);
      XRCCTRL(*this, "rbStroustrup", wxRadioButton)->SetValue(true);
      break;
#undef AS_STROUSTRUP
#define AS_WHITESMITH "\
int Foo(bool isBar)\n\
    {\n\
    if (isBar)\n\
        {\n\
        bar();\n\
        return 1;\n\
        }\n\
    else\n\
        return 0;\n\
    }"
    case aspsWhitesmith:
      sample = _T(AS_WHITESMITH);
      XRCCTRL(*this, "rbWhitesmith", wxRadioButton)->SetValue(true);
      break;
#undef AS_WHITESMITH
#define AS_BANNER "\
int Foo(bool isBar) {\n\
    if (isBar) {\n\
        bar();\n\
        return 1;\n\
        }\n\
    else\n\
        return 0;\n\
    }"
    case aspsBanner:
      sample = _T(AS_BANNER);
      XRCCTRL(*this, "rbBanner", wxRadioButton)->SetValue(true);
      break;
#undef AS_BANNER
#define AS_GNU "\
int Foo(bool isBar)\n\
{\n\
  if (isBar)\n\
    {\n\
      bar();\n\
      return 1;\n\
    }\n\
  else\n\
    return 0;\n\
}"
    case aspsGnu:
      sample = _T(AS_GNU);
      XRCCTRL(*this, "rbGNU", wxRadioButton)->SetValue(true);
      break;
#undef AS_GNU
#define AS_LINUX "\
int Foo(bool isBar)\n\
{\n\
        if (isBar) {\n\
                bar();\n\
                return 1;\n\
        } else\n\
                return 0;\n\
}"
    case aspsLinux:
      sample = _T(AS_LINUX);
      XRCCTRL(*this, "rbLinux", wxRadioButton)->SetValue(true);
      break;
#undef AS_LINUX
#define AS_HORSTMANN "\
int Foo(bool isBar)\n\
{  if (isBar)\n\
   {  bar();\n\
      return 1;\n\
   }\n\
   else\n\
      return 0;\n\
}"
    case aspsHorstmann:
      sample = _T(AS_HORSTMANN);
      XRCCTRL(*this, "rbHorstmann", wxRadioButton)->SetValue(true);
      break;
#undef AS_HORSTMANN
#define AS_1TBS "\
int Foo(bool isBar)\n\
{\n\
    if (isFoo) {\n\
        bar();\n\
        return 1;\n\
    } else {\n\
        return 0;\n\
    }\n\
}"
    case asps1TBS:
      sample = _T(AS_1TBS);
      XRCCTRL(*this, "rb1TBS", wxRadioButton)->SetValue(true);
      break;
#undef AS_1TBS
#define AS_PICO "\
int Foo(bool isBar)\n\
{  if (isBar)\n\
   {  bar();\n\
      return 1; }\n\
    else\n\
      return 0; }"
    case aspsPico:
      sample = _T(AS_PICO);
      XRCCTRL(*this, "rbPico", wxRadioButton)->SetValue(true);
      break;
#undef AS_PICO
#define AS_LISP "\
int Foo(bool isBar) {\n\
    if (isBar) {\n\
        bar()\n\
        return 1; }\n\
    else\n\
        return 0; }"
    case aspsLisp:
      sample = _T(AS_LISP);
      XRCCTRL(*this, "rbLisp", wxRadioButton)->SetValue(true);
      break;
#undef AS_LISP
    default:
      XRCCTRL(*this, "rbCustom", wxRadioButton)->SetValue(true);
      break;
  }

  if (!sample.IsEmpty())
    XRCCTRL(*this, "txtSample", wxTextCtrl)->SetValue(sample);
}

void AstyleConfigDlg::OnStyleChange(wxCommandEvent& event)
{
  if      (event.GetId() == XRCID("rbAllman"))
    SetStyle(aspsAllman);
  else if (event.GetId() == XRCID("rbJava"))
    SetStyle(aspsJava);
  else if (event.GetId() == XRCID("rbKr"))
    SetStyle(aspsKr);
  else if (event.GetId() == XRCID("rbStroustrup"))
    SetStyle(aspsStroustrup);
  else if (event.GetId() == XRCID("rbWhitesmith"))
    SetStyle(aspsWhitesmith);
  else if (event.GetId() == XRCID("rbBanner"))
    SetStyle(aspsBanner);
  else if (event.GetId() == XRCID("rbGNU"))
    SetStyle(aspsGnu);
  else if (event.GetId() == XRCID("rbLinux"))
    SetStyle(aspsLinux);
  else if (event.GetId() == XRCID("rbHorstmann"))
    SetStyle(aspsHorstmann);
  else if (event.GetId() == XRCID("rb1TBS"))
    SetStyle(asps1TBS);
  else if (event.GetId() == XRCID("rbPico"))
    SetStyle(aspsPico);
  else if (event.GetId() == XRCID("rbLisp"))
    SetStyle(aspsLisp);
  else if (event.GetId() == XRCID("rbCustom"))
    SetStyle(aspsCustom);
}

void AstyleConfigDlg::OnPreview(wxCommandEvent& WXUNUSED(event))
{
  wxString text(XRCCTRL(*this, "txtSample", wxTextCtrl)->GetValue());
  wxString formattedText;

  astyle::ASFormatter formatter;

  // load settings
  DlgFormatterSettings settings(this);
  settings.ApplyTo(formatter);

  if (text.size() && text.Last() != _T('\r') && text.Last() != _T('\n'))
    text += _T('\n');

  formatter.init(new ASStreamIterator(0, text));

  while (formatter.hasMoreLines())
  {
    formattedText << cbC2U(formatter.nextLine().c_str());

    if (formatter.hasMoreLines())
      formattedText << _T('\n');
  }

  XRCCTRL(*this, "txtSample", wxTextCtrl)->SetValue(formattedText);
}

void AstyleConfigDlg::LoadSettings()
{
  ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("astyle"));
  int style = cfg->ReadInt(_T("/style"), 0);

  XRCCTRL(*this, "spnIndentation",        wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/indentation"),           4));
  XRCCTRL(*this, "chkUseTab",             wxCheckBox)->SetValue(cfg->ReadBool(_T("/use_tabs"),             false));
  XRCCTRL(*this, "chkForceUseTabs",       wxCheckBox)->SetValue(cfg->ReadBool(_T("/force_tabs"),           false));
  XRCCTRL(*this, "chkIndentClasses",      wxCheckBox)->SetValue(cfg->ReadBool(_T("/indent_classes"),       false));
  XRCCTRL(*this, "chkIndentSwitches",     wxCheckBox)->SetValue(cfg->ReadBool(_T("/indent_switches"),      false));
  XRCCTRL(*this, "chkIndentCase",         wxCheckBox)->SetValue(cfg->ReadBool(_T("/indent_case"),          false));
  XRCCTRL(*this, "chkIndentBrackets",     wxCheckBox)->SetValue(cfg->ReadBool(_T("/indent_brackets"),      false));
  XRCCTRL(*this, "chkIndentBlocks",       wxCheckBox)->SetValue(cfg->ReadBool(_T("/indent_blocks"),        false));
  XRCCTRL(*this, "chkIndentNamespaces",   wxCheckBox)->SetValue(cfg->ReadBool(_T("/indent_namespaces"),    false));
  XRCCTRL(*this, "chkIndentLabels",       wxCheckBox)->SetValue(cfg->ReadBool(_T("/indent_labels"),        false));
  XRCCTRL(*this, "chkIndentPreprocessor", wxCheckBox)->SetValue(cfg->ReadBool(_T("/indent_preprocessor"),  false));
  XRCCTRL(*this, "chkIndentCol1Comments", wxCheckBox)->SetValue(cfg->ReadBool(_T("/indent_col1_comments"), false));
  XRCCTRL(*this, "cmbPointerAlign",       wxComboBox)->SetValue(cfg->Read(_T("/pointer_align"),            _T("None")));
  XRCCTRL(*this, "chkBreakClosing",       wxCheckBox)->SetValue(cfg->ReadBool(_T("/break_closing"),        false));
  XRCCTRL(*this, "chkBreakBlocks",        wxCheckBox)->SetValue(cfg->ReadBool(_T("/break_blocks"),         false));
  XRCCTRL(*this, "chkBreakElseIfs",       wxCheckBox)->SetValue(cfg->ReadBool(_T("/break_elseifs"),        false));
  XRCCTRL(*this, "chkPadOperators",       wxCheckBox)->SetValue(cfg->ReadBool(_T("/pad_operators"),        false));
  XRCCTRL(*this, "chkPadParensIn",        wxCheckBox)->SetValue(cfg->ReadBool(_T("/pad_parentheses_in"),   false));
  XRCCTRL(*this, "chkPadParensOut",       wxCheckBox)->SetValue(cfg->ReadBool(_T("/pad_parentheses_out"),  false));
  XRCCTRL(*this, "chkPadHeader",          wxCheckBox)->SetValue(cfg->ReadBool(_T("/pad_header"),           false));
  XRCCTRL(*this, "chkUnpadParens",        wxCheckBox)->SetValue(cfg->ReadBool(_T("/unpad_parentheses"),    false));
  XRCCTRL(*this, "chkDelEmptyLine",       wxCheckBox)->SetValue(cfg->ReadBool(_T("/delete_empty_lines"),   false));
  XRCCTRL(*this, "chkKeepComplex",        wxCheckBox)->SetValue(cfg->ReadBool(_T("/keep_complex"),         false));
  XRCCTRL(*this, "chkKeepBlocks",         wxCheckBox)->SetValue(cfg->ReadBool(_T("/keep_blocks"),          false));
  XRCCTRL(*this, "chkConvertTabs",        wxCheckBox)->SetValue(cfg->ReadBool(_T("/convert_tabs"),         false));
  XRCCTRL(*this, "chkFillEmptyLines",     wxCheckBox)->SetValue(cfg->ReadBool(_T("/fill_empty_lines"),     false));
  XRCCTRL(*this, "chkAddBrackets",        wxCheckBox)->SetValue(cfg->ReadBool(_T("/add_brackets"),         false));

  SetStyle((AStylePredefinedStyle)style);
}

void AstyleConfigDlg::SaveSettings()
{
  ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("astyle"));
  int style = 0;

  if      (XRCCTRL(*this, "rbAllman",     wxRadioButton)->GetValue())
    style = aspsAllman;
  else if (XRCCTRL(*this, "rbJava",       wxRadioButton)->GetValue())
    style = aspsJava;
  else if (XRCCTRL(*this, "rbKr",         wxRadioButton)->GetValue())
    style = aspsKr;
  else if (XRCCTRL(*this, "rbStroustrup", wxRadioButton)->GetValue())
    style = aspsStroustrup;
  else if (XRCCTRL(*this, "rbWhitesmith", wxRadioButton)->GetValue())
    style = aspsWhitesmith;
  else if (XRCCTRL(*this, "rbBanner",     wxRadioButton)->GetValue())
    style = aspsBanner;
  else if (XRCCTRL(*this, "rbGNU",        wxRadioButton)->GetValue())
    style = aspsGnu;
  else if (XRCCTRL(*this, "rbLinux",      wxRadioButton)->GetValue())
    style = aspsLinux;
  else if (XRCCTRL(*this, "rbHorstmann",  wxRadioButton)->GetValue())
    style = aspsHorstmann;
  else if (XRCCTRL(*this, "rb1TBS",       wxRadioButton)->GetValue())
    style = asps1TBS;
  else if (XRCCTRL(*this, "rbPico",       wxRadioButton)->GetValue())
    style = aspsPico;
  else if (XRCCTRL(*this, "rbLisp",       wxRadioButton)->GetValue())
    style = aspsLisp;
  else if (XRCCTRL(*this, "rbCustom",     wxRadioButton)->GetValue())
    style = aspsCustom;

  cfg->Write(_T("/style"), style);
  cfg->Write(_T("/indentation"),          XRCCTRL(*this, "spnIndentation",        wxSpinCtrl)->GetValue());
  cfg->Write(_T("/use_tabs"),             XRCCTRL(*this, "chkUseTab",             wxCheckBox)->GetValue());
  cfg->Write(_T("/force_tabs"),           XRCCTRL(*this, "chkForceUseTabs",       wxCheckBox)->GetValue());
  cfg->Write(_T("/indent_classes"),       XRCCTRL(*this, "chkIndentClasses",      wxCheckBox)->GetValue());
  cfg->Write(_T("/indent_switches"),      XRCCTRL(*this, "chkIndentSwitches",     wxCheckBox)->GetValue());
  cfg->Write(_T("/indent_case"),          XRCCTRL(*this, "chkIndentCase",         wxCheckBox)->GetValue());
  cfg->Write(_T("/indent_brackets"),      XRCCTRL(*this, "chkIndentBrackets",     wxCheckBox)->GetValue());
  cfg->Write(_T("/indent_blocks"),        XRCCTRL(*this, "chkIndentBlocks",       wxCheckBox)->GetValue());
  cfg->Write(_T("/indent_namespaces"),    XRCCTRL(*this, "chkIndentNamespaces",   wxCheckBox)->GetValue());
  cfg->Write(_T("/indent_labels"),        XRCCTRL(*this, "chkIndentLabels",       wxCheckBox)->GetValue());
  cfg->Write(_T("/indent_preprocessor"),  XRCCTRL(*this, "chkIndentPreprocessor", wxCheckBox)->GetValue());
  cfg->Write(_T("/indent_col1_comments"), XRCCTRL(*this, "chkIndentCol1Comments", wxCheckBox)->GetValue());
  cfg->Write(_T("/pointer_align"),        XRCCTRL(*this, "cmbPointerAlign",       wxComboBox)->GetValue());
  cfg->Write(_T("/break_closing"),        XRCCTRL(*this, "chkBreakClosing",       wxCheckBox)->GetValue());
  cfg->Write(_T("/break_blocks"),         XRCCTRL(*this, "chkBreakBlocks",        wxCheckBox)->GetValue());
  cfg->Write(_T("/break_elseifs"),        XRCCTRL(*this, "chkBreakElseIfs",       wxCheckBox)->GetValue());
  cfg->Write(_T("/pad_operators"),        XRCCTRL(*this, "chkPadOperators",       wxCheckBox)->GetValue());
  cfg->Write(_T("/pad_parentheses_in"),   XRCCTRL(*this, "chkPadParensIn",        wxCheckBox)->GetValue());
  cfg->Write(_T("/pad_parentheses_out"),  XRCCTRL(*this, "chkPadParensOut",       wxCheckBox)->GetValue());
  cfg->Write(_T("/pad_header"),           XRCCTRL(*this, "chkPadHeader",          wxCheckBox)->GetValue());
  cfg->Write(_T("/unpad_parentheses"),    XRCCTRL(*this, "chkUnpadParens",        wxCheckBox)->GetValue());
  cfg->Write(_T("/delete_empty_lines"),   XRCCTRL(*this, "chkDelEmptyLine",       wxCheckBox)->GetValue());
  cfg->Write(_T("/keep_complex"),         XRCCTRL(*this, "chkKeepComplex",        wxCheckBox)->GetValue());
  cfg->Write(_T("/keep_blocks"),          XRCCTRL(*this, "chkKeepBlocks",         wxCheckBox)->GetValue());
  cfg->Write(_T("/convert_tabs"),         XRCCTRL(*this, "chkConvertTabs",        wxCheckBox)->GetValue());
  cfg->Write(_T("/fill_empty_lines"),     XRCCTRL(*this, "chkFillEmptyLines",     wxCheckBox)->GetValue());
  cfg->Write(_T("/add_brackets"),         XRCCTRL(*this, "chkAddBrackets",        wxCheckBox)->GetValue());
}
