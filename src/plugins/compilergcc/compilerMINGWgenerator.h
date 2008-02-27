/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef COMPILERMINGWGENERATOR_H
#define COMPILERMINGWGENERATOR_H

#include <compilercommandgenerator.h>

// Overriden to support PCH for GCC
class CompilerMINGWGenerator : public CompilerCommandGenerator
{
    public:
        CompilerMINGWGenerator();
        virtual ~CompilerMINGWGenerator();
    protected:
        virtual wxString SetupIncludeDirs(Compiler* compiler, ProjectBuildTarget* target);
    private:
        wxString m_VerStr;
};

#endif // COMPILERMINGWGENERATOR_H
