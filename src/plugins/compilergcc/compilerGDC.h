#ifndef COMPILER_GDC_H
#define COMPILER_GDC_H

#include <compiler.h>

class CompilerGDC : public Compiler
{
    public:
        CompilerGDC();
        virtual ~CompilerGDC();
        virtual void Reset();
        virtual void LoadDefaultRegExArray();
        virtual AutoDetectResult AutoDetectInstallationDir();
    protected:
        virtual Compiler* CreateCopy();
    private:
};

#endif // COMPILER_MINGW_H
