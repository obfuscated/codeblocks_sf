#ifndef COMPILER_MINGW_H
#define COMPILER_MINGW_H

#include <compiler.h>

class CompilerMINGW : public Compiler
{
	public:
		CompilerMINGW();
		virtual ~CompilerMINGW();
    protected:
        virtual Compiler* CreateCopy();
	private:
};

#endif // COMPILER_MINGW_H
