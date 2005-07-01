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
            formatter.bracketIndent = false;
            formatter.indentLength = 4;
            formatter.indentString = "    ";
            formatter.bracketFormatMode = astyle::BREAK_MODE;
            formatter.classIndent = false;
            formatter.switchIndent = false;
            formatter.namespaceIndent = false; 
            break;
        
        case 1: // K&R
            formatter.bracketIndent = false;
            formatter.indentLength = 4;
            formatter.indentString = "    ";
            formatter.bracketFormatMode = astyle::ATTACH_MODE;
            formatter.classIndent = false;
            formatter.switchIndent = false;
            formatter.namespaceIndent = false;
            break;
            
        case 2: // Linux
            formatter.bracketIndent = false;
            formatter.indentLength = 8;
            formatter.indentString = "        ";
            formatter.bracketFormatMode = astyle::BDAC_MODE;
            formatter.classIndent = false;
            formatter.switchIndent = false;
            formatter.namespaceIndent = false;
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
            break;
            
        case 4: // Java
            formatter.sourceStyle = astyle::STYLE_JAVA;
            formatter.modeSetManually = true;
            formatter.bracketIndent = false;
            formatter.indentLength = 4;
            formatter.indentString = "    ";
            formatter.bracketFormatMode = astyle::ATTACH_MODE;
            formatter.switchIndent = false;
            break;
            
        default: // Custom
        {
            int spaceNum = ConfigManager::Get()->Read("/astyle/indentation", 4);
            bool value;

            formatter.modeSetManually = false;
            formatter.indentLength = spaceNum;
            formatter.bracketFormatMode = astyle::BREAK_MODE; // Add control
            
            ConfigManager::Get()->Read("/astyle/use_tabs", &value);
            if (value)
            {
                formatter.indentString = '\t';
            }
            else
            {
                formatter.indentString = string(spaceNum, ' ');
            }

            ConfigManager::Get()->Read("/astyle/force_tabs", &value);
            if (value)
            {
                formatter.indentString = '\t';
                formatter.forceTabIndent = true;
            }
            else
            {
                formatter.forceTabIndent = false;
            }

            ConfigManager::Get()->Read("/astyle/convert_tabs", &value);
            formatter.convertTabs2Space = value;
            
            ConfigManager::Get()->Read("/astyle/fill_empty_lines", &value);
            formatter.emptyLineIndent = value;
            
            ConfigManager::Get()->Read("/astyle/indent_classes", &value);
            formatter.classIndent = value;
            
            ConfigManager::Get()->Read("/astyle/indent_switches", &value);
            formatter.switchIndent = value;
            
            ConfigManager::Get()->Read("/astyle/indent_case", &value);
            formatter.caseIndent = value;
            
            ConfigManager::Get()->Read("/astyle/indent_brackets", &value);
            formatter.bracketIndent = value;
            
            ConfigManager::Get()->Read("/astyle/indent_blocks", &value);
            formatter.blockIndent = value;
            
            ConfigManager::Get()->Read("/astyle/indent_namespaces", &value);
            formatter.namespaceIndent = value;
            
            ConfigManager::Get()->Read("/astyle/indent_labels", &value);
            formatter.labelIndent = value;
            
            ConfigManager::Get()->Read("/astyle/indent_preprocessor", &value);
            formatter.preprocessorIndent = value;
            
            wxString breakType = ConfigManager::Get()->Read("/astyle/break_type");
            if (breakType == "Break")
            {
            	formatter.bracketFormatMode = astyle::BREAK_MODE;
            }
            else if (breakType == "Attach")
            {
            	formatter.bracketFormatMode = astyle::ATTACH_MODE;
            }
            else if (breakType == "Linux")
            {
            	formatter.bracketFormatMode = astyle::BDAC_MODE;
            }
            else
            {
            	formatter.bracketFormatMode = astyle::NONE_MODE;
            }
            
            ConfigManager::Get()->Read("/astyle/break_blocks", &value);
            formatter.breakBlocks = value;
            
            ConfigManager::Get()->Read("/astyle/break_elseifs", &value);
            formatter.breakElseIfs = value;
            
            ConfigManager::Get()->Read("/astyle/pad_operators", &value);
            formatter.padOperators = value;
            
            ConfigManager::Get()->Read("/astyle/pad_parentheses", &value);
            formatter.padParen = value;
            
            ConfigManager::Get()->Read("/astyle/keep_complex", &value);
            formatter.breakOneLineStatements = !value;
            
            ConfigManager::Get()->Read("/astyle/keep_blocks", &value);
            formatter.breakOneLineBlocks = !value;
            break;
        }
    }
}
