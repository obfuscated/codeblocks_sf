#ifndef COMPILERMSVC_H
#define COMPILERMSVC_H

#include <compiler.h>

class CompilerMSVC : public Compiler
{
	public:
		CompilerMSVC();
		virtual ~CompilerMSVC();
		virtual Compiler::CompilerLineType CheckForWarningsAndErrors(const wxString& line);
        virtual AutoDetectResult AutoDetectInstallationDir();
	protected:
        Compiler * CreateCopy();
	private:
};

#endif // COMPILERMSVC_H

