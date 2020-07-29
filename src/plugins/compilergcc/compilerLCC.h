#ifndef COMPILER_LCC_H
#define COMPILER_LCC_H

#include <compiler.h>

class CompilerLCC : public Compiler
{
    public:
        CompilerLCC();
        ~CompilerLCC() override;
#ifdef __WXMSW__
        bool IsValid() override;
#endif // __WXMSW__
        void Reset() override;
        AutoDetectResult AutoDetectInstallationDir() override;
    protected:
        Compiler* CreateCopy() override;
    private:
        bool m_RegistryUpdated;
};

#endif // COMPILER_LCC_H
