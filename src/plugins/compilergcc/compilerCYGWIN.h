#ifndef COMPILER_CYGWIN_H
#define COMPILER_CYGWIN_H

#ifdef __WXMSW__

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

#endif // __WXMSW__

#endif // COMPILER_CYGWIN_H
