#ifdef __WXMSW__
// this compiler is valid only in windows

#ifndef COMPILERBCC_H
#define COMPILERBCC_H

#include <compiler.h>

class CompilerBCC : public Compiler
{
	public:
		CompilerBCC();
		virtual ~CompilerBCC();
        virtual void Reset();
		virtual void LoadDefaultRegExArray();
        virtual AutoDetectResult AutoDetectInstallationDir();
	protected:
        Compiler * CreateCopy();
	private:
};

#endif // COMPILERBCC_H

#endif // __WXMSW__
