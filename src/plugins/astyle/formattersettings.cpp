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
      int spaceNum = cfg->ReadInt(_T("/indentation"), 4);

      formatter.modeSetManually = false;
      formatter.indentLength = spaceNum;

      if (cfg->ReadBool(_T("/use_tabs")))
      {
        formatter.indentString = '\t';
      }
      else
      {
        formatter.indentString = string(spaceNum, ' ');
      }

      value = cfg->ReadBool(_T("/force_tabs"));

      if (value)
      {
        formatter.indentString = '\t';
        formatter.forceTabIndent = true;
      }
      else
      {
        formatter.forceTabIndent = false;
      }

      formatter.convertTabs2Space = cfg->ReadBool(_T("/convert_tabs"));
      formatter.emptyLineIndent = cfg->ReadBool(_T("/fill_empty_lines"));
      formatter.classIndent = cfg->ReadBool(_T("/indent_classes"));
      formatter.switchIndent = cfg->ReadBool(_T("/indent_switches"));
      formatter.caseIndent = cfg->ReadBool(_T("/indent_case"));
      formatter.bracketIndent = cfg->ReadBool(_T("/indent_brackets"));
      formatter.blockIndent = cfg->ReadBool(_T("/indent_blocks"));
      formatter.namespaceIndent = cfg->ReadBool(_T("/indent_namespaces"));
      formatter.labelIndent = cfg->ReadBool(_T("/indent_labels"));
      formatter.preprocessorIndent = cfg->ReadBool(_T("/indent_preprocessor"));

      wxString breakType = cfg->Read(_T("/break_type"));

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

      formatter.breakBlocks = cfg->ReadBool(_T("/break_blocks"));
      formatter.breakElseIfs = cfg->ReadBool(_T("/break_elseifs"));
      formatter.padOperators = cfg->ReadBool(_T("/pad_operators"));
      formatter.padParen = cfg->ReadBool(_T("/pad_parentheses"));
      formatter.breakOneLineStatements = !cfg->ReadBool(_T("/keep_complex"));
      formatter.breakOneLineBlocks = !cfg->ReadBool(_T("/keep_blocks"));
      break;
    }
  }
}
