#ifndef COMPILERBCC_H
#define COMPILERBCC_H

#include <compiler.h>

class CompilerBCC : public Compiler
{
	public:
		CompilerBCC();
		virtual ~CompilerBCC();
        virtual void Reset();
        virtual AutoDetectResult AutoDetectInstallationDir();
	protected:
        Compiler * CreateCopy();
	private:
};

#endif // COMPILERBCC_H
