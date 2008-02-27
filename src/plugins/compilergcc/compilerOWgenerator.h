/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifdef __WXMSW__ // For Windows Only

#ifndef COMPILEROWGENERATOR_H
#define COMPILEROWGENERATOR_H

#include <compilercommandgenerator.h>

//Overriden to fix OpenWatcom Linking Problem (@Biplab)

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
};



#endif // COMPILEROWGENERATOR_H

#endif // __WXMSW__
