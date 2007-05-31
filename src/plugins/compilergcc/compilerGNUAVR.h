#ifndef COMPILER_GNUAVR_H
#define COMPILER_GNUAVR_H

#include "compiler.h"

class CompilerGNUAVR : public Compiler
{
    public:
        CompilerGNUAVR();
        virtual ~CompilerGNUAVR();
        virtual void Reset();
        virtual void LoadDefaultRegExArray();
        virtual AutoDetectResult AutoDetectInstallationDir();
    protected:
        virtual Compiler* CreateCopy();
};

#endif // COMPILER_GNUAVR_H
