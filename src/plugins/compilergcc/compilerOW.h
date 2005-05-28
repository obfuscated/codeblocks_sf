#ifndef COMPILEROW_H
#define COMPILEROW_H

#include <compiler.h>

class CompilerOW : public Compiler
{
	public:
		CompilerOW();
		virtual ~CompilerOW();
        virtual void Reset();
        virtual AutoDetectResult AutoDetectInstallationDir();

        virtual void LoadSettings(const wxString& baseKey);
		virtual void SetMasterPath(const wxString& path);
	protected:
        Compiler * CreateCopy();
	private:
};

#endif // COMPILEROW_H
