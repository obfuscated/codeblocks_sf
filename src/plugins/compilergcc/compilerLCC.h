#ifdef __WXMSW__
// this compiler is valid only in windows

#ifndef COMPILER_LCC_H
#define COMPILER_LCC_H

#include <compiler.h>

class CompilerLCC : public Compiler
{
    public:
        CompilerLCC();
        virtual ~CompilerLCC();
        virtual bool IsValid();
        virtual void Reset();
        virtual void LoadDefaultRegExArray();
        virtual AutoDetectResult AutoDetectInstallationDir();
    protected:
        virtual Compiler* CreateCopy();
    private:
        bool m_RegistryUpdated;
};

#endif // COMPILER_LCC_H

#endif // __WXMSW__
