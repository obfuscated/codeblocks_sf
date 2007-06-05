#ifdef __WXMSW__
// this compiler is valid only in windows

#ifndef COMPILER_CYGWIN_H
#define COMPILER_CYGWIN_H

#include "compilerMINGW.h"

class CompilerCYGWIN : public CompilerMINGW
{
	public:
		CompilerCYGWIN();
		~CompilerCYGWIN();
        virtual void Reset();
        virtual AutoDetectResult AutoDetectInstallationDir();
	protected:
        virtual Compiler* CreateCopy();
	private:

};

#endif // COMPILER_CYGWIN_H

#endif // __WXMSW__
