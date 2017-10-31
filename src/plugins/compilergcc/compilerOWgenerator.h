/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef COMPILEROWGENERATOR_H
#define COMPILEROWGENERATOR_H

#include <compilercommandgenerator.h>

//Overridden to fix OpenWatcom Linking Problem (@Biplab)

class CompilerOWGenerator : public CompilerCommandGenerator
{
    public:
        CompilerOWGenerator();
        virtual ~CompilerOWGenerator();
        virtual wxString SetupLibrariesDirs(Compiler* compiler, ProjectBuildTarget* target);
        virtual wxString SetupLinkerOptions(Compiler* compiler, ProjectBuildTarget* target);
        virtual wxString SetupLinkLibraries(Compiler* compiler, ProjectBuildTarget* target);
    protected:
    private:
        // This maps target type to wlink.exe options
        wxString MapTargetType(const wxString& Opt, int target_type);
        // This maps debug related compiler options to wlink.exe options
        wxString MapDebugOptions(const wxString& Opt);
        // Open Watcom supports: Watcom debugger, Dwarf (default), CodeView
        void MapDebuggerOptions(const wxString& Opt);

    private:
        // Debugger name which will later be used in MapDebugOptions
        wxString m_DebuggerType;
};



#endif // COMPILEROWGENERATOR_H
