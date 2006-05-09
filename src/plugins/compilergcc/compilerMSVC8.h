#ifdef __WXMSW__
// this compiler is valid only in windows

#ifndef COMPILERMSVC8_H
#define COMPILERMSVC8_H

#include <compiler.h>

class CompilerMSVC8 : public Compiler
{
	public:
		CompilerMSVC8();
		virtual ~CompilerMSVC8();
        virtual void Reset();
		virtual void LoadDefaultRegExArray();
        virtual AutoDetectResult AutoDetectInstallationDir();
	protected:
        Compiler * CreateCopy();
	private:
};

#endif // COMPILERMSVC8_H

#endif // __WXMSW__
