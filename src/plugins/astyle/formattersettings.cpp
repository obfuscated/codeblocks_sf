/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include "formattersettings.h"
#include "astylepredefinedstyles.h"
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
  // NOTE: Keep this in sync with DlgFormatterSettings::ApplyTo
  ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("astyle"));

  int style = cfg->ReadInt(_T("/style"), 0);

  switch (style)
  {
    case aspsAllman: // Allman (ANSI)
      formatter.setFormattingStyle(astyle::STYLE_ALLMAN);
      break;

    case aspsJava: // Java
      formatter.setFormattingStyle(astyle::STYLE_JAVA);
      break;

    case aspsKr: // K&R
      formatter.setFormattingStyle(astyle::STYLE_KR);
      break;

    case aspsStroustrup: // Stroustrup
      formatter.setFormattingStyle(astyle::STYLE_STROUSTRUP);
      break;

    case aspsWhitesmith: // Whitesmith
      formatter.setFormattingStyle(astyle::STYLE_WHITESMITH);
      break;

    case aspsBanner: // Banner
      formatter.setFormattingStyle(astyle::STYLE_BANNER);
      break;

    case aspsGnu: // GNU
      formatter.setFormattingStyle(astyle::STYLE_GNU);
      break;

    case aspsLinux: // Linux
      formatter.setFormattingStyle(astyle::STYLE_LINUX);
      break;

    case aspsHorstmann: // Horstmann
      formatter.setFormattingStyle(astyle::STYLE_HORSTMANN);
      break;

    case asps1TBS: // 1TBS
      formatter.setFormattingStyle(astyle::STYLE_1TBS);
      break;

    case aspsPico: // Pico
      formatter.setFormattingStyle(astyle::STYLE_PICO);
      break;

    case aspsLisp: // Lisp
      formatter.setFormattingStyle(astyle::STYLE_LISP);
      break;

    default: // Custom
      break;
  }

  bool value = cfg->ReadBool(_T("/force_tabs"));
  int spaceNum = cfg->ReadInt(_T("/indentation"), 4);

  if (cfg->ReadBool(_T("/use_tabs")))
    formatter.setTabIndentation(spaceNum, value);
  else
    formatter.setSpaceIndentation(spaceNum);

  formatter.setClassIndent(cfg->ReadBool(_T("/indent_classes")));
  formatter.setSwitchIndent(cfg->ReadBool(_T("/indent_switches")));
  formatter.setCaseIndent(cfg->ReadBool(_T("/indent_case")));
  formatter.setBracketIndent(cfg->ReadBool(_T("/indent_brackets")));
  formatter.setBlockIndent(cfg->ReadBool(_T("/indent_blocks")));
  formatter.setNamespaceIndent(cfg->ReadBool(_T("/indent_namespaces")));
  formatter.setLabelIndent(cfg->ReadBool(_T("/indent_labels")));
  formatter.setPreprocessorIndent(cfg->ReadBool(_T("/indent_preprocessor")));
  formatter.setIndentCol1CommentsMode(cfg->ReadBool(_T("/indent_col1_comments")));

  wxString pointerAlign = cfg->Read(_T("/pointer_align"));

  if      (pointerAlign == _T("Type"))
    formatter.setPointerAlignment(astyle::PTR_ALIGN_TYPE);
  else if (pointerAlign == _T("Middle"))
    formatter.setPointerAlignment(astyle::PTR_ALIGN_MIDDLE);
  else if (pointerAlign == _T("Name"))
    formatter.setPointerAlignment(astyle::PTR_ALIGN_NAME);
  else
    formatter.setPointerAlignment(astyle::PTR_ALIGN_NONE);

  formatter.setBreakClosingHeaderBracketsMode(cfg->ReadBool(_T("/break_closing")));
  formatter.setBreakBlocksMode(cfg->ReadBool(_T("/break_blocks")));
  formatter.setBreakElseIfsMode(cfg->ReadBool(_T("/break_elseifs")));
  formatter.setOperatorPaddingMode(cfg->ReadBool(_T("/pad_operators")));
  formatter.setParensOutsidePaddingMode(cfg->ReadBool(_T("/pad_parentheses_out")));
  formatter.setParensInsidePaddingMode(cfg->ReadBool(_T("/pad_parentheses_in")));
  formatter.setParensHeaderPaddingMode(cfg->ReadBool(_T("/pad_header")));
  formatter.setParensUnPaddingMode(cfg->ReadBool(_T("/unpad_parentheses")));
  formatter.setDeleteEmptyLinesMode(cfg->ReadBool(_T("/delete_empty_lines")));
  formatter.setSingleStatementsMode(!cfg->ReadBool(_T("/keep_complex")));
  formatter.setBreakOneLineBlocksMode(!cfg->ReadBool(_T("/keep_blocks")));
  formatter.setTabSpaceConversionMode(cfg->ReadBool(_T("/convert_tabs")));
  formatter.setEmptyLineFill(cfg->ReadBool(_T("/fill_empty_lines")));
  formatter.setAddBracketsMode(cfg->ReadBool(_T("/add_brackets")));

  if (cfg->ReadBool(_T("/break_lines")))
    formatter.setMaxCodeLength( wxAtoi(cfg->Read(_T("/max_line_length"))));
  else
    formatter.setMaxCodeLength(INT_MAX);
}
