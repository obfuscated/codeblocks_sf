#ifndef COMPILERDMC_H
#define COMPILERDMC_H

#include <compiler.h>

class CompilerDMC : public Compiler
{
	public:
		CompilerDMC();
		virtual ~CompilerDMC();
		virtual Compiler::CompilerLineType CheckForWarningsAndErrors(const wxString& line);
        virtual AutoDetectResult AutoDetectInstallationDir();
	protected:
        Compiler * CreateCopy();
	private:
};

#endif // COMPILERDMC_H

