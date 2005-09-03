#ifdef __WXMSW__
// this compiler is valid only in windows

#ifndef COMPILERDMC_H
#define COMPILERDMC_H

#include <compiler.h>

class CompilerDMC : public Compiler
{
	public:
		CompilerDMC();
		virtual ~CompilerDMC();
        virtual void Reset();
		virtual void LoadDefaultRegExArray();
        virtual AutoDetectResult AutoDetectInstallationDir();
	protected:
        Compiler * CreateCopy();
	private:
};

#endif // COMPILERDMC_H

#endif // __WXMSW__
