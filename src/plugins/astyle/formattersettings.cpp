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
    int style = ConfigManager::Get()->Read("/astyle/style", 0L);
    switch(style)
    {
        case 0: // ansi
            formatter.setBracketIndent(false);
            formatter.setSpaceIndentation(4);
            formatter.setBracketFormatMode(BREAK_MODE);
            formatter.setClassIndent(false);
            formatter.setSwitchIndent(false);
            formatter.setNamespaceIndent(false); 
            break;
        
        case 1: // K&R
            formatter.setBracketIndent(false);
            formatter.setSpaceIndentation(4);
            formatter.setBracketFormatMode(ATTACH_MODE);
            formatter.setClassIndent(false);
            formatter.setSwitchIndent(false);
            formatter.setNamespaceIndent(false);
            break;
            
        case 2: // Linux
            formatter.setBracketIndent(false);
            formatter.setSpaceIndentation(8);
            formatter.setBracketFormatMode(BDAC_MODE);
            formatter.setClassIndent(false);
            formatter.setSwitchIndent(false);
            formatter.setNamespaceIndent(false);
            break;
            
        case 3: // GNU
            formatter.setBlockIndent(true);
            formatter.setSpaceIndentation(2);
            formatter.setBracketFormatMode(BREAK_MODE);
            formatter.setClassIndent(false);
            formatter.setSwitchIndent(false);
            formatter.setNamespaceIndent(false);
            break;
            
        case 4: // Java
            formatter.setJavaStyle();
            formatter.setBracketIndent(false);
            formatter.setSpaceIndentation(4);
            formatter.setBracketFormatMode(ATTACH_MODE);
            formatter.setSwitchIndent(false);
            break;
            
        default: // Custom
        {
            int spaceNum = ConfigManager::Get()->Read("/astyle/indentation", 4);
            if (ConfigManager::Get()->Read("/astyle/use_tabs"))
                formatter.setTabIndentation(spaceNum, false);
            else
                formatter.setSpaceIndentation(spaceNum);
            if (ConfigManager::Get()->Read("/astyle/force_tabs"))
                formatter.setTabIndentation(spaceNum, true);
            if (ConfigManager::Get()->Read("/astyle/convert_tabs"))
                formatter.setTabSpaceConversionMode(true); 
            if (ConfigManager::Get()->Read("/astyle/fill_empty_lines"))
                formatter.setEmptyLineFill(true);
            if (ConfigManager::Get()->Read("/astyle/indent_classes"))
                formatter.setClassIndent(true);
            if (ConfigManager::Get()->Read("/astyle/indent_switches"))
                formatter.setSwitchIndent(true);
            if (ConfigManager::Get()->Read("/astyle/indent_case"))
                formatter.setCaseIndent(true);
            if (ConfigManager::Get()->Read("/astyle/indent_brackets"))
                formatter.setBracketIndent(true); 
            if (ConfigManager::Get()->Read("/astyle/indent_blocks"))
                formatter.setBlockIndent(true);
            if (ConfigManager::Get()->Read("/astyle/indent_namespaces"))
                formatter.setNamespaceIndent(true);
            if (ConfigManager::Get()->Read("/astyle/indent_labels"))
                formatter.setLabelIndent(true);
            if (ConfigManager::Get()->Read("/astyle/indent_preprocessor"))
                formatter.setPreprocessorIndent(true);
            if (ConfigManager::Get()->Read("/astyle/break_blocks"))
                formatter.setBreakBlocksMode(true);
            if (ConfigManager::Get()->Read("/astyle/break_elseifs"))
                formatter.setBreakElseIfsMode(true);
            if (ConfigManager::Get()->Read("/astyle/pad_operators"))
                formatter.setOperatorPaddingMode(true);
            if (ConfigManager::Get()->Read("/astyle/pad_parentheses"))
                formatter.setParenthesisPaddingMode(true);
            if (ConfigManager::Get()->Read("/astyle/keep_complex"))
                formatter.setSingleStatementsMode(false);
            if (ConfigManager::Get()->Read("/astyle/keep_blocks"))
                formatter.setBreakOneLineBlocksMode(false);
            break;
        }
    }
}
