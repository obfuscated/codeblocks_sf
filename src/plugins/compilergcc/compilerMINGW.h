#ifndef COMPILER_MINGW_H
#define COMPILER_MINGW_H

#include <compiler.h>

class CompilerMINGW : public Compiler
{
	public:
		CompilerMINGW();
		virtual ~CompilerMINGW();
		virtual Compiler::CompilerLineType CheckForWarningsAndErrors(const wxString& line);
        virtual AutoDetectResult AutoDetectInstallationDir();
    protected:
        virtual Compiler* CreateCopy();
	private:
};

#endif // COMPILER_MINGW_H
