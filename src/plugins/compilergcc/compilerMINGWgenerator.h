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
