#if defined(_WIN32) || defined(linux)
// this compiler is valid only in windows and linux

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

#endif // COMPILERDMD_H

#endif // _WIN32 || linux
