#ifndef COMPILER_MINGW_H
#define COMPILER_MINGW_H

#include <compiler.h>

class CompilerMINGW : public Compiler
{
	public:
		CompilerMINGW();
		virtual ~CompilerMINGW();
        virtual void Reset();
		virtual void LoadDefaultRegExArray();
        virtual AutoDetectResult AutoDetectInstallationDir();
        virtual CompilerCommandGenerator* GetCommandGenerator();
    protected:
        virtual Compiler* CreateCopy();
	private:
};

#endif // COMPILER_MINGW_H
