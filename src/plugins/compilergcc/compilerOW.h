#ifdef __WXMSW__
// this compiler is valid only in windows

#ifndef COMPILEROW_H
#define COMPILEROW_H

#include <compiler.h>

class CompilerOW : public Compiler
{
	public:
		CompilerOW();
		virtual ~CompilerOW();
        virtual void Reset();
		virtual void LoadDefaultRegExArray();
        virtual AutoDetectResult AutoDetectInstallationDir();

        virtual void LoadSettings(const wxString& baseKey);
		virtual void SetMasterPath(const wxString& path);
		virtual CompilerCommandGenerator* GetCommandGenerator();
	protected:
        Compiler * CreateCopy();
	private:
};

#endif // COMPILEROW_H

#endif // __WXMSW__
