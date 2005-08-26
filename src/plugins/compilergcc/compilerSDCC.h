#ifndef COMPILER_SDCC_H
#define COMPILER_SDCC_H

#include <compiler.h>

class CompilerSDCC : public Compiler
{
	public:
		CompilerSDCC();
		virtual ~CompilerSDCC();
        virtual void Reset();
		virtual void LoadDefaultRegExArray();
        virtual AutoDetectResult AutoDetectInstallationDir();
    protected:
        virtual Compiler* CreateCopy();
	private:
};

#endif // COMPILER_SDCC_H
