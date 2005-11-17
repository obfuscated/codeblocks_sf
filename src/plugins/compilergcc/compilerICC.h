/********************************************************************
	filename:		compilerICC.h
	created by:	Yorgos Pagles (yop [at] protiamail.gr)
	description:	Support of Intel's ICC compiler for CodeBlocks IDE
********************************************************************/
#ifndef COMPILER_ICC_H
#define COMPILER_ICC_H

#include <compiler.h>

class CompilerICC : public Compiler
{
	public:
		CompilerICC();
		virtual ~CompilerICC();
        virtual void Reset();
		virtual void LoadDefaultRegExArray();
        virtual AutoDetectResult AutoDetectInstallationDir();
    protected:
        virtual Compiler* CreateCopy();
	private:
};

#endif // COMPILER_ICC_H
