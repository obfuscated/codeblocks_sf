#ifdef __WXMSW__
// this compiler is valid only in windows

#ifndef COMPILERDMD_H
#define COMPILERDMD_H

#include <compiler.h>

class CompilerDMD : public Compiler
{
	public:
		CompilerDMD();
		virtual ~CompilerDMD();
        virtual void Reset();
		virtual void LoadDefaultRegExArray();
        virtual AutoDetectResult AutoDetectInstallationDir();
	protected:
        Compiler * CreateCopy();
	private:
};

#endif // COMPILERDMC_H

#endif // __WXMSW__
