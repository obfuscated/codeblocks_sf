#ifndef COMPILER_GNUPOWERPC_H
#define COMPILER_GNUPOWERPC_H
#include <compiler.h>
class CompilerGNUPOWERPC : public Compiler
{
	public:
		CompilerGNUPOWERPC();
		virtual ~CompilerGNUPOWERPC();
		virtual void Reset();
		virtual void LoadDefaultRegExArray();
		virtual AutoDetectResult AutoDetectInstallationDir();
	protected:
		virtual Compiler* CreateCopy();
};
#endif // COMPILER_GNUPOWERPC_H
