#include <sdk.h>
#include "formattersettings.h"
#include <configmanager.h>

FormatterSettings::FormatterSettings()
{
  //ctor
}

FormatterSettings::~FormatterSettings()
{
  //dtor
}

void FormatterSettings::ApplyTo(astyle::ASFormatter& formatter)
{
  ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("astyle"));

  int style = cfg->ReadInt(_T("/style"), 0);

  switch (style)
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
      formatter.setBreakClosingHeaderBracketsMode(false);
      formatter.setBreakBlocksMode(false);
      formatter.setBreakElseIfsMode(false);
      formatter.setOperatorPaddingMode(false);
      formatter.setParensOutsidePaddingMode(false);
      formatter.setParensInsidePaddingMode(false);
      formatter.setParensUnPaddingMode(false);
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
      formatter.setBreakClosingHeaderBracketsMode(false);
      formatter.setBreakBlocksMode(false);
      formatter.setBreakElseIfsMode(false);
      formatter.setOperatorPaddingMode(false);
      formatter.setParensInsidePaddingMode(false);
      formatter.setParensOutsidePaddingMode(false);
      formatter.setParensUnPaddingMode(false);
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
      formatter.setBreakClosingHeaderBracketsMode(false);
      formatter.setBreakBlocksMode(false);
      formatter.setBreakElseIfsMode(false);
      formatter.setOperatorPaddingMode(false);
      formatter.setParensOutsidePaddingMode(false);
      formatter.setParensInsidePaddingMode(false);
      formatter.setParensUnPaddingMode(false);
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
      formatter.setBreakClosingHeaderBracketsMode(false);
      formatter.setBreakBlocksMode(false);
      formatter.setBreakElseIfsMode(false);
      formatter.setOperatorPaddingMode(false);
      formatter.setParensOutsidePaddingMode(false);
      formatter.setParensInsidePaddingMode(false);
      formatter.setParensUnPaddingMode(false);
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
      formatter.setBreakClosingHeaderBracketsMode(false);
      formatter.setBreakBlocksMode(false);
      formatter.setBreakElseIfsMode(false);
      formatter.setOperatorPaddingMode(false);
      formatter.setParensOutsidePaddingMode(false);
      formatter.setParensInsidePaddingMode(false);
      formatter.setParensUnPaddingMode(false);
      formatter.setSingleStatementsMode(true);
      formatter.setBreakOneLineBlocksMode(true);
      break;

    default: // Custom
    {
      bool value = cfg->ReadBool(_T("/force_tabs"));
      int spaceNum = cfg->ReadInt(_T("/indentation"), 4);

      if (cfg->ReadBool(_T("/use_tabs")))
      {
        formatter.setTabIndentation(spaceNum, value);
      }
      else
      {
        formatter.setSpaceIndentation(spaceNum);
      }

      formatter.setClassIndent(cfg->ReadBool(_T("/indent_classes")));
      formatter.setSwitchIndent(cfg->ReadBool(_T("/indent_switches")));
      formatter.setCaseIndent(cfg->ReadBool(_T("/indent_case")));
      formatter.setBracketIndent(cfg->ReadBool(_T("/indent_brackets")));
      formatter.setBlockIndent(cfg->ReadBool(_T("/indent_blocks")));
      formatter.setNamespaceIndent(cfg->ReadBool(_T("/indent_namespaces")));
      formatter.setLabelIndent(cfg->ReadBool(_T("/indent_labels")));
      formatter.setPreprocessorIndent(cfg->ReadBool(_T("/indent_preprocessor")));

      wxString breakType = cfg->Read(_T("/break_type"));

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

      formatter.setBreakClosingHeaderBracketsMode(cfg->ReadBool(_T("/break_closing")));
      formatter.setBreakBlocksMode(cfg->ReadBool(_T("/break_blocks")));
      formatter.setBreakElseIfsMode(cfg->ReadBool(_T("/break_elseifs")));
      formatter.setOperatorPaddingMode(cfg->ReadBool(_T("/pad_operators")));
      formatter.setParensOutsidePaddingMode(cfg->ReadBool(_T("/pad_parentheses_out")));
      formatter.setParensInsidePaddingMode(cfg->ReadBool(_T("/pad_parentheses_in")));
      formatter.setParensUnPaddingMode(cfg->ReadBool(_T("/unpad_parentheses")));
      formatter.setSingleStatementsMode(!cfg->ReadBool(_T("/keep_complex")));
      formatter.setBreakOneLineBlocksMode(!cfg->ReadBool(_T("/keep_blocks")));
      formatter.setTabSpaceConversionMode(cfg->ReadBool(_T("/convert_tabs")));
      formatter.setEmptyLineFill(cfg->ReadBool(_T("/fill_empty_lines")));
      break;
    }
  }
}
