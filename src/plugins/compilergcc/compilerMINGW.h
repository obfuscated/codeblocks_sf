#ifndef COMPILER_MINGW_H
#define COMPILER_MINGW_H

#include <compiler.h>

class CompilerMINGW : public Compiler
{
	public:
		CompilerMINGW();
		virtual ~CompilerMINGW();
        virtual void Reset();
        virtual AutoDetectResult AutoDetectInstallationDir();
    protected:
        virtual Compiler* CreateCopy();
	private:
};

#endif // COMPILER_MINGW_H
