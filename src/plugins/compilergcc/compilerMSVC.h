#ifndef COMPILERMSVC_H
#define COMPILERMSVC_H

#include <compiler.h>

class CompilerMSVC : public Compiler
{
	public:
		CompilerMSVC();
		virtual ~CompilerMSVC();
	protected:
        Compiler * CreateCopy();
	private:
};

#endif // COMPILERMSVC_H

